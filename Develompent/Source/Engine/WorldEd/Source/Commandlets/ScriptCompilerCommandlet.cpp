#include "Reflection/Class.h"
#include "Misc/CoreGlobals.h"
#include "Logger/LoggerMacros.h"
#include "System/BaseFileSystem.h"
#include "Commandlets/ScriptCompilerCommandlet.h"
#include "Scripts/NativeClassExporter.h"
#include "Scripts/ScriptFileParser.h"
#include "Scripts/ScriptFunctionCompiler.h"
#include "Scripts/ScriptCompilerEnvironment.h"
#include "Scripts/ScriptEnvironmentBuilder.h"
#include "Reflection/ReflectionEnvironment.h"
#include "Reflection/ObjectPackage.h"

IMPLEMENT_CLASS( CScriptCompilerCommandlet )
IMPLEMENT_DEFAULT_INITIALIZE_CLASS( CScriptCompilerCommandlet )

/*
==================
CScriptCompilerCommandlet::Main
==================
*/
bool CScriptCompilerCommandlet::Main( const CCommandLine& InCommandLine )
{
	// Path to directory with scripts
	CScriptSystemStub		stubs;
	const std::wstring		scriptDir = CString::Format( TEXT( "%s" ) PATH_SEPARATOR TEXT( "Develompent" ) PATH_SEPARATOR TEXT( "Source" ) PATH_SEPARATOR TEXT( "Engine" ) PATH_SEPARATOR TEXT( "Engine" ) PATH_SEPARATOR TEXT( "Classes" ) PATH_SEPARATOR, Sys_BaseDir().c_str() );
	const std::wstring		outputDir = CString::Format( TEXT( "%s" ) PATH_SEPARATOR TEXT( "Scripts" ) PATH_SEPARATOR, Sys_GameDir().c_str() );

	// Parse scripts
	bool	bResult = ParseScripts( scriptDir, stubs );
	if ( !bResult )
	{
		Errorf( TEXT( "Failed to parse scripts from directory '%s'\n" ), scriptDir.c_str() );
		return false;
	}

	// Build environment for native headers generator and function compiler
	CScriptEnvironmentBuilder		builder( stubs );
	if ( !builder.Build() )
	{
		Errorf( TEXT( "Failed to build environment for function compiler\n" ) );
		return false;
	}

	// Generate native headers
	bResult = GenerateNativeHeaders( stubs );
	if ( bResult )
	{
		Errorf( TEXT( "C++ headers for native classes has been generated. Recompile the build and restart script compiler to continue\n" ) );
		return false;
	}

	// Compile scripts
	bResult = CompileScripts( stubs );
	if ( !bResult )
	{
		Errorf( TEXT( "Failed to compile scripts from directory '%s'\n" ), scriptDir.c_str() );
		return false;
	}

	// Save compiled scripts
	bResult = SaveScripts( outputDir + PATH_SEPARATOR + TEXT( "Engine.classes" ), stubs );
	if ( !bResult )
	{
		Errorf( TEXT( "Failed to save scripts to directory '%s'\n" ), outputDir.c_str() );
		return false;
	}

	return true;
}

/*
==================
CScriptCompilerCommandlet::ParseScripts
==================
*/
bool CScriptCompilerCommandlet::ParseScripts( const std::wstring& InScriptDir, CScriptSystemStub& InOutStubs )
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
			ParseScripts( InScriptDir + filename.GetFullPath(), InOutStubs );
			continue;
		}

		// If the file has 'class' extension its our LifeScript and parse it
		if ( filename.GetExtension() == TEXT( "class" ) )
		{
			// Read all file into buffer
			CArchive*		fileArchive = g_FileSystem->CreateFileReader( filename.GetFullPath() );
			if ( !fileArchive )
			{
				Errorf( TEXT( "File '%s' not found\n" ), filename.GetFullPath().c_str() );
				continue;
			}

			std::string		buffer;
			buffer.resize( fileArchive->GetSize() );
			fileArchive->Serialize( buffer.data(), buffer.size() );
			delete fileArchive;

			// Parse script file
			CScriptFileParser		parser( InOutStubs );
			bResult |= parser.ParseFile( filename.GetFullPath(), buffer, false );
			if ( !bResult )
			{
				Errorf( TEXT( "Failed to parse '%s'\n" ), filename.GetFullPath().c_str() );
				continue;
			}
		}
	}

	return bResult;
}

/*
==================
CScriptCompilerCommandlet::GenerateNativeHeaders
==================
*/
bool CScriptCompilerCommandlet::GenerateNativeHeaders( CScriptSystemStub& InStubs )
{
	const std::wstring		includeDir	= CString::Format( TEXT( "%s" ) PATH_SEPARATOR TEXT( "Develompent" ) PATH_SEPARATOR TEXT( "Source" ) PATH_SEPARATOR TEXT( "Engine" ) PATH_SEPARATOR TEXT( "Engine" ) PATH_SEPARATOR TEXT( "Include" ) PATH_SEPARATOR TEXT( "Classes" ), Sys_BaseDir().c_str() );
	const std::wstring		sourceDir	= CString::Format( TEXT( "%s" ) PATH_SEPARATOR TEXT( "Develompent" ) PATH_SEPARATOR TEXT( "Source" ) PATH_SEPARATOR TEXT( "Engine" ) PATH_SEPARATOR TEXT( "Engine" ) PATH_SEPARATOR TEXT( "Source" ) PATH_SEPARATOR TEXT( "Classes" ), Sys_BaseDir().c_str() );
	
	CNativeClassExporter	nativeClassExporter( includeDir, sourceDir, InStubs );
	nativeClassExporter.ExportClasses();
	return nativeClassExporter.IsNativeClassesChanged();
}

/*
==================
CScriptCompilerCommandlet::CompileScripts
==================
*/
bool CScriptCompilerCommandlet::CompileScripts( CScriptSystemStub& InStubs )
{
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
}

/*
==================
CScriptCompilerCommandlet::SaveScripts
==================
*/
bool CScriptCompilerCommandlet::SaveScripts( const std::wstring& InFilename, CScriptSystemStub& InStubs )
{
	// Save objects into a package
	CObjectPackage										objectPackage;
	const std::vector<TSharedPtr<CScriptClassStub>>&	classes = InStubs.GetClasses();
	for ( uint32 classIdx = 0, numClasses = classes.size(); classIdx < numClasses; ++classIdx )
	{
		objectPackage.AddObject( classes[classIdx]->GetCreatedClass() );
	}

	return objectPackage.Save( InFilename );
}