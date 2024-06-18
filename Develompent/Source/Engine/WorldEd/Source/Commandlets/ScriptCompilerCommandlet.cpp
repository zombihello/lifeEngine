#include "Misc/CoreGlobals.h"
#include "Misc/FileTools.h"
#include "Misc/StringConv.h"
#include "Misc/CommandLine.h"
#include "System/Config.h"
#include "System/BaseFileSystem.h"
#include "Reflection/Class.h"
#include "Reflection/ObjectPackage.h"
#include "Scripts/ScriptFileParser.h"
#include "Scripts/ScriptEnvironmentBuilder.h"
#include "Scripts/NativeClassExporter.h"
#include "Scripts/ScriptFunctionCompiler.h"
#include "Commandlets/ScriptCompilerCommandlet.h"

IMPLEMENT_CLASS( CScriptCompilerCommandlet )
IMPLEMENT_DEFAULT_INITIALIZE_CLASS( CScriptCompilerCommandlet )

/**
 * @ingroup WorldEd
 * @brief Module info
 */
struct ScriptPackageInfo
{
	std::wstring		srcPath;	/**< Relative path to source scripts (e.g Engine/Core, Games/EleotGame) */
	CObjectPackage*		package;	/**< Package with script classes */
};

/*
==================
CScriptCompilerCommandlet::Main
==================
*/
bool CScriptCompilerCommandlet::Main( const CCommandLine& InCommandLine )
{
	// Modules to compile
	std::vector<ScriptPackageInfo>		scriptPackageInfos;
	{
		const CJsonValue*				jsonSrcDirs = CConfig::Get().GetValue( CT_Editor, TEXT( "Editor.ScriptCompiler" ), TEXT( "SrcDirs" ) );
		const std::vector<CJsonValue>*	srcDirsJsonArray = jsonSrcDirs ? jsonSrcDirs->GetArray() : nullptr;
		if ( srcDirsJsonArray )
		{
			for ( uint32 indexSrcDir = 0, countSrcDirs = srcDirsJsonArray->size(); indexSrcDir < countSrcDirs; ++indexSrcDir )
			{
				// Get module name
				const CJsonValue&		jsonScriptSrcDir = srcDirsJsonArray->at( indexSrcDir );
				if ( !jsonScriptSrcDir.IsA( JVT_String ) )
				{
					Warnf( TEXT( "Invalid script source directory at %i, must be string type\n" ), indexSrcDir );
					continue;
				}

				// If we have place holder %Game% replace it by g_GameName
				ScriptPackageInfo&			scriptPackageInfo = scriptPackageInfos.emplace_back();
				scriptPackageInfo.srcPath	= jsonScriptSrcDir.GetString();
				if ( scriptPackageInfo.srcPath.find( TEXT( "%Game%" ) ) != std::wstring::npos )
				{
					scriptPackageInfo.srcPath	= L_ReplaceSubString( scriptPackageInfo.srcPath, TEXT( "%Game%" ), g_GameName );
				}

				// Create script package
				scriptPackageInfo.package		= CObjectPackage::CreatePackage( nullptr, CFilename( scriptPackageInfo.srcPath ).GetBaseFileName().c_str() );
			}
		}

		if ( scriptPackageInfos.empty() )
		{
			Errorf( TEXT( "Have not modules to compile, check in Editor.ScriptCompiler:SrcDirs (Editor.ini)\n" ) );
			return false;
		}

		if ( scriptPackageInfos.size() != srcDirsJsonArray->size() )
		{
			Warnf( TEXT( "In section Editor.ScriptCompiler:SrcDirs (Editor.ini) exists invalid elements, check it\n" ) );
		}
	}

	// Parse and compile scripts for each package
	for ( uint32 index = 0, count = scriptPackageInfos.size(); index < count; ++index )
	{	
		CScriptSystemStub			stubs;
		const ScriptPackageInfo&	scriptPackageInfo	= scriptPackageInfos[index];
		const std::wstring			packageName			= scriptPackageInfo.package->GetName();
		const std::wstring			scriptDir			= L_Sprintf(
			TEXT( "%s" ) PATH_SEPARATOR
			TEXT( "Develompent" ) PATH_SEPARATOR 
			TEXT( "Source" ) PATH_SEPARATOR
			TEXT( "%s" ) PATH_SEPARATOR 
			TEXT( "Classes" ) PATH_SEPARATOR,
			Sys_BaseDir().c_str(), scriptPackageInfo.srcPath.c_str() );

		const std::wstring		outputPackage			= L_Sprintf( 
			TEXT( "%s" ) PATH_SEPARATOR 
			TEXT( "Scripts" ) PATH_SEPARATOR
			TEXT( "%s.classes" ),
			Sys_GameDir().c_str(), packageName.c_str() );

		// Parse scripts for the package
		Logf( TEXT( "----------------- '%s' -----------------\n" ), packageName.c_str() );
		bool	bResult = ParseScripts( scriptDir, stubs, scriptDir );
		if ( !bResult )
		{
			return false;
		}

		// Build CClass, CStruct and others
		CScriptEnvironmentBuilder	scriptEnvironmentBuilder( stubs, scriptPackageInfo.package );
		bResult = scriptEnvironmentBuilder.Build();
		if ( !bResult )
		{
			return false;
		}

		// Generate header files for native classes
		const std::wstring			nativeClassIncludeDir = L_Sprintf(
			TEXT( "%s" ) PATH_SEPARATOR
			TEXT( "Develompent" ) PATH_SEPARATOR
			TEXT( "Source" ) PATH_SEPARATOR
			TEXT( "%s" ) PATH_SEPARATOR
			TEXT( "Include" ) PATH_SEPARATOR
			TEXT( "Classes" ) PATH_SEPARATOR,
			Sys_BaseDir().c_str(), scriptPackageInfo.srcPath.c_str() );

		CNativeClassExporter		nativeClassExporter( nativeClassIncludeDir, packageName, stubs );
		nativeClassExporter.ExportClasses();
		if ( nativeClassExporter.IsNativeClassesChanged() )
		{
			Errorf( TEXT( "C++ headers for native classes has been generated. Recompile the build and restart script compiler to continue\n" ) );
			return false;
		}

		// Compile scripts
		bResult = CompileScripts( stubs );
		if ( !bResult )
		{
			return false;
		}

		// Save compiled script package
		bResult = CObjectPackage::SavePackage( scriptPackageInfo.package, nullptr, OBJECT_None, outputPackage.c_str(), SAVE_None );
		if ( !bResult )
		{
			return false;
		}
	}

	return true;
}

/*
==================
CScriptCompilerCommandlet::ParseScripts
==================
*/
bool CScriptCompilerCommandlet::ParseScripts( const std::wstring& InScriptDir, CScriptSystemStub& InOutStubs, const std::wstring& InRootDir )
{
	// Grab all files from directory
	bool						bHasErrors	= false;
	std::vector<std::wstring>	files		= g_FileSystem->FindFiles( InScriptDir, true, true );
	for ( uint32 index = 0, count = files.size(); index < count; ++index )
	{
		// If the file is a directory look in
		CFilename		filename = InScriptDir + files[index];
		if ( filename.GetExtension().empty() )
		{
			ParseScripts( filename.GetFullPath() + PATH_SEPARATOR, InOutStubs, InRootDir );
			continue;
		}

		// If the file has 'class' extension its our LifeScript and parse it
		if ( filename.GetExtension() == TEXT( "class" ) )
		{
			// Read all file into buffer
			CArchive*	fileReader = g_FileSystem->CreateFileReader( filename.GetFullPath() );
			if ( !fileReader )
			{
				bHasErrors = true;
				Errorf( TEXT( "File '%s' not found\n" ), filename.GetFileName().c_str() );
				continue;
			}

			std::string		buffer;
			buffer.resize( fileReader->GetSize() );
			fileReader->Serialize( buffer.data(), buffer.size() );
			delete fileReader;

			// Parse script file
			CScriptFileParser		parser( InOutStubs );
			if ( !parser.ParseFile( filename.GetFullPath(), buffer ) )
			{
				bHasErrors			= true;
				std::wstring		relativeFilePath;
				L_MakeRelativePath( filename.GetFullPath(), InRootDir, relativeFilePath, false );
				Errorf( TEXT( "Failed to parse '%s'\n" ), relativeFilePath.c_str() );
				continue;
			}
		}
	}

	return !bHasErrors;
}

/*
==================
CScriptCompilerCommandlet::CompileScripts
==================
*/
bool CScriptCompilerCommandlet::CompileScripts( CScriptSystemStub& InStubs )
{
	// Compile functions in classes
	bool										bHasError = false;
	CScriptFunctionCompiler						functionCompiler( InStubs, g_CommandLine.HasParam( TEXT( "showdump" ) ) );
	const std::vector<ScriptClassStubPtr_t>		classes = InStubs.GetClasses();
	for ( uint32 classIdx = 0, numClasses = classes.size(); classIdx < numClasses; ++classIdx )
	{
		const ScriptClassStubPtr_t&						classStub = classes[classIdx];
		CClass*											theClass = classStub->GetCreatedClass();
		const std::vector<ScriptFunctionStubPtr_t>		functions = classStub->GetFunctions();
		Assert( theClass );

		// Skip the class if has been already compiled
		if ( theClass->HasAnyClassFlags( CLASS_Compiled ) )
		{
			continue;
		}

		for ( uint32 funcIdx = 0, numFunctions = functions.size(); funcIdx < numFunctions; ++funcIdx )
		{
			// We compile only script functions
			const ScriptFunctionStubPtr_t&		function = functions[funcIdx];
			if ( !function->HasAnyFlags( FUNC_Native ) )
			{
				// If function has not body it is error
				if ( !function->HasBody() )
				{
					Errorf( TEXT( "%s: Function '%s' has not body\n" ), function->GetContext().ToString().c_str(), function->GetName().c_str() );
					bHasError = true;
					continue;
				}

				if ( !functionCompiler.Compile( *classStub.Get(), *function.Get() ) )
				{
					bHasError = true;
				}
			}
		}

		// If we have not any errors then alright and we mark class by CLASS_Compiled flag
		if ( !bHasError )
		{
			theClass->AddClassFlag( CLASS_Compiled );
		}
	}

	return !bHasError;
}