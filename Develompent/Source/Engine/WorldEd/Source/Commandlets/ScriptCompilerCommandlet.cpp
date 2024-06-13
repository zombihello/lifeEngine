#include "Misc/CoreGlobals.h"
#include "Misc/FileTools.h"
#include "Misc/StringConv.h"
#include "System/Config.h"
#include "System/BaseFileSystem.h"
#include "Reflection/Class.h"
#include "Reflection/ObjectPackage.h"
#include "Scripts/ScriptFileParser.h"
#include "Scripts/ScriptEnvironmentBuilder.h"
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