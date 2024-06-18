#include "Misc/CoreGlobals.h"
#include "Misc/StringConv.h"
#include "System/BaseFileSystem.h"
#include "Scripts/NativeClassExporter.h"

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
	const std::vector<ScriptClassStubPtr_t>&	classes = stubs.GetClasses();
	bNativeClassesChanged						= false;
	for ( uint32 index = 0, count = classes.size(); index < count; ++index )
	{
		const ScriptClassStubPtr_t&				theClass = classes[index];
		if ( theClass->HasAnyFlags( CLASS_Native ) )
		{
			ExportClass( theClass );
		}
	}
}

/*
==================
CNativeClassExporter::ExportClass
==================
*/
void CNativeClassExporter::ExportClass( const ScriptClassStubPtr_t& InClassStub )
{
	const std::wstring&		className		= InClassStub->GetName();
	const std::wstring&		superClassName	= InClassStub->GetSuperClassName();
	CClass*					theClass		= InClassStub->GetCreatedClass();
	Assert( theClass );

	// Do nothing if class has already been exported
	if ( theClass->HasAnyClassFlags( CLASS_Exported ) )
	{
		return;
	}

	// Mark class as exported
	theClass->AddClassFlag( CLASS_Exported );

	// Generate C++ body for header
	std::wstring			cppTextBuffer	= GenerateCppTextBody( InClassStub );

	// Generate within class macro
	std::wstring			withinClassMacro = GenerateWithinClass( InClassStub );

	// Generate register of native functions
	std::wstring			registerNativeFunctions = GenerateRegisterNativeFunctions( InClassStub );

	// Generate C++ class flags
	std::wstring			cppClassFlags	= GenerateClassFlags( InClassStub );

	// Generate native functions
	std::wstring			nativeFunctions = GenerateNativeFunctions( InClassStub );
	if ( !nativeFunctions.empty() && !cppTextBuffer.empty() )
	{
		cppTextBuffer += TEXT( "\n" );
	}

	// Generate full header
	std::wstring			buffer			= L_Sprintf( 
		TEXT( "//\n" ) 
		TEXT( "// This is automatically generated by the tools.\n" )
		TEXT( "// DO NOT modify this manually! Edit the corresponding .class files instead!\n" )
		TEXT( "// Broken Singularity, All Rights Reserved.\n" ) 
		TEXT( "//\n\n" ) 
		TEXT( "class %s : public %s\n" )
		TEXT( "{\n" )
		TEXT( "\tDECLARE_CLASS( %s, %s, %s, 0, TEXT( \"%s\" ) )\n" )
		TEXT( "\tNO_DEFAULT_CONSTRUCTOR( %s )\n" )
		TEXT( "%s%s\n" )
		TEXT( "public:\n" )
		TEXT( "%s%s" )
		TEXT( "};" ),
		className.c_str(), superClassName.c_str(), className.c_str(), superClassName.c_str(), cppClassFlags.c_str(), packageName.c_str(), className.c_str(), withinClassMacro.c_str(), registerNativeFunctions.c_str(), cppTextBuffer.c_str(), nativeFunctions.c_str() );

	// Read original header to compare with the new one later
	std::wstring	pathToHeader = L_Sprintf( TEXT( "%s" ) PATH_SEPARATOR TEXT( "%s.h" ), includeDir.c_str(), className.c_str() );
	std::wstring	originalHeader;
	CArchive*		fileReader = g_FileSystem->CreateFileReader( pathToHeader );
	if ( fileReader )
	{
		std::string		buffer;
		buffer.resize( fileReader->GetSize() );
		fileReader->Serialize( buffer.data(), buffer.size() );
		originalHeader = ANSI_TO_TCHAR( buffer.c_str() );
		delete fileReader;
	}

	// If header file is changed we save the new one
	if ( originalHeader != buffer )
	{
		fileReader = g_FileSystem->CreateFileWriter( pathToHeader, AW_NoFail );
		fileReader->Serialize( TCHAR_TO_ANSI( buffer.c_str() ), buffer.size() );
		delete fileReader;
		bNativeClassesChanged = true;
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
std::wstring CNativeClassExporter::GenerateNativeFunctions( const ScriptClassStubPtr_t& InClassStub )
{
	const std::vector<ScriptFunctionStubPtr_t>&		functions = InClassStub->GetFunctions();
	if ( !functions.empty() )
	{
		std::wstring	result;
		bool			bHasAnyNativeFunction = false;
		for ( uint32 index = 0, count = functions.size(); index < count; ++index )
		{
			ScriptFunctionStubPtr_t		function		= functions[index];
			if ( function->HasAnyFlags( FUNC_Native ) )
			{
				std::wstring				functionName = function->GetName();
				bHasAnyNativeFunction		= true;
				result += L_Sprintf( TEXT( "\tvirtual %s %s();\n" )
									 TEXT( "\tDECLARE_FUNCTION( %s )\n" )
									 TEXT( "\t{\n" )
									 TEXT( "\t\tthis->%s();\n" )
									 TEXT( "\t}\n" ),
									 function->GetReturnTypeName().c_str(), functionName.c_str(), functionName.c_str(), functionName.c_str() );
			}
		}

		if ( bHasAnyNativeFunction )
		{
			return TEXT( "\t// BEGIN NATIVE FUNCTIONS\n" ) + result + TEXT( "\t// END NATIVE FUNCTIONS\n" );
		}
	}
	
	return TEXT( "" );
}

/*
==================
CNativeClassExporter::GenerateRegisterNativeFunctions
==================
*/
std::wstring CNativeClassExporter::GenerateRegisterNativeFunctions( const ScriptClassStubPtr_t& InClassStub )
{
	const std::wstring&								className = InClassStub->GetName();
	const std::vector<ScriptFunctionStubPtr_t>&		functions = InClassStub->GetFunctions();
	if ( !functions.empty() )
	{
		std::wstring	result;
		bool			bHasAnyNativeFunction = false;
		for ( uint32 index = 0, count = functions.size(); index < count; ++index )
		{
			ScriptFunctionStubPtr_t		function		= functions[index];
			if ( function->HasAnyFlags( FUNC_Native ) )
			{
				std::wstring				functionName = function->GetName();
				bHasAnyNativeFunction		= true;
				result += L_Sprintf( TEXT( "\t\t\tMAP_NATIVE_FUNC( %s, %s )\n" ), className.c_str(), functionName.c_str() );
			}
		}

		if ( bHasAnyNativeFunction )
		{
			return L_Sprintf( TEXT( "\tDECLARE_REGISTER_NATIVE_FUNCS()\n" )
							  TEXT( "\t{\n" )
							  TEXT( "\t\tstatic ScriptNativeFunctionLookup s_NativeFunctions[] =\n" )
							  TEXT( "\t\t{\n" )
							  TEXT( "%s" )
							  TEXT( "\t\t\t{ nullptr, nullptr }\n" ) 
							  TEXT( "\t\t};\n" )
							  TEXT( "\t\tCScriptVM::StaticAddNativeFunctions( TEXT( \"%s\" ), s_NativeFunctions );\n" )
							  TEXT( "\t}\n" ),
							  result.c_str(), className.c_str() );
		}
	}
	
	return TEXT( "" );
}