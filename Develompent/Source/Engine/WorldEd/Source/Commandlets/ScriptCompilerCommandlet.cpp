#include "Reflection/Class.h"
#include "Misc/CoreGlobals.h"
#include "Logger/LoggerMacros.h"
#include "System/BaseFileSystem.h"
#include "System/Config.h"
#include "Commandlets/ScriptCompilerCommandlet.h"
#include "Reflection/ReflectionEnvironment.h"
#include "Reflection/ObjectPackage.h"
#include "Reflection/ObjectIterator.h"
#include "Scripts/ScriptEnvironmentBuilder.h"

IMPLEMENT_CLASS( CScriptCompilerCommandlet )
IMPLEMENT_DEFAULT_INITIALIZE_CLASS( CScriptCompilerCommandlet )

/*
==================
CScriptCompilerCommandlet::CScriptCompilerCommandlet
==================
*/
CScriptCompilerCommandlet::CScriptCompilerCommandlet()
	: contentDir( CString::Format( TEXT( "%s" ) PATH_SEPARATOR TEXT( "Content" ) PATH_SEPARATOR, Sys_GameDir().c_str() ) )
	, scriptsDir( CString::Format( TEXT( "%s" ) PATH_SEPARATOR TEXT( "Scripts" ) PATH_SEPARATOR, Sys_GameDir().c_str() ) )
	, outputFile( CString::Format( TEXT( "%s" ) PATH_SEPARATOR TEXT( "%s.classes" ), contentDir.c_str(), g_GameName.c_str() ) )
{}

/*
==================
CScriptCompilerCommandlet::Main
==================
*/
bool CScriptCompilerCommandlet::Main( const CCommandLine& InCommandLine )
{
	// Path to directory with scripts
	CObjectPackage*			scriptPackage = CObjectPackage::CreatePackage( nullptr, g_GameName.c_str() );
	CScriptSystemStub		stubs( scriptPackage );
	scriptPackage->AddToRoot();

	// Parse script classes
	Logf( TEXT( "Build classes..\n" ) );
	bool	bResult = ParseScriptClasses( scriptsDir, stubs );
	if ( !bResult )
	{
		Errorf( TEXT( "Failed to parse script classes from directory '%s'\n" ), scriptsDir.c_str() );
		return false;
	}

	// Build environment for script compiler
	CScriptEnvironmentBuilder		builder( stubs );
	if ( !builder.Build() )
	{
		Errorf( TEXT( "Failed to build environment for script compiler\n" ) );
		return false;
	}

	// Compile script classes
	Logf( TEXT( "Compile classes..\n" ) );
	bResult = CompileScriptClasses( stubs );
	if ( !bResult )
	{
		Errorf( TEXT( "Failed to compile script classes from directory '%s'\n" ), scriptsDir.c_str() );
		return false;
	}

	// Save compiled script classes
	bResult = CObjectPackage::SavePackage( scriptPackage, nullptr, 0, outputFile.c_str() );
	if ( !bResult )
	{
		Errorf( TEXT( "Failed to save script classes '%s'\n" ), outputFile.c_str() );
		return false;
	}

	scriptPackage->RemoveFromRoot();
	return true;
}

/*
==================
CScriptCompilerCommandlet::ParseScriptClasses
==================
*/
bool CScriptCompilerCommandlet::ParseScriptClasses( const std::wstring& InScriptDir, CScriptSystemStub& InOutStubs )
{
	// Grab all files from directory
	bool						bResult = false;
	std::vector<std::wstring>	files = g_FileSystem->FindFiles( InScriptDir, true, true );

	for ( uint32 index = 0, count = files.size(); index < count; ++index )
	{
		// If the file is a directory look in
		CFilename		filename = InScriptDir + files[index];
		if ( filename.GetExtension().empty() )
		{
			ParseScriptClasses( InScriptDir + filename.GetFullPath(), InOutStubs );
			continue;
		}

		// If the file has 'class' extension its our script class and parse it
		if ( filename.GetExtension() == TEXT( "class" ) )
		{
			// Load JSON file
			CConfig		jsonScriptClass;
			bool		bLocalResult = jsonScriptClass.LoadFile( filename.GetFullPath() );
			if ( !bLocalResult )
			{
				Errorf( TEXT( "Failed to load file '%s'\n" ), filename.GetFullPath().c_str() );
			}

			// Grab all JSON values and init script class description
			if ( bLocalResult )
			{
				// Get ScriptClass:Class from JSON file
				CConfigValue	jsonClass = jsonScriptClass.GetValue( TEXT( "ScriptClass" ), TEXT( "Class" ) );
				if ( !jsonClass.IsValid() || !jsonClass.IsA( CConfigValue::T_String ) )
				{
					Errorf( TEXT( "JSON value 'ScriptClass:Class' isn't exist or has wrong type (must be string)\n" ) );
					bLocalResult = false;
				}

				// Get ScriptClass:SuperClass from JSON file
				CConfigValue	jsonSuperClass = jsonScriptClass.GetValue( TEXT( "ScriptClass" ), TEXT( "SuperClass" ) );
				if ( !jsonSuperClass.IsValid() || !jsonSuperClass.IsA( CConfigValue::T_String ) )
				{
					Errorf( TEXT( "JSON value 'ScriptClass:SuperClass' isn't exist or has wrong type (must be string)\n" ) );
					bLocalResult = false;
				}

				// Get ScriptClass:Script from JSON file
				std::string		scriptCode;
				CConfigValue	jsonScript = jsonScriptClass.GetValue( TEXT( "ScriptClass" ), TEXT( "Script" ) );
				if ( jsonScript.IsValid() && !jsonScript.IsA( CConfigValue::T_String ) )
				{
					Errorf( TEXT( "JSON value 'ScriptClass:SuperClass' has wrong type (must be string)\n" ) );
					bLocalResult = false;
				}
				// Otherwise if valid we load script code
				else if ( jsonScript.IsValid() )
				{
					std::wstring	fullPathToScriptFile = scriptsDir + PATH_SEPARATOR + jsonScript.GetString();
					CArchive*		archiveScriptFile = g_FileSystem->CreateFileReader( fullPathToScriptFile.c_str() );
					if ( !archiveScriptFile )
					{
						Errorf( TEXT( "Failed to load script file '%s'\n" ), fullPathToScriptFile.c_str() );
						bLocalResult = false;
					}

					// Serialize data to string
					scriptCode.resize( archiveScriptFile->GetSize() );
					archiveScriptFile->Serialize( scriptCode.data(), archiveScriptFile->GetSize() );
					delete archiveScriptFile;
				}

				// Get ScriptClass:Properties from JSON file
				std::vector<TSharedPtr<CScriptPropertyStub>>	propertyStubs;
				CConfigValue									jsonProperties = jsonScriptClass.GetValue( TEXT( "ScriptClass" ), TEXT( "Properties" ) );
				if ( jsonProperties.IsValid() && !jsonProperties.IsA( CConfigValue::T_Array ) )
				{
					Errorf( TEXT( "JSON value 'ScriptClass:Properties' has wrong type (must be array of objects)\n" ) );
					bLocalResult = false;
				}
				// Otherwise if valid we grab all properties
				else if( jsonProperties.IsValid() )
				{
					std::vector<CConfigValue>	array = jsonProperties.GetArray();
					for ( uint32 index = 0, count = array.size(); index < count; ++index )
					{
						const CConfigValue&		value = array[index];
						if ( !value.IsValid() || !value.IsA( CConfigValue::T_Object ) )
						{
							Warnf( TEXT( "JSON value 'ScriptClass:Properties[%i]' isn't valid or has wrong type (must be object)\n" ), index );
							continue;
						}
						CConfigObject			object = value.GetObject();

						// Get property name
						CConfigValue			jsonName	= object.GetValue( TEXT( "Name" ) );
						if ( !jsonName.IsValid() || !jsonName.IsA( CConfigValue::T_String ) )
						{
							Warnf( TEXT( "JSON value 'ScriptClass:Properties[%i]:Name' isn't valid or has wrong type (must be string)\n" ), index );
							continue;
						}

						// Get property type
						CConfigValue			jsonType		= object.GetValue( TEXT( "Type" ) );
						EScriptPropertyType		propertyType	= SPT_Unknown;
						if ( !jsonType.IsValid() || !jsonType.IsA( CConfigValue::T_String ) )
						{
							Warnf( TEXT( "JSON value 'ScriptClass:Properties[%i]:Type' isn't valid or has wrong type (must be string)\n" ), index );
							continue;
						}
						else
						{
							propertyType = ConvertTextToEScriptPropertyType( jsonType.GetString() );
							if ( propertyType == SPT_Unknown )
							{
								Warnf( TEXT( "JSON value 'ScriptClass:Properties[%i]:Type' unknown property type\n" ), index );
								continue;
							}
						}

						// Get property sub type (must be for byte (enum), asset and array types)
						CConfigValue			jsonSubType = object.GetValue( TEXT( "SubType" ) );
						if ( IsScriptPropertyTypeHaveSubType( propertyType ) && ( !jsonSubType.IsValid() || !jsonSubType.IsA( CConfigValue::T_String ) || jsonSubType.GetString().empty() ) )
						{
							Warnf( TEXT( "JSON value 'ScriptClass:Properties[%i]:SubType' isn't valid or has wrong type (must be string)\n" ), index );
							continue;
						}

						// Get property category
						CConfigValue			jsonCategory = object.GetValue( TEXT( "Category" ) );
						if ( jsonCategory.IsValid() && !jsonCategory.IsA( CConfigValue::T_String ) )
						{
							Warnf( TEXT( "JSON value 'ScriptClass:Properties[%i]:Category' has wrong type (must be string)\n" ), index );
							continue;
						}

						// Get property description
						CConfigValue			jsonDescription = object.GetValue( TEXT( "Description" ) );
						if ( jsonDescription.IsValid() && !jsonDescription.IsA( CConfigValue::T_String ) )
						{
							Warnf( TEXT( "JSON value 'ScriptClass:Properties[%i]:Description' has wrong type (must be string)\n" ), index );
							continue;
						}

						// Get property array size (count of persistent variables)
						CConfigValue			jsonArraySize = object.GetValue( TEXT( "ArraySize" ) );
						if ( jsonArraySize.IsValid() && !jsonArraySize.IsA( CConfigValue::T_Int ) )
						{
							Warnf( TEXT( "JSON value 'ScriptClass:Properties[%i]:ArraySize' has wrong type (must be integer)\n" ), index );
							continue;
						}

						// Get property flags
						CConfigValue	jsonFlags = object.GetValue( TEXT( "Flags" ) );
						uint32			propertyFlags = CPF_None;
						if ( jsonFlags.IsValid() && !jsonFlags.IsA( CConfigValue::T_Object ) )
						{
							Warnf( TEXT( "JSON value 'ScriptClass:Properties[%i]:Flags' has wrong type (must be object)\n" ), index );
							continue;
						}
						else
						{
							CConfigObject	objectFlags = jsonFlags.GetObject();

							// Const flag
							CConfigValue	jsonConst	= objectFlags.GetValue( TEXT( "Const" ) );
							if ( jsonConst.IsValid() && !jsonConst.IsA( CConfigValue::T_Bool ) )
							{
								Warnf( TEXT( "JSON value 'ScriptClass:Properties[%i]:Flags:Const' has wrong type (must be bool)\n" ), index );
								continue;
							}
							else if ( jsonConst.GetBool() )
							{
								propertyFlags |= CPF_Const;
							}

							// Editor only flag
							CConfigValue	jsonEditorOnly = objectFlags.GetValue( TEXT( "EditorOnly" ) );
							if ( jsonEditorOnly.IsValid() && !jsonEditorOnly.IsA( CConfigValue::T_Bool ) )
							{
								Warnf( TEXT( "JSON value 'ScriptClass:Properties[%i]:Flags:EditorOnly' has wrong type (must be bool)\n" ), index );
								continue;
							}
							else if ( jsonEditorOnly.GetBool() )
							{
								propertyFlags |= CPF_EditorOnly;
							}

							// Edit flag
							CConfigValue	jsonEdit = objectFlags.GetValue( TEXT( "Edit" ) );
							if ( jsonEdit.IsValid() && !jsonEdit.IsA( CConfigValue::T_Bool ) )
							{
								Warnf( TEXT( "JSON value 'ScriptClass:Properties[%i]:Flags:Edit' has wrong type (must be bool)\n" ), index );
								continue;
							}
							else if ( jsonEdit.GetBool() )
							{
								propertyFlags |= CPF_Edit;
							}

							// Edit fixed size flag
							CConfigValue	jsonEditFixedSize = objectFlags.GetValue( TEXT( "EditFixedSize" ) );
							if ( jsonEditFixedSize.IsValid() && !jsonEditFixedSize.IsA( CConfigValue::T_Bool ) )
							{
								Warnf( TEXT( "JSON value 'ScriptClass:Properties[%i]:Flags:EditFixedSize' has wrong type (must be bool)\n" ), index );
								continue;
							}
							else if ( jsonEditFixedSize.GetBool() )
							{
								propertyFlags |= CPF_EditFixedSize;
							}

							// Edit const flag
							CConfigValue	jsonEditConst = objectFlags.GetValue( TEXT( "EditConst" ) );
							if ( jsonEditConst.IsValid() && !jsonEditConst.IsA( CConfigValue::T_Bool ) )
							{
								Warnf( TEXT( "JSON value 'ScriptClass:Properties[%i]:Flags:EditConst' has wrong type (must be bool)\n" ), index );
								continue;
							}
							else if ( jsonEditConst.GetBool() )
							{
								propertyFlags |= CPF_EditConst;
							}
						}

						// Create script property stub
						TSharedPtr<CScriptPropertyStub>		propertyStub = MakeSharedPtr<CScriptPropertyStub>( jsonName.GetString(), propertyType, propertyFlags, jsonSubType.GetString() );
						propertyStub->SetArraySize( jsonArraySize.GetNumber( 1.f ) );
						propertyStub->SetCategory( jsonCategory.GetString() );
						propertyStub->SetDescription( jsonDescription.GetString() );
						propertyStubs.push_back( propertyStub );
					}
				}

				// If all ok we init script class description
				if ( bLocalResult )
				{
					TSharedPtr<CScriptClassStub>	classStub = MakeSharedPtr<CScriptClassStub>( jsonClass.GetString(), jsonSuperClass.GetString() );
					classStub->SetScriptCode( scriptCode );
					classStub->SetProperties( propertyStubs );
					InOutStubs.AddClass( classStub );
				}
			}

			bResult |= bLocalResult;
		}
	}

	return bResult;
}

/*
==================
CScriptCompilerCommandlet::CompileScriptClasses
==================
*/
bool CScriptCompilerCommandlet::CompileScriptClasses( CScriptSystemStub& InStubs )
{
#if 0
	CScriptCompilerEnvironment		environment( InStubs );

	// Compile functions
	bool														bHasError = false;
	CScriptFunctionCompiler										functionCompiler( environment, g_CommandLine.HasParam( TEXT( "verbose" ) ) );
	const std::vector<TSharedPtr<CScriptClassStub>>&			classes = InStubs.GetClasses();
	for ( uint32 classIdx = 0, numClasses = classes.size(); classIdx < numClasses; ++classIdx )
	{
		const TSharedPtr<CScriptClassStub>						theClass = classes[classIdx];
		const std::vector<TSharedPtr<CScriptFunctionStub>>&		functions = theClass->GetFunctions();
		for ( uint32 funcIdx = 0, numFunctions = functions.size(); funcIdx < numFunctions; ++funcIdx )
		{
			// We must compile only script functions
			TSharedPtr<CScriptFunctionStub>			function = functions[funcIdx];
			if ( !function->IsNative() )
			{
				if ( !functionCompiler.Compile( *theClass.Get(), *function.Get() ) )
				{
					bHasError = true;
				}
			}
		}

		// If we have not any errors then alright and we set class flag 'CLASS_Compiled'
		if ( !bHasError )
		{
			theClass->GetCreatedClass()->AddClassFlag( CLASS_Compiled );
		}
	}

	return !bHasError;
#else
	return true;
#endif // 0
}