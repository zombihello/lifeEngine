#include "Misc/CoreGlobals.h"
#include "Misc/StringConv.h"
#include "System/BaseFileSystem.h"
#include "Scripts/NativeClassExporter.h"

/**
 * @ingroup WorldEd
 * @brief Native group of stubs
 */
struct NativeGroupStubs
{
	std::vector<ScriptClassStubPtr_t>		classes;	/**< Native classes */
	std::vector<ScriptStructStubPtr_t>		structs;	/**< Native structs */
	std::vector<ScriptEnumStubPtr_t>		enums;		/**< Native enums */
};

/*
==================
CNativeClassExporter::CNativeClassExporter
==================
*/
CNativeClassExporter::CNativeClassExporter( const std::wstring& InIncludeDir, const std::wstring& InPackageName, const CScriptSystemStub& InStubs )
	: bNativeClassesChanged( false )
	, includeDir( InIncludeDir )
	, packageName( InPackageName )
	, stubs( InStubs )
{}

/*
==================
CNativeClassExporter::ExportClasses
==================
*/
void CNativeClassExporter::ExportClasses()
{
	// Grouping native classes by native groups
	std::unordered_map<std::wstring, NativeGroupStubs>		nativeGroups;
	{
		// Grab all native classes
		const std::vector<ScriptClassStubPtr_t>&	classes = stubs.GetClasses();
		for ( uint32 index = 0, count = classes.size(); index < count; ++index )
		{
			// We add into map only native class
			const ScriptClassStubPtr_t&		theClass = classes[index];
			if ( theClass->HasAnyFlags( CLASS_Native ) )
			{
				// If the class is native but don't have specified group then we add it into main group
				const std::wstring&		nativeGroup = theClass->GetNativeClassGroup();
				if ( nativeGroup.empty() )
				{
					nativeGroups[packageName].classes.push_back( theClass );
				}
				// Otherwise we add into specified group
				else
				{
					nativeGroups[packageName + nativeGroup].classes.push_back( theClass );
				}
			}
		}

		// Grab all native structs
		// NOTE yehor.pohuliaka - We use it only for generate registrants, because currently we not support structs in LifeScript
		nativeGroups[packageName].structs = stubs.GetStructs();

		// Grab all native enums
		// NOTE yehor.pohuliaka - We use it only for generate registrants, because currently we not support enums in LifeScript
		nativeGroups[packageName].enums = stubs.GetEnums();
	}

	// Export classes for each group
	for ( auto it = nativeGroups.begin(), itEnd = nativeGroups.end(); it != itEnd; ++it )
	{
		std::wstring								nativeClasses;
		std::wstring								nativeFuncsTables;
		std::wstring								registerNativeFuncsTables;
		std::wstring								registerNativeTypes;
		const NativeGroupStubs&						nativeGroupStubs = it->second;

		// Export classes into C++
		for ( uint32 index = 0, count = nativeGroupStubs.classes.size(); index < count; ++index )
		{			
			std::wstring					nativeClass;
			std::wstring					nativeFuncsTable;
			std::wstring					registerNativeFuncsTable;
			std::wstring					registerNativeClass;
			ExportClass( nativeGroupStubs.classes[index], registerNativeClass, nativeClass, nativeFuncsTable, registerNativeFuncsTable );

			// If generated code is valid we add it to our header
			if ( !registerNativeClass.empty() )
			{
				registerNativeTypes += L_Sprintf( TEXT( "%s%s" ), !registerNativeTypes.empty() ? TEXT( "\n" ) : TEXT( "" ), registerNativeClass.c_str() );
			}

			if ( !nativeClass.empty() )
			{
				nativeClasses += L_Sprintf( TEXT( "%s%s" ), !nativeClasses.empty() ? TEXT( "\n\n" ) : TEXT( "" ), nativeClass.c_str() );
			}

			if ( !nativeFuncsTable.empty() )
			{
				nativeFuncsTables += L_Sprintf( TEXT( "%s%s" ), !nativeFuncsTables.empty() ? TEXT( "\n" ) : TEXT( "" ), nativeFuncsTable.c_str() );
			}

			if ( !registerNativeFuncsTable.empty() )
			{
				registerNativeFuncsTables += L_Sprintf( TEXT( "%s%s" ), !registerNativeFuncsTables.empty() ? TEXT( "\n" ) : TEXT( "" ), registerNativeFuncsTable.c_str() );
			}
		}

		// Export structs into C++
		for ( uint32 index = 0, count = nativeGroupStubs.structs.size(); index < count; ++index )
		{
			std::wstring	registerNativeStruct;
			ExportStruct( nativeGroupStubs.structs[index], registerNativeStruct );

			// If generated code is valid we add it to our header
			if ( !registerNativeStruct.empty() )
			{
				registerNativeTypes += L_Sprintf( TEXT( "%s%s" ), !registerNativeTypes.empty() ? TEXT( "\n" ) : TEXT( "" ), registerNativeStruct.c_str() );
			}
		}

		// Export enums into C++
		for ( uint32 index = 0, count = nativeGroupStubs.enums.size(); index < count; ++index )
		{
			std::wstring	registerNativeEnum;
			ExportEnum( nativeGroupStubs.enums[index], registerNativeEnum );

			// If generated code is valid we add it to our header
			if ( !registerNativeEnum.empty() )
			{
				registerNativeTypes += L_Sprintf( TEXT( "%s%s" ), !registerNativeTypes.empty() ? TEXT( "\n" ) : TEXT( "" ), registerNativeEnum.c_str() );
			}
		}

		const std::wstring&	nativeGroup				= it->first;
		std::wstring		nativeGroupUpperCase	= L_Strupr( nativeGroup );

		// Generate body of exported classes
		std::wstring		bodyExportedClasses;
		if ( !nativeClasses.empty() )
		{
			bodyExportedClasses = L_Sprintf( TEXT( "// BEGIN CLASSES\n" )
											 TEXT( "#ifndef INCLUDED_%s_CLASSES\n" )
											 TEXT( "#define INCLUDED_%s_CLASSES\n" )
											 TEXT( "%s\n" )
											 TEXT( "#endif // !INCLUDED_%s_CLASSES\n" )
											 TEXT( "// END CLASSES" ),
											 nativeGroupUpperCase.c_str(), 
											 nativeGroupUpperCase.c_str(), 
											 nativeClasses.c_str(), 
											 nativeGroupUpperCase.c_str() );
		}

		// Generate body of registrants function
		std::wstring		bodyRegistrantsFunction;
		if ( !nativeFuncsTables.empty() || !registerNativeTypes.empty() || !registerNativeFuncsTables.empty() )
		{
			bodyRegistrantsFunction = L_Sprintf( TEXT( "// BEGIN REGISTRANTS\n" )
												 TEXT( "#ifdef WITH_REGISTRANTS\n" )
												 TEXT( "#ifndef INCLUDED_REGISTRANTS_%s\n" )
												 TEXT( "#define INCLUDED_REGISTRANTS_%s\n" )
												 TEXT( "static void InitRegistrants_%s()\n" )
												 TEXT( "{\n" )
												 TEXT( "%s%s" )
												 TEXT( "%s%s" )
												 TEXT( "%s\n" )
												 TEXT( "}\n" )
												 TEXT( "#endif // !INCLUDED_REGISTRANTS_%s\n" )
												 TEXT( "#endif // WITH_REGISTRANTS\n" )
												 TEXT( "// END REGISTRANTS" ),
												 nativeGroupUpperCase.c_str(), 
												 nativeGroupUpperCase.c_str(), 
												 nativeGroup.c_str(), 
												 nativeFuncsTables.c_str(), ( !nativeFuncsTables.empty() && !registerNativeTypes.empty() ) ? TEXT( "\n\n" ) : TEXT( "" ),
												 registerNativeTypes.c_str(), ( !registerNativeTypes.empty() && !registerNativeFuncsTables.empty() ) ? TEXT( "\n\n" ) : TEXT( "" ),
												 registerNativeFuncsTables.c_str(),
												 nativeGroupUpperCase.c_str() );
		}

		// Generate full header
		std::wstring		buffer	= L_Sprintf( TEXT( "//\n" )
												 TEXT( "// This is automatically generated by the tools.\n" )
												 TEXT( "// DO NOT modify this manually! Edit the corresponding .class files instead!\n" )
												 TEXT( "// Broken Singularity, All Rights Reserved.\n" )
												 TEXT( "//\n\n" )
												 TEXT( "%s" )
												 TEXT( "%s%s" ),
												 !bodyExportedClasses.empty() ? bodyExportedClasses.c_str() : TEXT( "" ),
												 ( !bodyRegistrantsFunction.empty() && !bodyExportedClasses.empty() ) ? TEXT( "\n\n" ) : TEXT( "" ),
												 !bodyRegistrantsFunction.empty() ? bodyRegistrantsFunction.c_str() : TEXT( "" ) );
	
		// Save C++ header to file
		SaveCppHeader( buffer, L_Sprintf( TEXT( "%s" ) PATH_SEPARATOR TEXT( "%sClasses.h" ), includeDir.c_str(), nativeGroup.c_str() ) );
	}
}

/*
==================
CNativeClassExporter::ExportClass
==================
*/
void CNativeClassExporter::ExportClass( const ScriptClassStubPtr_t& InClassStub, std::wstring& OutRegistrarClass, std::wstring& OutExportedClass, std::wstring& OutNativeFuncsTable, std::wstring& OutRegisterNativeFuncsTable )
{
	const std::wstring&		className		= InClassStub->GetName();
	const std::wstring&		superClassName	= InClassStub->GetSuperClassName();

	// Generate registrar for the class
	OutRegistrarClass = GenerateRegistrarClass( InClassStub );

	// Do nothing if class has already been exported or it is intrinsic
	if ( InClassStub->HasAnyFlags( CLASS_Exported | CLASS_Intrinsic ) )
	{
		return;
	}

	// If the class has CLASS_NoExport we generate only registers of native functions
	if ( InClassStub->HasAnyFlags( CLASS_NoExport ) )
	{
		GenerateNativeFunctions( InClassStub, OutNativeFuncsTable, OutRegisterNativeFuncsTable );
		return;
	}

	// Mark class as exported
	InClassStub->AddFlag( CLASS_Exported );

	// Generate C++ body for header
	std::wstring			cppTextBuffer	= GenerateCppTextBody( InClassStub );

	// Generate within class macro
	std::wstring			withinClassMacro = GenerateWithinClass( InClassStub );

	// Generate C++ class flags
	std::wstring			cppClassFlags	= GenerateClassFlags( InClassStub );

	// Generate native functions
	std::wstring			nativeFunctions = GenerateNativeFunctions( InClassStub, OutNativeFuncsTable, OutRegisterNativeFuncsTable );
	if ( !nativeFunctions.empty() && !cppTextBuffer.empty() )
	{
		cppTextBuffer += TEXT( "\n" );
	}

	// Generate full header
	OutExportedClass = L_Sprintf( TEXT( "class %s : public %s\n" )
								  TEXT( "{\n" )
								  TEXT( "\tDECLARE_CLASS( %s, %s, %s, 0, TEXT( \"%s\" ) )\n" )
								  TEXT( "\tNO_DEFAULT_CONSTRUCTOR( %s )\n" )
								  TEXT( "%s\n" )
								  TEXT( "public:\n" )
								  TEXT( "%s%s" )
								  TEXT( "};" ),
								  className.c_str(), superClassName.c_str(), 
								  className.c_str(), superClassName.c_str(), cppClassFlags.c_str(), packageName.c_str(), 
								  className.c_str(), 
								  withinClassMacro.c_str(), 
								  cppTextBuffer.c_str(), 
								  nativeFunctions.c_str() );
}

/*
==================
CNativeClassExporter::ExportStruct
==================
*/
void CNativeClassExporter::ExportStruct( const ScriptStructStubPtr_t& InStructStub, std::wstring& OutRegistrarStruct )
{
	AssertMsg( InStructStub->IsIntrinsic(), TEXT( "Currently LifeScript don't support structs" ) );

	// Generate registrar for the struct
	OutRegistrarStruct = GenerateRegistrarStruct( InStructStub );
}

/*
==================
CNativeClassExporter::ExportClass
==================
*/
void CNativeClassExporter::ExportEnum( const ScriptEnumStubPtr_t& InEnumStub, std::wstring& OutRegistrarEnum )
{
	AssertMsg( InEnumStub->IsIntrinsic(), TEXT( "Currently LifeScript don't support enums" ) );

	// Generate registrar for the enum
	OutRegistrarEnum = GenerateRegistrarEnum( InEnumStub );
}

/*
==================
CNativeClassExporter::SaveCppHeader
==================
*/
void CNativeClassExporter::SaveCppHeader( const std::wstring& InBuffer, const std::wstring& InPath )
{
	std::wstring	originalHeader;
	CArchive*		fileReader = g_FileSystem->CreateFileReader( InPath );
	if ( fileReader )
	{
		std::string		buffer;
		buffer.resize( fileReader->GetSize() );
		fileReader->Serialize( buffer.data(), buffer.size() );
		originalHeader = ANSI_TO_TCHAR( buffer.c_str() );
		delete fileReader;
	}

	// If header file is changed we save the new one
	if ( originalHeader != InBuffer )
	{
		bNativeClassesChanged	= true;
		fileReader				= g_FileSystem->CreateFileWriter( InPath, AW_NoFail );
		fileReader->Serialize( TCHAR_TO_ANSI( InBuffer.c_str() ), InBuffer.size() );
		delete fileReader;
	}
}

/*
==================
CNativeClassExporter::GenerateCppTextBody
==================
*/
std::wstring CNativeClassExporter::GenerateCppTextBody( const ScriptClassStubPtr_t& InClassStub )
{
	Assert( InClassStub );
	const std::wstring&		cppTextBuffer = InClassStub->GetCppText();
	
	if ( !cppTextBuffer.empty() )
	{
		return L_Sprintf(
			TEXT( "\t// BEGIN CPP TEXT\n" )
			TEXT( "%s\n" )
			TEXT( "\t// END CPP TEXT\n" ),
			L_ReplaceSubString( cppTextBuffer, TEXT( "\r" ), TEXT( "" ) ).c_str() );
	}
	else
	{
		return TEXT( "" );
	}
}

/*
==================
CNativeClassExporter::GenerateClassFlags
==================
*/
std::wstring CNativeClassExporter::GenerateClassFlags( const ScriptClassStubPtr_t& InClassStub )
{
	std::wstring	resultClassFlags = TEXT( "0" );
	Assert( InClassStub );
	
	if ( InClassStub->HasAnyFlags( CLASS_Transient ) )
	{
		resultClassFlags += TEXT( "|CLASS_Transient" );
	}
	if ( InClassStub->HasAnyFlags( CLASS_Deprecated ) )
	{
		resultClassFlags += TEXT( "|CLASS_Deprecated" );
	}
	if ( InClassStub->HasAnyFlags( CLASS_Abstract ) )
	{
		resultClassFlags += TEXT( "|CLASS_Abstract" );
	}

	return resultClassFlags;
}

/*
==================
CNativeClassExporter::GenerateWithinClass
==================
*/
std::wstring CNativeClassExporter::GenerateWithinClass( const ScriptClassStubPtr_t& InClassStub )
{
	Assert( InClassStub );
	std::wstring	withinClassName = InClassStub->GetWithinClassName();
	if ( !withinClassName.empty() )
	{
		return L_Sprintf( TEXT( "\tDECLARE_WITHIN_CLASS( %s )\n" ), withinClassName.c_str() );
	}
	else
	{
		return TEXT( "" );
	}
}

/*
==================
CNativeClassExporter::GenerateNativeFunctions
==================
*/
std::wstring CNativeClassExporter::GenerateNativeFunctions( const ScriptClassStubPtr_t& InClassStub, std::wstring& OutNativeFuncsTable, std::wstring& OutRegisterNativeFuncsTable )
{
	const std::wstring&								className = InClassStub->GetName();
	const std::vector<ScriptFunctionStubPtr_t>&		functions = InClassStub->GetFunctions();
	bool											bNoExport = InClassStub->HasAnyFlags( CLASS_NoExport );
	if ( !functions.empty() )
	{
		std::wstring	nativeFunctions;
		std::wstring	nativeFuncsTableBody;
		bool			bHasAnyNativeFunction = false;
		for ( uint32 index = 0, count = functions.size(); index < count; ++index )
		{
			ScriptFunctionStubPtr_t		function		= functions[index];
			if ( function->HasAnyFlags( FUNC_Native ) )
			{
				std::wstring				functionName = function->GetName();
				bHasAnyNativeFunction		= true;
				nativeFuncsTableBody		+= L_Sprintf( TEXT( "\t\tMAP_NATIVE_FUNC( %s, %s )\n" ), className.c_str(), functionName.c_str() );

				// We export native functions only if the class hasn't CLASS_NoExport
				if ( !bNoExport )
				{
					nativeFunctions += L_Sprintf( TEXT( "\tvirtual %s %s();\n" )
												  TEXT( "\tDECLARE_FUNCTION( %s )\n" )
												  TEXT( "\t{\n" )
												  TEXT( "\t\tthis->%s();\n" )
												  TEXT( "\t}\n" ),
												  function->GetReturnTypeName().c_str(), functionName.c_str(), functionName.c_str(), functionName.c_str() );
				}
			}
		}

		if ( bHasAnyNativeFunction )
		{
			// Return native functions table and register the one in the system
			OutNativeFuncsTable			= L_Sprintf( TEXT( "\tstatic ScriptNativeFunctionLookup s_%sNativeFunctions[] =\n" )
													 TEXT( "\t{\n" ) 
													 TEXT( "%s" )
													 TEXT( "\t\t{ nullptr, nullptr }\n" )
													 TEXT( "\t};" ),
													 className.c_str(), nativeFuncsTableBody.c_str() );
			OutRegisterNativeFuncsTable = L_Sprintf( TEXT( "\tCObject::StaticAddNativeFunctions( TEXT( \"%s\" ), s_%sNativeFunctions );" ),
													 className.c_str(), className.c_str() );

			// Return exported native functions (only if the class hasn't CLASS_NoExport)
			if ( !bNoExport )
			{
				return TEXT( "\t// BEGIN NATIVE FUNCTIONS\n" ) + nativeFunctions + TEXT( "\t// END NATIVE FUNCTIONS\n" );
			}

			return TEXT( "" );
		}
	}
	
	return TEXT( "" );
}