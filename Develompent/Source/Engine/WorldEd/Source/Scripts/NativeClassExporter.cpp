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
	
	// Generate C++ body for header
	std::wstring			cppTextBuffer	= GenerateCppTextBody( InClassStub );

	// Generate within class macro
	std::wstring			withinClassMacro = GenerateWithinClass( InClassStub );

	// Generate C++ class flags
	std::wstring			cppClassFlags	= GenerateClassFlags( InClassStub );

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
		TEXT( "%s\n" )
		TEXT( "public:\n" )
		TEXT( "%s" )
		TEXT( "};" ),
		className.c_str(), superClassName.c_str(), className.c_str(), superClassName.c_str(), cppClassFlags.c_str(), packageName.c_str(), className.c_str(), withinClassMacro.c_str(), cppTextBuffer.c_str() );

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
		return TEXT( "\n" );
	}
}