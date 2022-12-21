#include "Misc/CoreGlobals.h"
#include "Misc/WorldEdGlobals.h"
#include "Misc/UIGlobals.h"
#include "Misc/Misc.h"
#include "Containers/StringConv.h"
#include "Logger/LoggerMacros.h"
#include "System/BaseEngine.h"
#include "System/BaseFileSystem.h"
#include "System/AssetsImport.h"
#include "System/EditorEngine.h"
#include "System/ThreadingBase.h"
#include "System/DragNDrop.h"
#include "Windows/ContentBrowserWindow.h"
#include "Windows/DialogWindow.h"
#include "Windows/InputTextDialog.h"
#include "Windows/FileDialog.h"
#include "ImGUI/ImGUIEngine.h"
#include "ImGUI/imgui_internal.h"
#include "ImGUI/imgui_stdlib.h"

// Assets
#include "Render/Material.h"
#include "System/PhysicsMaterial.h"

// Asset editor windows
#include "Windows/TextureEditorWindow.h"
#include "Windows/MaterialEditorWindow.h"
#include "Windows/StaticMeshEditorWindow.h"

// Dialogs
#include "Windows/ImportSettingsDialogs.h"

/** Border size for buttons in asset viewer */
#define CONTENTBROWSER_ASSET_BORDERSIZE		1.f

/** Selection colode */
#define CONTENTBROWSER_SELECTCOLOR			ImVec4( 0.f, 0.43f, 0.87f, 1.f )

/** Internal drag & drop type of a file node */
#define CONTENTBROWSER_DND_FILENODETYPE		"DND::FileNode"

/**
 * @ingroup WorldEd
 * @brief Struct package info
 */
struct SPackageInfo
{
	CFilename	oldPath;		/**< Old path to package */
	CFilename	newPath;		/**< New path to package */
};

/** Table of color buttons by asset type */
static ImVec4		GAssetBorderColors[] =
{
	ImVec4( 1.f, 1.f, 1.f, 1.f ),				// AT_Unknown
	ImVec4( 0.75f, 0.25f, 0.25f, 1.f ),			// AT_Texture2D
	ImVec4( 0.25f, 0.75f, 0.25f, 1.f ),			// AT_Material
	ImVec4( 0.f, 0.f, 0.f, 0.f ),				// AT_Script
	ImVec4( 0.f, 1.f, 1.f, 1.f ),				// AT_StaticMesh
	ImVec4( 0.38f, 0.33f, 0.83f, 1.f ),			// AT_AudioBank
	ImVec4( 0.78f, 0.75f, 0.5f, 1.f )			// AT_PhysicsMaterial
};
static_assert( ARRAY_COUNT( GAssetBorderColors ) == AT_Count, "Need full init GAssetBorderColors array" );

/** Table of path to asset icons by type */
static const tchar* GAssetIconPaths[] =
{
	TEXT( "Unknown.png" ),				// AT_Unknown
	TEXT( "Texture.png" ),				// AT_Texture2D
	TEXT( "Material.png" ),				// AT_Material
	TEXT( "Script.png" ),				// AT_Script
	TEXT( "StaticMesh.png" ),			// AT_StaticMesh
	TEXT( "Audio.png" ),				// AT_AudioBank
	TEXT( "PhysMaterial.png" )			// AT_PhysicsMaterial
};
static_assert( ARRAY_COUNT( GAssetIconPaths ) == AT_Count, "Need full init GAssetIconPaths array" );

/**
 * @ingroup WorldEd
 * @brief Convert array of filenames to one string
 *
 * @param InArray	Array of filenames
 * @param InMaxSize	Max size of array to print in result string
 * @return Return formated one string
 */
static FORCEINLINE std::wstring ArrayFilenamesToString( const std::vector<CFilename>& InArray, uint32 InMaxSize = 3 )
{
	std::wstring	result;
	for ( uint32 index = 0, count = InArray.size(); index < count; ++index )
	{
		result += InArray[index].GetBaseFilename();
		if ( count > InMaxSize && index + 1 == InMaxSize )
		{
			result += TEXT( "\n...\n" );
			break;
		}
		else
		{
			result += TEXT( "\n" );
		}
	}

	return result;
}

/**
 * @ingroup WorldEd
 * @brief Get packages in directory (recursive)
 *
 * @param InRootDir		Root directory
 * @param InNewRootDir	New root directory
 * @param OutResult		Output array of path to packages in directory (old and new)
 */
static void GetPackagesInDirectory( const std::wstring& InRootDir, const std::wstring& InNewRootDir, std::vector<SPackageInfo>& OutResult )
{
	// Find all files in directory
	std::vector<std::wstring>	files = GFileSystem->FindFiles( InRootDir, true, true );
	for ( uint32 index = 0, count = files.size(); index < count; ++index )
	{
		CFilename		oldPath = InRootDir + PATH_SEPARATOR + files[index];
		CFilename		newPath = InNewRootDir + PATH_SEPARATOR + files[index];

		// If this directory, we look packages inside
		if ( GFileSystem->IsDirectory( oldPath.GetFullPath() ) )
		{
			GetPackagesInDirectory( oldPath.GetFullPath(), newPath.GetFullPath(), OutResult );
		}
		// Else if this is package, we add to OutResult
		else if ( oldPath.GetExtension() == TEXT( "pak" ) )
		{
			OutResult.push_back( SPackageInfo{ oldPath, newPath } );
		}
	}
}

/**
 * @ingroup WorldEd
 * @brief Get files in directory (recursive)
 * 
 * @param InRootDir		Root directory
 * @param OutFiles		Output files in directory
 * @param OutDirs		Output sub directories in directory
 */
static void GetFilesInDirectory( const std::wstring& InRootDir, std::vector<std::wstring>& OutFiles, std::vector<std::wstring>& OutDirs )
{
	// Find all files in directory
	std::vector<std::wstring>	files = GFileSystem->FindFiles( InRootDir, true, true );
	for ( uint32 index = 0, count = files.size(); index < count; ++index )
	{
		std::wstring path = InRootDir + PATH_SEPARATOR + files[index];

		// If this directory, we look files inside
		if ( GFileSystem->IsDirectory( path ) )
		{
			GetFilesInDirectory( path, OutFiles, OutDirs );
			OutDirs.push_back( path );
		}
		else
		{
			OutFiles.push_back( path );
		}
	}
}

/**
 * @ingroup WorldEd
 * @brief Is empty directory (recursive)
 *
 * @param InRootDir		Root directory
 * @param InIgnoreDir	Is need ignore folders
 * @return Return TRUE if directory is empty, otherwise will return FALSE
 */
static bool IsEmptyDirectory( const std::wstring& InRootDir, bool InIgnoreDir = false )
{
	std::vector<std::wstring>	files = GFileSystem->FindFiles( InRootDir, true, true );
	if ( files.empty() )
	{
		return true;
	}
	else if ( !InIgnoreDir )
	{
		return false;
	}

	bool	bExistFiles = false;
	for ( uint32 index = 0, count = files.size(); !bExistFiles && index < count; ++index )
	{
		const std::wstring&		file = files[index];
		if ( GFileSystem->IsDirectory( InRootDir + PATH_SEPARATOR + file ) )
		{
			bExistFiles |= !IsEmptyDirectory( InRootDir + PATH_SEPARATOR + file, InIgnoreDir );
		}
		else
		{
			bExistFiles = true;
		}
	}

	return !bExistFiles;
}

//
// IMPORT ASSETS RUNNABLE
//

/**
  * @ingroup WorldEd
  * @brief Runnable object for import assets in other thread
 */
class CImportAssetsRunnable : public CRunnable
{
public:
	/**
	 * @brief Constructor
	 * 
	 * @param InOwner			Owner
	 * @param InFilesToImport	Array of files to import
	 */
	CImportAssetsRunnable( CContentBrowserWindow* InOwner, const std::vector<std::wstring>& InFilesToImport )
		: importMode( IM_Default )
		, owner( InOwner )
		, eventResponse( nullptr )
		, filesToImport( InFilesToImport )
	{}

	/**
	 * @brief Initialize
	 *
	 * Allows per runnable object initialization. NOTE: This is called in the
	 * context of the thread object that aggregates this, not the thread that
	 * passes this runnable to a new thread.
	 *
	 * @return True if initialization was successful, false otherwise
	 */
	virtual bool Init() override
	{
		check( owner && owner->package );
		eventResponse = GSynchronizeFactory->CreateSynchEvent();
		return true;
	}

	/**
	 * @brief Run
	 *
	 * This is where all per object thread work is done. This is only called
	 * if the initialization was successful.
	 *
	 * @return The exit code of the runnable object
	 */
	virtual uint32 Run() override
	{
		// Import all assets
		std::wstring									errorMessages;
		CAssetFactory::EResultShowImportSettings		importSettingsModes[AT_Count];
		appMemzero( importSettingsModes, sizeof( CAssetFactory::EResultShowImportSettings ) * AT_Count );

		for ( uint32 index = 0, count = filesToImport.size(); index < count; ++index )
		{
			CFilename		filename( filesToImport[index] );
			bool			bExist	= owner->package->IsExist( filename.GetBaseFilename() );
			if ( bExist )
			{
				if ( importMode == IM_SkipConflicts )
				{
					continue;
				}
				else if ( importMode != IM_Force )
				{
					CDialogWindow::EButtonType	pressedButton;
					TSharedPtr<CDialogWindow>	popup = owner->OpenPopup<CDialogWindow>( TEXT( "Question" ), CString::Format( TEXT( "Asset '%s' already exist in package.\nDo you want change him?" ), filename.GetBaseFilename().c_str() ), CDialogWindow::BT_Yes | CDialogWindow::BT_YesToAll | CDialogWindow::BT_No | CDialogWindow::BT_NoToAll );
					popup->OnButtonPressed().Add( [&]( CDialogWindow::EButtonType InButtonType )
												  {
													  pressedButton = InButtonType;
													  eventResponse->Trigger();
												  } );
					eventResponse->Wait();
					switch ( pressedButton )
					{
					case CDialogWindow::BT_Yes:
						break;

					case CDialogWindow::BT_No:
						continue;

					case CDialogWindow::BT_YesToAll:
						importMode = IM_Force;
						break;

					case CDialogWindow::BT_NoToAll:
						importMode = IM_SkipConflicts;
						continue;
					}
				}
			}

			EAssetType							assetType = GAssetFactory.GetAssetTypeByPath( filename.GetFullPath() );
			std::wstring						errorMsg;
			std::vector<TSharedPtr<CAsset>>		result;

			// Show dialog with import settings if it need
			if ( importSettingsModes[assetType] != CAssetFactory::RSIS_ImportAll && GAssetFactory.ShowImportSettings( assetType, owner, eventResponse, importSettingsModes[assetType] ) && importSettingsModes[assetType] == CAssetFactory::RSIS_Cancel )
			{
				continue;
			}
			
			// Import asset
			if ( !GAssetFactory.Import( filename.GetFullPath(), result, errorMsg ) )
			{
				errorMessages += CString::Format( TEXT( "\n%s: %s" ), filename.GetBaseFilename().c_str(), errorMsg.c_str() );
			}
			else
			{
				// Add to package all imported assets
				for ( uint32 assetIdx = 0, countAssets = result.size(); assetIdx < countAssets; ++assetIdx )
				{
					owner->package->Add( result[assetIdx]->GetAssetHandle() );
				}
			}
		}

		// If exist errors, show popup window
		if ( !errorMessages.empty() )
		{
			owner->OpenPopup<CDialogWindow>( TEXT( "Error" ), CString::Format( TEXT( "The following assets not imported with following errors\n\nAssets:%s" ), errorMessages.c_str() ), CDialogWindow::BT_Ok );
		}
		return 0;
	}

	/**
	 * @brief Stop
	 *
	 * This is called if a thread is requested to terminate early
	 */
	virtual void Stop() override
	{
		GSynchronizeFactory->Destroy( eventResponse );
	}

	/**
	 * @brief Exit
	 *
	 * Called in the context of the aggregating thread to perform any cleanup.
	 */
	virtual void Exit() override
	{}

private:
	/**
	 * @brief Enumeration of import modes
	 */
	enum EImportMode
	{
		IM_Default,			/**< Default import mode. When importing asset is exist in package, will show dialog window with question is need change it asset or no */
		IM_Force,			/**< Import all assets and ignore conflicts */
		IM_SkipConflicts	/**< Skip conflicts while import assets */
	};

	EImportMode					importMode;		/**< Import mode */
	CContentBrowserWindow*		owner;			/**< Owner */
	CEvent*						eventResponse;	/**< Event used when opened popup of change exist assets */
	std::vector<std::wstring>	filesToImport;	/**< Array of files to import */
};

//
// RENAME ASSET RUNNABLE
//

/**
  * @ingroup WorldEd
  * @brief Runnable object for rename asset in other thread
  */
class CRenameAssetRunnable : public CRunnable
{
public:
	/**
	 * @brief Constructor
	 *
	 * @param InOwner			Owner
	 * @param InAssetNode		Asset node to rename
	 */
	CRenameAssetRunnable( CContentBrowserWindow* InOwner, CContentBrowserWindow::CAssetNode InAssetNode )
		: owner( InOwner )
		, eventResponse( nullptr )
		, assetNode( InAssetNode )
	{}

	/**
	 * @brief Initialize
	 *
	 * Allows per runnable object initialization. NOTE: This is called in the
	 * context of the thread object that aggregates this, not the thread that
	 * passes this runnable to a new thread.
	 *
	 * @return True if initialization was successful, false otherwise
	 */
	virtual bool Init() override
	{
		check( owner && owner->package );
		eventResponse = GSynchronizeFactory->CreateSynchEvent();
		return true;
	}

	/**
	 * @brief Run
	 *
	 * This is where all per object thread work is done. This is only called
	 * if the initialization was successful.
	 *
	 * @return The exit code of the runnable object
	 */
	virtual uint32 Run() override
	{
		// Get new asset name and check on exist other asset with this name
		bool			bIsOk = false;
		std::wstring	newAssetName;
		while ( !bIsOk )
		{
			// Get new asset name. If we not press 'ok' nothing apply and exit from method
			{
				TSharedPtr<CInputTextDialog>	popup = owner->OpenPopup<CInputTextDialog>( TEXT( "Enter" ), TEXT( "New Asset Name" ), assetNode.GetName() );
				popup->OnTextEntered().Add( [&]( const std::string& InText )
											{
												bIsOk = true;
												newAssetName = ANSI_TO_TCHAR( InText.c_str() );
												eventResponse->Trigger();
											} );

				popup->OnCenceled().Add(	[&]()
											{
												 bIsOk = false;
												 eventResponse->Trigger();
											} );
				eventResponse->Wait();
				if ( !bIsOk )
				{
					return 0;
				}
			}		

			// If asset with new name already exist - try enter other name 
			if ( owner->package->IsExist( newAssetName ) )
			{
				TSharedPtr<CDialogWindow>	popup = owner->OpenPopup<CDialogWindow>( TEXT( "Error" ), CString::Format( TEXT( "Name '%s' already exist in package" ), newAssetName.c_str() ), CDialogWindow::BT_Ok );
				popup->OnButtonPressed().Add( [&]( CDialogWindow::EButtonType InButtonType )
											  {
												  eventResponse->Trigger();
											  } );
				bIsOk = false;
				eventResponse->Wait();
			}
		}

		// Find asset in package
		TSharedPtr<CAsset>		assetRef = owner->package->Find( assetNode.GetName() ).ToSharedPtr();
		if ( !assetRef )
		{
			return 0;
		}

		// Add change asset name
		assetRef->SetAssetName( newAssetName );
		return 0;
	}

	/**
	 * @brief Stop
	 *
	 * This is called if a thread is requested to terminate early
	 */
	virtual void Stop() override
	{
		GSynchronizeFactory->Destroy( eventResponse );
	}

	/**
	 * @brief Exit
	 *
	 * Called in the context of the aggregating thread to perform any cleanup.
	 */
	virtual void Exit() override
	{}

private:
	CContentBrowserWindow*				owner;			/**< Owner */
	CEvent*								eventResponse;	/**< Event used when opened popup of change exist assets */
	CContentBrowserWindow::CAssetNode	assetNode;		/**< Asset node to rename */
};

//
// CREATE ASSET RUNNABLE
//

/**
  * @ingroup WorldEd
  * @brief Runnable object for create asset in other thread
  */
template<class TAssetClass>
class TCreateAssetRunnable : public CRunnable
{
public:
	/**
	 * @brief Constructor
	 *
	 * @param InOwner			Owner
	 */
	TCreateAssetRunnable( CContentBrowserWindow* InOwner )
		: owner( InOwner )
		, eventResponse( nullptr )
	{
		package = owner->GetCurrentPackage();
	}

	/**
	 * @brief Initialize
	 *
	 * Allows per runnable object initialization. NOTE: This is called in the
	 * context of the thread object that aggregates this, not the thread that
	 * passes this runnable to a new thread.
	 *
	 * @return True if initialization was successful, false otherwise
	 */
	virtual bool Init() override
	{
		check( owner && package );
		eventResponse = GSynchronizeFactory->CreateSynchEvent();
		return true;
	}

	/**
	 * @brief Run
	 *
	 * This is where all per object thread work is done. This is only called
	 * if the initialization was successful.
	 *
	 * @return The exit code of the runnable object
	 */
	virtual uint32 Run() override
	{
		// Get new asset name and check on exist other asset with this name
		bool			bIsOk = false;
		std::wstring	assetName;
		while ( !bIsOk )
		{
			// Get new asset name. If we not press 'ok' nothing apply and exit from method
			{
				TSharedPtr<CInputTextDialog>	popup = owner->OpenPopup<CInputTextDialog>( TEXT( "Enter" ), TEXT( "Asset Name" ), TEXT( "NewAsset" ) );
				popup->OnTextEntered().Add( [&]( const std::string& InText )
											{
												bIsOk = true;
												assetName = ANSI_TO_TCHAR( InText.c_str() );
												eventResponse->Trigger();
											} );

				popup->OnCenceled().Add( [&]()
										 {
											 bIsOk = false;
											 eventResponse->Trigger();
										 } );
				eventResponse->Wait();
				if ( !bIsOk )
				{
					return 0;
				}
			}

			// If asset with new name already exist - try enter other name 
			if ( package->IsExist( assetName ) )
			{
				TSharedPtr<CDialogWindow>	popup = owner->OpenPopup<CDialogWindow>( TEXT( "Error" ), CString::Format( TEXT( "Name '%s' already exist in package" ), assetName.c_str() ), CDialogWindow::BT_Ok );
				popup->OnButtonPressed().Add( [&]( CDialogWindow::EButtonType InButtonType )
											  {
												  eventResponse->Trigger();
											  } );
				bIsOk = false;
				eventResponse->Wait();
			}
		}

		// Create new asset
		TSharedPtr<TAssetClass>		asset = MakeSharedPtr<TAssetClass>();
		asset->SetAssetName( assetName );
		
		// Add asset to package
		package->Add( asset->GetAssetHandle() );
		return 0;
	}

	/**
	 * @brief Stop
	 *
	 * This is called if a thread is requested to terminate early
	 */
	virtual void Stop() override
	{
		GSynchronizeFactory->Destroy( eventResponse );
	}

	/**
	 * @brief Exit
	 *
	 * Called in the context of the aggregating thread to perform any cleanup.
	 */
	virtual void Exit() override
	{}

private:
	PackageRef_t			package;		/**< Package */
	CContentBrowserWindow*	owner;			/**< Owner */
	CEvent*					eventResponse;	/**< Event used when opened popup of change exist assets */
};

//
// MOVE/COPY FILES
//

/**
  * @ingroup WorldEd
  * @brief Runnable object for move/copy files in other thread
 */
class CMoveCopyFilesRunnable : public CRunnable
{
public:
	/**
	 * @brief Info about copied/moved file
	 */
	struct SFileInfo
	{
		bool		bDirectory;		/**< File is directory? */
		CFilename	srcPath;		/**< Source path to file */
		CFilename	dstPath;		/**< Destination path to file */
	};

	/**
	 * @brief Constructor
	 *
	 * @param InOwner			Owner
	 * @param InFilesToMoveCopy	Array of files to move/copy
	 */
	CMoveCopyFilesRunnable( CContentBrowserWindow* InOwner, const std::vector<SFileInfo>& InFilesToMoveCopy )
		: bMove( true )
		, mode( CMM_Default )
		, owner( InOwner )
		, eventResponse( nullptr )
		, filesToMoveCopy( InFilesToMoveCopy )
	{}

	/**
	 * @brief Initialize
	 *
	 * Allows per runnable object initialization. NOTE: This is called in the
	 * context of the thread object that aggregates this, not the thread that
	 * passes this runnable to a new thread.
	 *
	 * @return True if initialization was successful, false otherwise
	 */
	virtual bool Init() override
	{
		check( owner );
		eventResponse = GSynchronizeFactory->CreateSynchEvent();

		// Expanding all folders. It need for we contain in array only files
		std::vector<SFileInfo>		oldFilesToMoveCopy = filesToMoveCopy;
		filesToMoveCopy.swap( std::vector<SFileInfo>() );
		for ( uint32 index = 0, count = oldFilesToMoveCopy.size(); index < count; ++index )
		{
			const SFileInfo&	oldFileInfo = oldFilesToMoveCopy[index];
			if ( oldFileInfo.bDirectory )
			{
				GetFilesFromDirectory( oldFileInfo, filesToMoveCopy );
				directorisToDelete.push_back( oldFileInfo.srcPath );
			}
			else
			{
				filesToMoveCopy.push_back( oldFileInfo );
			}
		}

		return true;
	}

	/**
	 * @brief Run
	 *
	 * This is where all per object thread work is done. This is only called
	 * if the initialization was successful.
	 *
	 * @return The exit code of the runnable object
	 */
	virtual uint32 Run() override
	{
		// TODO: yehor.pohuliaka. Need add dialog window with select mode (move or copy) and add progress bar
		
		// Copy/move all files
		bool				bDirtyTOC = false;
		std::wstring		errorMessages;
		for ( uint32 index = 0, count = filesToMoveCopy.size(); index < count; ++index )
		{
			const SFileInfo&	fileInfo			= filesToMoveCopy[index];
			bool				bExistFile			= !fileInfo.bDirectory && GFileSystem->IsExistFile( fileInfo.dstPath.GetFullPath() );
			bool				bPackage			= fileInfo.srcPath.GetExtension() == TEXT( "pak" );
			if ( bExistFile )
			{
				if ( mode == CMM_SkipConflicts )
				{
					continue;
				}
				else if ( mode != CMM_Force )
				{
					CDialogWindow::EButtonType	pressedButton;
					TSharedPtr<CDialogWindow>	popup = owner->OpenPopup<CDialogWindow>( TEXT( "Question" ), CString::Format( TEXT( "File '%s' already exist in folder '%s'.\n\nDo you want replace him?" ), fileInfo.srcPath.GetFullPath().c_str(), fileInfo.dstPath.GetPath().c_str() ), CDialogWindow::BT_Yes | CDialogWindow::BT_YesToAll | CDialogWindow::BT_No | CDialogWindow::BT_NoToAll );
					popup->OnButtonPressed().Add( [&]( CDialogWindow::EButtonType InButtonType )
												  {
													  pressedButton = InButtonType;
													  eventResponse->Trigger();
												  } );
					eventResponse->Wait();
					switch ( pressedButton )
					{
					case CDialogWindow::BT_Yes:
						break;

					case CDialogWindow::BT_No:
						continue;

					case CDialogWindow::BT_YesToAll:
						mode = CMM_Force;
						break;

					case CDialogWindow::BT_NoToAll:
						mode = CMM_SkipConflicts;
						continue;
					}
				}
			}

			//  If we moving files and that is current package. We have to unload him and remove entry from TOC file
			if ( bMove && bPackage )
			{
				// We try unload package for remove unused assets.
				// If package still used, we skip him
				if ( !GPackageManager->UnloadPackage( fileInfo.srcPath.GetFullPath() ) )
				{
					errorMessages += CString::Format( TEXT( "\n%s: Package in using or modifided and cannot be moved" ), fileInfo.srcPath.GetBaseFilename().c_str() );
					continue;
				}

				// Otherwise we remove entry from TOC file
				GTableOfContents.RemoveEntry( fileInfo.srcPath.GetFullPath() );
				bDirtyTOC = true;
			}

			// Copy/move file		
			ECopyMoveResult	result;
			if ( bMove )
			{
				result = GFileSystem->Move( fileInfo.dstPath.GetFullPath(), fileInfo.srcPath.GetFullPath(), true, true );
			}
			else
			{
				result = GFileSystem->Copy( fileInfo.dstPath.GetFullPath(), fileInfo.srcPath.GetFullPath(), true, true );
			}

			if ( result != CMR_OK )
			{
				const tchar*			errorMsg;
				switch ( result )
				{
				case CMR_MiscFail:		errorMsg = TEXT( "Misc Fail" );		break;
				case CMR_ReadFail:		errorMsg = TEXT( "Read Fail" );		break;
				case CMR_WriteFail:		errorMsg = TEXT( "Write Fail" );	break;
				case CMR_Canceled:		errorMsg = TEXT( "Canceled" );		break;
				default:				errorMsg = TEXT( "Unknown Error" );	break;
				}

				errorMessages += CString::Format( TEXT( "\n%s: %s" ), fileInfo.srcPath.GetFullPath().c_str(), errorMsg );
				if ( bMove && bPackage )		// We must restore entry in TOC file if this is package
				{
					GTableOfContents.AddEntry( fileInfo.srcPath.GetFullPath() );
				}
			}
			else
			{
				// We update TOC file if this is package
				if ( bPackage )
				{
					GTableOfContents.AddEntry( fileInfo.dstPath.GetFullPath() );
					bDirtyTOC = true;
				}
			}
		}
	
		// Delete old directories if all files has been moved
		if ( bMove )
		{
			for ( uint32 index = 0, count = directorisToDelete.size(); index < count; ++index )
			{
				std::wstring		fullDirPath = directorisToDelete[index].GetFullPath();
				if ( IsEmptyDirectory( fullDirPath, true ) )
				{
					GFileSystem->DeleteDirectory( fullDirPath, true );
				}
			}
		}

		// If TOC file is dirty, we serialize him
		if ( bDirtyTOC )
		{
			GEditorEngine->SerializeTOC( true );
		}

		// If exist errors, show popup window
		if ( !errorMessages.empty() )
		{
			owner->OpenPopup<CDialogWindow>( TEXT( "Error" ), CString::Format( TEXT( "The following files not copy/moved with following errors\n\nFiles:%s" ), errorMessages.c_str() ), CDialogWindow::BT_Ok );
		}
		return 0;
	}

	/**
	 * @brief Stop
	 *
	 * This is called if a thread is requested to terminate early
	 */
	virtual void Stop() override
	{
		GSynchronizeFactory->Destroy( eventResponse );
	}

	/**
	 * @brief Exit
	 *
	 * Called in the context of the aggregating thread to perform any cleanup.
	 */
	virtual void Exit() override
	{}

private:
	/**
	 * @brief Enumeration of move/copy modes
	 */
	enum ECopyMoveMode
	{
		CMM_Default,		/**< Default move/copy mode. When moved/copied file is exist in folder, will show dialog window with question is need change it or no */
		CMM_Force,			/**< Move/copy all files and ignore conflicts */
		CMM_SkipConflicts	/**< Skip conflicts while move/copy files */
	};

	/**
	 * @brief Get files from directory (recursive)
	 * 
	 * @param InFileInfo	Info about directory
	 * @param OutResult		Output array of info about files in directory
	 */
	void GetFilesFromDirectory( const SFileInfo& InFileInfo, std::vector<SFileInfo>& OutResult ) const
	{
		// Do nothing if file info it's not directory
		if ( !InFileInfo.bDirectory )
		{
			return;
		}

		// Find all files in directory
		std::wstring				srcDirectory		= InFileInfo.srcPath.GetPath() + InFileInfo.srcPath.GetBaseFilename() + PATH_SEPARATOR;
		std::wstring				dstDirectory		= InFileInfo.dstPath.GetPath() + InFileInfo.dstPath.GetBaseFilename() + PATH_SEPARATOR;
		std::vector<std::wstring>	files				= GFileSystem->FindFiles( srcDirectory, true, true );
		for ( uint32 index = 0, count = files.size(); index < count; ++index )
		{
			CFilename			filename = files[index];
			SFileInfo			fileInfo;
			fileInfo.dstPath	= dstDirectory + filename.GetFullPath();
			fileInfo.srcPath	= srcDirectory + filename.GetFullPath();
			fileInfo.bDirectory = GFileSystem->IsDirectory( fileInfo.srcPath.GetFullPath() );		
			
			if ( fileInfo.bDirectory )
			{
				GetFilesFromDirectory( fileInfo, OutResult );
			}
			else
			{
				OutResult.push_back( fileInfo );
			}
		}
	}

	bool						bMove;				/**< Is need move files? If FALSE we will copy they */
	ECopyMoveMode				mode;				/**< Copy/move mode */
	CContentBrowserWindow*		owner;				/**< Owner */
	CEvent*						eventResponse;		/**< Event used when opened popup of change exist files */
	std::vector<SFileInfo>		filesToMoveCopy;	/**< Array of files to move/copy */
	std::vector<CFilename>		directorisToDelete;	/**< Array of directories who need delete after move */
};

//
// CREATE FILE RUNNABLE
//

/**
  * @ingroup WorldEd
  * @brief Runnable object for create file in other thread
  */
class CCreateFileRunnable : public CRunnable
{
public:
	/**
	 * @brief Enumeration create file mode
	 */
	enum ECreateMode
	{
		CM_CreateFolder,		/**< Create folder */
		CM_CreatePackage		/**< Create package */
	};

	/**
	 * @brief Constructor
	 *
	 * @param InOwner			Owner
	 * @param InRootNode		Root node where we create file
	 * @param InCreateMode		Create mode
	 */
	CCreateFileRunnable( CContentBrowserWindow* InOwner, const TSharedPtr<CContentBrowserWindow::CFileTreeNode>& InRootNode, ECreateMode InCreateMode )
		: mode( InCreateMode )
		, owner( InOwner )
		, eventResponse( nullptr )
		, rootNode( InRootNode )
	{}

	/**
	 * @brief Initialize
	 *
	 * Allows per runnable object initialization. NOTE: This is called in the
	 * context of the thread object that aggregates this, not the thread that
	 * passes this runnable to a new thread.
	 *
	 * @return True if initialization was successful, false otherwise
	 */
	virtual bool Init() override
	{
		check( owner && rootNode );
		eventResponse = GSynchronizeFactory->CreateSynchEvent();
		return true;
	}

	/**
	 * @brief Run
	 *
	 * This is where all per object thread work is done. This is only called
	 * if the initialization was successful.
	 *
	 * @return The exit code of the runnable object
	 */
	virtual uint32 Run() override
	{
		// Get file name and check on exist other file with this name
		bool			bIsOk		= false;
		std::wstring	fileName;
		std::wstring	rootDir		= ( rootNode->GetType() == CContentBrowserWindow::FNT_Folder ? rootNode->GetPath() : CFilename( rootNode->GetPath() ).GetPath() ) + PATH_SEPARATOR;
		while ( !bIsOk )
		{
			// Get file name. If we not press 'ok' nothing apply and exit from method
			{
				TSharedPtr<CInputTextDialog>	popup = owner->OpenPopup<CInputTextDialog>( TEXT( "Enter" ), mode == CM_CreateFolder ? TEXT( "New Directory Name" ) : TEXT("New Package Name"), mode == CM_CreateFolder ? TEXT( "NewDirectory" ) : TEXT( "MyPackage" ) );
				popup->OnTextEntered().Add( [&]( const std::string& InText )
											{
												bIsOk = true;
												fileName = ANSI_TO_TCHAR( InText.c_str() );
												eventResponse->Trigger();
											} );

				popup->OnCenceled().Add( [&]()
										 {
											 bIsOk = false;
											 eventResponse->Trigger();
										 } );
				eventResponse->Wait();
				if ( !bIsOk )
				{
					return 0;
				}
			}

			switch ( mode )
			{
				// If folder with name already exist - try enter other name 
			case CM_CreateFolder:			
				if ( GFileSystem->IsExistFile( rootDir + fileName ) )
				{
					TSharedPtr<CDialogWindow>	popup = owner->OpenPopup<CDialogWindow>( TEXT( "Error" ), CString::Format( TEXT( "Folder '%s' already exist" ), fileName.c_str() ), CDialogWindow::BT_Ok );
					popup->OnButtonPressed().Add( [&]( CDialogWindow::EButtonType InButtonType )
												  {
													  eventResponse->Trigger();
												  } );
					bIsOk = false;
					eventResponse->Wait();
				}
				break;

				// If package with name already exist - try enter other name 
			case CM_CreatePackage:
				if ( GFileSystem->IsExistFile( rootDir + fileName + TEXT( ".pak" ) ) || !GTableOfContents.GetPackagePath( fileName ).empty() )
				{
					TSharedPtr<CDialogWindow>	popup = owner->OpenPopup<CDialogWindow>( TEXT( "Error" ), CString::Format( TEXT( "Package '%s' already exist" ), fileName.c_str() ), CDialogWindow::BT_Ok );
					popup->OnButtonPressed().Add( [&]( CDialogWindow::EButtonType InButtonType )
												  {
													  eventResponse->Trigger();
												  } );
					bIsOk = false;
					eventResponse->Wait();
				}
				break;

			default: 
				appErrorf( TEXT( "Unknown mode 0x%X" ), mode );
				return 1;
			}
		}

		switch ( mode )
		{
			// Create directory
		case CM_CreateFolder:
			GFileSystem->MakeDirectory( rootDir + fileName );
			break;

			// Create new package
		case CM_CreatePackage:
		{
			std::wstring	fullPath	= rootDir + fileName + TEXT( ".pak" );
			PackageRef_t	package		= GPackageManager->LoadPackage( fullPath, true );
			if ( !package->Save( fullPath ) )
			{
				GPackageManager->UnloadPackage( fullPath, true );
				owner->OpenPopup<CDialogWindow>( TEXT( "Error" ), CString::Format( TEXT( "Failed save package '%s'" ), fileName.c_str() ), CDialogWindow::BT_Ok );
				return 0;
			}

			// Update TOC file and serialize him
			GTableOfContents.AddEntry( package->GetGUID(), package->GetName(), package->GetFileName() );
			GEditorEngine->SerializeTOC( true );
			break;
		}

		default:
			appErrorf( TEXT( "Unknown mode 0x%X" ), mode );
			return 1;
		}

		return 0;
	}

	/**
	 * @brief Stop
	 *
	 * This is called if a thread is requested to terminate early
	 */
	virtual void Stop() override
	{
		GSynchronizeFactory->Destroy( eventResponse );
	}

	/**
	 * @brief Exit
	 *
	 * Called in the context of the aggregating thread to perform any cleanup.
	 */
	virtual void Exit() override
	{}

private:
	ECreateMode											mode;			/**< Create mode */
	CContentBrowserWindow*								owner;			/**< Owner */
	CEvent*												eventResponse;	/**< Event used when opened popup */
	TSharedPtr<CContentBrowserWindow::CFileTreeNode>	rootNode;		/**< Root node where we create package */
};

//
// RENAME FILE RUNNABLE
//

/**
  * @ingroup WorldEd
  * @brief Runnable object for rename file in other thread
  */
class CRenameFileRunnable : public CRunnable
{
public:
	/**
	 * @brief Enumeration rename file mode
	 */
	enum ERenameMode
	{
		RM_RenameFolder,	/**< Rename folder */
		RM_RenamePackage	/**< Rename package */
	};

	/**
	 * @brief Constructor
	 *
	 * @param InOwner			Owner
	 * @param InNodeToRename	Node to rename
	 * @param InRenameMode		Rename mode
	 */
	CRenameFileRunnable( CContentBrowserWindow* InOwner, const TSharedPtr<CContentBrowserWindow::CFileTreeNode>& InNodeToRename, ERenameMode InRenameMode )
		: mode( InRenameMode )
		, owner( InOwner )
		, eventResponse( nullptr )
		, nodeToRename( InNodeToRename )
	{}

	/**
	 * @brief Initialize
	 *
	 * Allows per runnable object initialization. NOTE: This is called in the
	 * context of the thread object that aggregates this, not the thread that
	 * passes this runnable to a new thread.
	 *
	 * @return True if initialization was successful, false otherwise
	 */
	virtual bool Init() override
	{
		check( owner && nodeToRename );
		eventResponse = GSynchronizeFactory->CreateSynchEvent();
		return true;
	}

	/**
	 * @brief Run
	 *
	 * This is where all per object thread work is done. This is only called
	 * if the initialization was successful.
	 *
	 * @return The exit code of the runnable object
	 */
	virtual uint32 Run() override
	{
		// Get file name and check on exist other file with this name
		bool			bIsOk		= false;
		bool			bDirtyTOC	= false;
		std::wstring	newFileName;
		CFilename		filename	= nodeToRename->GetPath();
		bool			bPackage	= filename.GetExtension() == TEXT( "pak" );
		check( mode == RM_RenamePackage && bPackage || mode == RM_RenameFolder );

		while ( !bIsOk )
		{
			// Get file name. If we not press 'ok' nothing apply and exit from method
			{
				TSharedPtr<CInputTextDialog>	popup = owner->OpenPopup<CInputTextDialog>( TEXT( "Enter" ), mode == RM_RenameFolder ? TEXT( "New Directory Name" ) : TEXT( "New Package Name" ), mode == RM_RenameFolder ? nodeToRename->GetName() : CFilename( nodeToRename->GetName() ).GetBaseFilename() );
				popup->OnTextEntered().Add( [&]( const std::string& InText )
											{
												bIsOk = true;
												newFileName = ANSI_TO_TCHAR( InText.c_str() );
												eventResponse->Trigger();
											} );

				popup->OnCenceled().Add( [&]()
										 {
											 bIsOk = false;
											 eventResponse->Trigger();
										 } );
				eventResponse->Wait();
				if ( !bIsOk )
				{
					return 0;
				}
			}

			switch ( mode )
			{
				// If folder with name already exist - try enter other name 
			case RM_RenameFolder:
				if ( GFileSystem->IsExistFile( filename.GetPath() + newFileName ) )
				{
					TSharedPtr<CDialogWindow>	popup = owner->OpenPopup<CDialogWindow>( TEXT( "Error" ), CString::Format( TEXT( "Folder '%s' already exist" ), newFileName.c_str() ), CDialogWindow::BT_Ok );
					popup->OnButtonPressed().Add( [&]( CDialogWindow::EButtonType InButtonType )
												  {
													  eventResponse->Trigger();
												  } );
					bIsOk = false;
					eventResponse->Wait();
				}
				break;

				// If package with name already exist - try enter other name 
			case RM_RenamePackage:
				if ( GFileSystem->IsExistFile( filename.GetPath() + newFileName + TEXT( ".pak" ) ) || !GTableOfContents.GetPackagePath( newFileName ).empty() )
				{
					TSharedPtr<CDialogWindow>	popup = owner->OpenPopup<CDialogWindow>( TEXT( "Error" ), CString::Format( TEXT( "Package '%s' already exist" ), newFileName.c_str() ), CDialogWindow::BT_Ok );
					popup->OnButtonPressed().Add( [&]( CDialogWindow::EButtonType InButtonType )
												  {
													  eventResponse->Trigger();
												  } );
					bIsOk = false;
					eventResponse->Wait();
				}
				break;

			default:
				appErrorf( TEXT( "Unknown mode 0x%X" ), mode );
				return 1;
			}
		}

		switch ( mode )
		{
			// Rename directory
		case RM_RenameFolder:
		{
			std::vector<SPackageInfo>		updateTOCPackages;
			GetPackagesInDirectory( filename.GetFullPath(), filename.GetPath() + newFileName, updateTOCPackages );
			
			// We try unload all package in directory for rename folder.
			// If package still used, we exit with error
			for ( uint32 index = 0, count = updateTOCPackages.size(); index < count; ++index )
			{
				const SPackageInfo&		packageInfo = updateTOCPackages[index];
				if ( !GPackageManager->UnloadPackage( packageInfo.oldPath.GetFullPath() ) )
				{
					owner->OpenPopup<CDialogWindow>( TEXT( "Warning" ), CString::Format( TEXT( "The package '%s' in using or modifided and folder '%s' cannot be renamed. Close all assets from this package or save him for will allow rename folder" ), packageInfo.oldPath.GetBaseFilename().c_str(), filename.GetBaseFilename().c_str() ), CDialogWindow::BT_Ok );
					return 2;
				}
			}

			// All good and we remove entries from TOC file
			for ( uint32 index = 0, count = updateTOCPackages.size(); index < count; ++index )
			{
				const SPackageInfo&		packageInfo = updateTOCPackages[index];
				GTableOfContents.RemoveEntry( packageInfo.oldPath.GetFullPath() );
				bDirtyTOC = true;
			}

			ECopyMoveResult		result = GFileSystem->Move( filename.GetPath() + newFileName, filename.GetFullPath() );
			check( result == CMR_OK );

			// Insert new entries to TOC file
			for ( uint32 index = 0, count = updateTOCPackages.size(); index < count; ++index )
			{
				GTableOfContents.AddEntry( updateTOCPackages[index].newPath.GetFullPath() );
				bDirtyTOC = true;
			}
			break;
		}

			// Rename package
		case RM_RenamePackage:
		{
			std::wstring	newFullPath = filename.GetPath() + newFileName + filename.GetExtension( true );

			// If this is package need call SetName in him
			if ( bPackage )
			{
				// We try unload package for remove unused assets.
				// If package still used, we skip him
				if ( !GPackageManager->UnloadPackage( filename.GetFullPath() ) )
				{
					owner->OpenPopup<CDialogWindow>( TEXT( "Warning" ), CString::Format( TEXT( "The package '%s' in using or modifided and cannot be renamed. Close all assets from this package or save him for will allow them to be rename" ), filename.GetBaseFilename().c_str() ), CDialogWindow::BT_Ok );
					return 2;
				}

				bool			bIsNeedUnloadPackage = !GPackageManager->IsPackageLoaded( filename.GetFullPath() );
				PackageRef_t	package = GPackageManager->LoadPackage( filename.GetFullPath() );

				// If package failed loaded - we not change him name
				if ( !package )
				{
					owner->OpenPopup<CDialogWindow>( TEXT( "Error" ), CString::Format( TEXT( "File '%s' not renamed because failed loading package for change him name" ), filename.GetFullPath().c_str() ), CDialogWindow::BT_Ok );
					return 2;
				}

				// Else we change name and resave package
				package->SetName( newFileName );
				package->Save( package->GetFileName() );

				// Update TOC file
				GTableOfContents.RemoveEntry( package->GetGUID() );
				GTableOfContents.AddEntry( package->GetGUID(), package->GetName(), newFullPath );
				bDirtyTOC = true;

				// If this is package and him opened in package browser, we close it 
				if ( owner->GetCurrentPackage() == package )
				{
					owner->SetCurrentPackage( nullptr );
				}

				// If need unload package - do it
				if ( bIsNeedUnloadPackage )
				{
					GPackageManager->UnloadPackage( filename.GetFullPath() );
				}
			}

			GFileSystem->Move( newFullPath, filename.GetFullPath() );
			break;
		}

		default:
			appErrorf( TEXT( "Unknown mode 0x%X" ), mode );
			return 1;
		}

		// If TOC file is dirty, we serialize him to cache
		if ( bDirtyTOC )
		{
			GEditorEngine->SerializeTOC( true );
		}

		return 0;
	}

	/**
	 * @brief Stop
	 *
	 * This is called if a thread is requested to terminate early
	 */
	virtual void Stop() override
	{
		GSynchronizeFactory->Destroy( eventResponse );
	}

	/**
	 * @brief Exit
	 *
	 * Called in the context of the aggregating thread to perform any cleanup.
	 */
	virtual void Exit() override
	{}

private:
	ERenameMode											mode;			/**< Rename mode */
	CContentBrowserWindow*								owner;			/**< Owner */
	CEvent*												eventResponse;	/**< Event used when opened popup */
	TSharedPtr<CContentBrowserWindow::CFileTreeNode>	nodeToRename;	/**< Node to rename */
};

//
// CONTENT BROWSER WINDOW
//

CContentBrowserWindow::CContentBrowserWindow( const std::wstring& InName )
	: CImGUILayer( InName )
	, padding( 16.f )
	, thumbnailSize( 64.f )
{}

void CContentBrowserWindow::Init()
{
	CImGUILayer::Init();

	// Loading all of asset icons
	std::wstring		errorMsg;
	for ( uint32 index = 0; index < AT_Count; ++index )
	{
		std::vector<TSharedPtr<CAsset>>		result;
		if ( !CTexture2DImporter::Import( appBaseDir() + TEXT( "Engine/Editor/Thumbnails/" ) + GAssetIconPaths[index], result, errorMsg ) )
		{
			LE_LOG( LT_Warning, LC_Editor, TEXT( "Failed to load asset icon '%s' for type 0x%X. Message: %s" ), GAssetIconPaths[index], index, errorMsg.c_str() );
			assetIcons[index] = GEngine->GetDefaultTexture();
		}
		else
		{
			TAssetHandle<CTexture2D>	assetHandle = result[0]->GetAssetHandle();
			PackageRef_t				package = GPackageManager->LoadPackage( TEXT( "" ), true );
			check( package );

			package->Add( assetHandle );
			assetIcons[index] = assetHandle;
		}
	}

	// Create root nodes for engine and game directories
	engineRoot	= MakeSharedPtr<CFileTreeNode>( FNT_Folder, TEXT( "Engine" ), appBaseDir() + PATH_SEPARATOR TEXT( "Engine" ) PATH_SEPARATOR TEXT( "Content" ) PATH_SEPARATOR, this );
	gameRoot	= MakeSharedPtr<CFileTreeNode>( FNT_Folder, TEXT( "Game" ), appGameDir() + PATH_SEPARATOR TEXT( "Content" ) PATH_SEPARATOR, this );
}

void CContentBrowserWindow::OnTick()
{
	// If window focused, we reset current hovered node
	if ( ImGui::IsWindowFocused() )
	{
		hoveredNode = nullptr;
	}

	ImGui::Columns( 2 );

	// Draw file system tree
	{
		ImGui::InputTextWithHint( "##FilterPackages", "Search", &filterInfo.fileName );
		ImGui::SameLine( 0, 0 );
		if ( ImGui::Button( "X##0" ) )
		{
			filterInfo.fileName.clear();
		}
		ImGui::Spacing();
		ImGui::BeginChild( "##Packages", ImVec2{ 0.f, 0.f }, false, ImGuiWindowFlags_HorizontalScrollbar );
		ImGui::PushStyleColor( ImGuiCol_Button, ImVec4( 0.f, 0.f, 0.f, 0.f ) );
		
		engineRoot->Tick();
		gameRoot->Tick();
		DrawPackagesPopupMenu();
		
		ImGui::PopStyleColor();	
		ImGui::EndChild();
		ImGui::NextColumn();
	}

	// Draw assets list
	{
		ImGui::InputTextWithHint( "##FilterAssets", "Search", &filterInfo.assetName );
		ImGui::SameLine( 0, 0 );
		if ( ImGui::Button( "X##1" ) )
		{
			filterInfo.assetName.clear();
		}

		ImGui::SameLine();
		if ( ImGui::BeginCombo( "##FilterAssetTypes", filterInfo.GetPreviewFilterAssetType().c_str() ) )
		{
			bool		bEnabledAllTypes = filterInfo.IsShowAllAssetTypes();
			if ( ImGui::Selectable( "All", &bEnabledAllTypes, ImGuiSelectableFlags_DontClosePopups ) )
			{
				filterInfo.SetShowAllAssetTypes( bEnabledAllTypes );
			}

			for ( uint32 index = AT_FirstType; index < AT_Count; ++index )
			{
				ImGui::Selectable( TCHAR_TO_ANSI( ConvertAssetTypeToText( ( EAssetType )index ).c_str() ), &filterInfo.assetTypes[index-1], ImGuiSelectableFlags_DontClosePopups );
			}
			ImGui::EndCombo();
		}
		ImGui::Separator();

		// Draw assets in the package
		if ( package )
		{
			// Section of assets
			ImGui::Spacing();
			ImGui::BeginChild( "##Assets" );
			float	panelWidth	= ImGui::GetContentRegionAvail().x;
			int32	columnCount = panelWidth / ( thumbnailSize + padding );
			if ( columnCount < 1 )
			{
				columnCount = 1;
			}

			// Refresh asset nodes
			RefreshAssetNodes();

			// Draw all asset nodes
			ImGui::Columns( columnCount, 0, false );	
			for ( uint32 index = 0, count = assets.size(); index < count; ++index )
			{
				CAssetNode&		assetNode = assets[index];
				if ( filterInfo.assetTypes[assetNode.GetType() - 1] && CString::InString( assetNode.GetName(), ANSI_TO_TCHAR( filterInfo.assetName.c_str() ), true ) )
				{
					assetNode.Tick();
					ImGui::NextColumn();
				}
			}

			// Unselect all assets if clicked on empty space
			if ( ImGui::IsWindowHovered() && ( ImGui::IsMouseClicked( ImGuiMouseButton_Left ) || ( !ImGui::IsAnyItemHovered() && ImGui::IsMouseDown( ImGuiMouseButton_Right ) ) ) )
			{
				for ( uint32 index = 0, count = assets.size(); index < count; ++index )
				{
					assets[index].SetSelect( false );
				}
			}

			// Draw popup menu of assets
			DrawAssetsPopupMenu();
			ImGui::EndChild();
		}
	}

	ImGui::EndColumns();
}

void CContentBrowserWindow::ShowAsset( const std::wstring& InAssetReference )
{
	// If asset reference is not valid, we nothing do
	if ( InAssetReference.empty() )
	{
		return;
	}

	// Parse asset reference
	EAssetType		assetType;
	std::wstring	packageName;
	std::wstring	assetName;
	bool			bResult = ParseReferenceToAsset( InAssetReference, packageName, assetName, assetType );
	if ( !bResult )
	{
		return;
	}

	// Find path to package from TOC table and load him
	PackageRef_t	package;
	std::wstring	packagePath;
	{
		packagePath = GTableOfContents.GetPackagePath( packageName );
		if ( !packagePath.empty() )
		{
			package = GPackageManager->LoadPackage( packagePath );
		}
	}

	// If package not loaded, we exit from method
	if ( !package )
	{
		return;
	}

	// Set new current package and refresh asset nodes
	SetCurrentPackage( package );
	RefreshAssetNodes();

	// Select asset
	for ( uint32 index = 0, count = assets.size(); index < count; ++index )
	{
		CAssetNode&		assetNode = assets[index];
		if ( assetNode.GetName() == assetName )
		{
			assetNode.SetSelect( true );
			break;
		}
	}
}

std::wstring CContentBrowserWindow::GetSelectedAssetReference() const
{
	std::wstring	result;
	
	// If current package is valid, we find first selected asset
	if ( package )
	{
		const CAssetNode*	assetNode = nullptr;
		for ( uint32 index = 0, count = assets.size(); index < count; ++index )
		{
			if ( assets[index].IsSelected() )
			{
				assetNode = &assets[index];
				break;
			}
		}

		if ( assetNode )
		{
			MakeReferenceToAsset( package->GetName(), assetNode->GetName(), assetNode->GetType(), result );
		}
	}

	return result;
}

void CContentBrowserWindow::RefreshAssetNodes()
{
	// Mark all asset nodes to delete
	bool											bNeedRemoveAssetNodes = false;
	std::unordered_map<std::wstring, uint32>		mapAssets;						// For fast find asset nodes in array by name
	for ( uint32 index = 0, count = assets.size(); index < count; ++index )
	{
		CAssetNode&				assetNode = assets[index];
		bNeedRemoveAssetNodes	= true;
		
		// If asset in package is deleted, we skip him
		if ( !assetNode.GetAssetInfo() )
		{
			continue;
		}

		mapAssets.insert( std::make_pair( assetNode.GetName(), index ) );
	}

	// Add new asset node if in package is added new resources
	if ( package )
	{
		for ( uint32 index = 0, count = package->GetNumAssets(); index < count; ++index )
		{
			const SAssetInfo*	assetInfo = nullptr;
			package->GetAssetInfo( index, assetInfo );		
			check( assetInfo );
			auto			itFind = mapAssets.find( assetInfo->name );
			
			// If this asset is new, we insert node
			if ( itFind == mapAssets.end() )
			{
				CAssetNode	assetNode( assetInfo, this );
				assets.push_back( assetNode );
			}
		}
	}

	// If need remove some asset nodes
	if ( bNeedRemoveAssetNodes )
	{
		for ( uint32 index = 0; index < assets.size(); ++index )
		{
			const CAssetNode&	assetNode = assets[index];
			if ( !assetNode.GetAssetInfo() )
			{
				assets.erase( assets.begin() + index );
				--index;
			}
		}
	}
}

void CContentBrowserWindow::DrawAssetsPopupMenu()
{
	if ( ImGui::BeginPopupContextWindow( "", ImGuiMouseButton_Right ) )
	{
		std::vector<CAssetNode>		selectedAssets;
		for ( uint32 index = 0, count = assets.size(); index < count; ++index )
		{
			const CAssetNode&	asset = assets[index];
			if ( asset.IsSelected() )
			{
				selectedAssets.push_back( asset );
			}
		}

		bool	bSelectedOnlyOneAsset	= selectedAssets.size() == 1;
		bool	bSelectedAssets			= !selectedAssets.empty();

		if ( ImGui::BeginMenu( "Create" ) )
		{
			// Create material
			if ( ImGui::MenuItem( "Material" ) )
			{
				GThreadFactory->CreateThread( new TCreateAssetRunnable<CMaterial>( this ), TEXT( "CreateAsset" ), true, true );
			}
			
			if ( ImGui::BeginMenu( "Physics" ) )
			{
				// Create physics material
				if ( ImGui::MenuItem( "Physics Material" ) )
				{
					GThreadFactory->CreateThread( new TCreateAssetRunnable<CPhysicsMaterial>( this ), TEXT( "CreateAsset" ), true, true );
				}
				ImGui::EndMenu();
			}		
			ImGui::EndMenu();
		}

		ImGui::Separator();

		// Reload assets
		if ( ImGui::MenuItem( "Reload", "", nullptr, bSelectedAssets ) )
		{
			PopupMenu_Asset_Reload( selectedAssets );
		}
		
		// Import asset
		if ( ImGui::MenuItem( "Import" ) )
		{
			PopupMenu_Asset_Import();
		}
		
		// Reimport assets
		if ( ImGui::MenuItem( "Reimport", "", nullptr, bSelectedAssets ) )
		{
			PopupMenu_Asset_Reimport( selectedAssets );
		}

		// Reimport with new file
		if ( ImGui::MenuItem( "Reimport With New File", "", nullptr, bSelectedOnlyOneAsset ) )
		{
			PopupMenu_Asset_ReimportWithNewFile( selectedAssets[0] );
		}

		ImGui::Separator();

		// Delete assets
		if ( ImGui::MenuItem( "Delete", "", nullptr, bSelectedAssets ) )
		{
			// Convert selected assets to string with they's names for popup
			std::wstring				selectedAssetNames;
			for ( uint32 index = 0, count = selectedAssets.size(); index < count; ++index )
			{
				selectedAssetNames += CString::Format( TEXT( "\n%s" ), selectedAssets[index].GetAssetInfo()->name.c_str() );
			}

			TSharedPtr<CDialogWindow>		popup = OpenPopup<CDialogWindow>( TEXT( "Question" ), CString::Format( TEXT( "Is need delete selected assets?\n\nAssets:%s" ), selectedAssetNames.c_str() ), CDialogWindow::BT_Ok | CDialogWindow::BT_Cancel );
			popup->OnButtonPressed().Add( [&]( CDialogWindow::EButtonType InButtonType )
										  {
											  // We pressed 'Ok', delete selected folders
											  if ( InButtonType == CDialogWindow::BT_Ok )
											  {
												  PopupMenu_Asset_Delete();
											  }
										  } );
		}

		// Rename an asset
		if ( ImGui::MenuItem( "Rename", "", nullptr, bSelectedOnlyOneAsset ) )
		{
			GThreadFactory->CreateThread( new CRenameAssetRunnable( this, selectedAssets[0] ), CString::Format( TEXT( "RenameAsset_%s" ), selectedAssets[0].GetName().c_str() ).c_str(), true, true );
		}

		// Copy reference to asset
		if ( ImGui::MenuItem( "Copy Reference", "", nullptr, bSelectedOnlyOneAsset ) )
		{
			PopupMenu_Asset_CopyReference( selectedAssets[0] );
		}
		
		ImGui::EndPopup();
	}
}

void CContentBrowserWindow::DrawPackagesPopupMenu()
{
	if ( ImGui::BeginPopupContextWindow( "", ImGuiMouseButton_Right ) )
	{
		// Getting all selected nodes
		std::vector<TSharedPtr<CFileTreeNode>>		selectedNode;
		engineRoot->GetSelectedNodes( selectedNode );
		gameRoot->GetSelectedNodes( selectedNode );

		// Check what type of nodes we selected
		bool			bExistLoadedPackage		= false;
		bool			bExistUnloadedPackage	= false;
		uint32			numSelectedFolders		= 0;
		uint32			numSelectedFiles		= 0;
		for ( uint32 index = 0, count = selectedNode.size(); index < count; ++index )
		{
			switch ( selectedNode[index]->GetType() )
			{
			case FNT_File:
				++numSelectedFiles;
				if ( !GPackageManager->IsPackageLoaded( selectedNode[index]->GetPath() ) )
				{
					bExistUnloadedPackage = true;
				}
				else
				{
					bExistLoadedPackage = true;
				}
				break;

			case FNT_Folder:
				++numSelectedFolders;
				break;

			default:
				checkMsg( false, TEXT( "Unknown type 0x%X" ), selectedNode[index]->GetType() );
				break;
			}
		}

		bool			bSelectedFolders		= numSelectedFolders > 0;
		bool			bSelectedFiles			= numSelectedFiles > 0;
		bool			bSelectedMoreOneItems	= selectedNode.size() > 1;

		// Save all packages
		if ( ImGui::MenuItem( "Save", "", nullptr, bSelectedFiles && bExistLoadedPackage ) )
		{
			PopupMenu_Package_Save( selectedNode );
		}

		// Open packages
		if ( ImGui::MenuItem( "Open", "", nullptr, bSelectedFiles && bExistUnloadedPackage ) )
		{
			PopupMenu_Package_Open( selectedNode );
		}

		// Unload packages
		if ( ImGui::MenuItem( "Unload", "", nullptr, bSelectedFiles && bExistLoadedPackage ) )
		{
			PopupMenu_Package_Unload( selectedNode );
		}

		// Reload packages
		if ( ImGui::MenuItem( "Reload", "", nullptr, bSelectedFiles && bExistLoadedPackage ) )
		{
			PopupMenu_Package_Reload( selectedNode );
		}
		ImGui::Separator();
		
		// Show in explorer
		if ( ImGui::MenuItem( "Show In Explorer", "", nullptr, ( bSelectedFiles || bSelectedFolders ) && !bSelectedMoreOneItems ) )
		{
			PopupMenu_Package_ShowInExplorer( selectedNode );
		}

		ImGui::Separator();
		if ( ImGui::BeginMenu( "Create" ) )
		{
			// Create a folder
			if ( ImGui::MenuItem( "Folder" ) && hoveredNode )
			{
				GThreadFactory->CreateThread( new CCreateFileRunnable( this, hoveredNode, CCreateFileRunnable::CM_CreateFolder ), TEXT( "CreateDirectoryThread" ), true, true );
			}

			// Create a package
			if ( ImGui::MenuItem( "Package" ) && hoveredNode )
			{
				GThreadFactory->CreateThread( new CCreateFileRunnable( this, hoveredNode, CCreateFileRunnable::CM_CreatePackage ), TEXT( "CreatePackageThread" ), true, true );
			}
			ImGui::EndMenu();
		}

		// Delete packages
		if ( ImGui::MenuItem( "Delete", "", nullptr, bSelectedFiles || bSelectedFolders ) )
		{
			// Convert selected nodes to array of filenames for popup
			std::vector<CFilename>		selectedFiles;
			for ( uint32 index = 0, count = selectedNode.size(); index < count; ++index )
			{
				selectedFiles.push_back( selectedNode[index]->GetPath() );
			}

			TSharedPtr<CDialogWindow>		popup = OpenPopup<CDialogWindow>( TEXT( "Question" ), CString::Format( TEXT( "Is need delete selected files?\n\nFiles:\n%s" ), ArrayFilenamesToString( selectedFiles ).c_str() ), CDialogWindow::BT_Ok | CDialogWindow::BT_Cancel );
			popup->OnButtonPressed().Add( [&]( CDialogWindow::EButtonType InButtonType ) 
										{ 
											// We pressed 'Ok', delete selected folders
											if ( InButtonType == CDialogWindow::BT_Ok )
											{
												PopupMenu_Package_Delete();
											}
										} );
		}

		// Rename file or folder
		if ( ImGui::MenuItem( "Rename", "", nullptr, ( bSelectedFiles || bSelectedFolders ) && !bSelectedMoreOneItems ) )
		{
			TSharedPtr<CFileTreeNode>	node			= selectedNode[0];
			if ( package )
			{
				CFilename	filenamePackage = package->GetFileName();
				CFilename	filenameNode	= node->GetPath();
				bool		bDirectory		= node->GetType() == FNT_Folder;
				if ( ( bDirectory && filenamePackage.IsInDirectory( filenameNode.GetFullPath() ) ) ||
					 ( !bDirectory && filenameNode.GetFullPath() == filenamePackage.GetFullPath() ) )
				{
					SetCurrentPackage( nullptr );
				}
			}

			GThreadFactory->CreateThread( new CRenameFileRunnable( this, node, node->GetType() == FNT_Folder ? CRenameFileRunnable::RM_RenameFolder : CRenameFileRunnable::RM_RenamePackage ), TEXT( "RenameFileThread" ), true, true );
		}
		ImGui::EndPopup();
	}
}

void CContentBrowserWindow::PopupMenu_Package_Save( const std::vector<TSharedPtr<CFileTreeNode>>& InSelectedNodes )
{
	for ( uint32 index = 0, count = InSelectedNodes.size(); index < count; ++index )
	{
		const TSharedPtr<CFileTreeNode>&		node = InSelectedNodes[index];
		std::wstring							path = node->GetPath();

		// If package is loaded - we resave him
		if ( GPackageManager->IsPackageLoaded( path ) )
		{
			PackageRef_t		package = GPackageManager->LoadPackage( path );
			if ( package )
			{
				package->Save( path );
			}
		}
	}
}

void CContentBrowserWindow::PopupMenu_Package_Open( const std::vector<TSharedPtr<CFileTreeNode>>& InSelectedNodes )
{
	PackageRef_t	lastLoadedPackage;
	for ( uint32 index = 0, count = InSelectedNodes.size(); index < count; ++index )
	{
		const TSharedPtr<CFileTreeNode>&	node = InSelectedNodes[index];
		std::wstring						path = node->GetPath();
		PackageRef_t						package = GPackageManager->LoadPackage( path );
		if ( package )
		{
			lastLoadedPackage = package;
		}
	}

	// Set last loaded package for view in package browser
	if ( lastLoadedPackage )
	{
		SetCurrentPackage( lastLoadedPackage );
	}
}

void CContentBrowserWindow::PopupMenu_Package_Unload( const std::vector<TSharedPtr<CFileTreeNode>>& InSelectedNodes )
{
	PackageRef_t				currentPackage = package;
	SetCurrentPackage( nullptr );

	std::vector<CFilename>		dirtyPackages;
	for ( uint32 index = 0, count = InSelectedNodes.size(); index < count; ++index )
	{
		const TSharedPtr<CFileTreeNode>&	node = InSelectedNodes[index];
		std::wstring						path = node->GetPath();

		// If package already not loaded - skip him
		if ( !GPackageManager->IsPackageLoaded( path ) )
		{
			continue;
		}

		// If package is dirty - we not unload him
		PackageRef_t		package = GPackageManager->LoadPackage( path );
		if ( package->IsDirty() )
		{
			dirtyPackages.push_back( CFilename( path ) );
			continue;
		}

		// If current package in viewer is closed, we forget about him
		bool	bSeccussed = GPackageManager->UnloadPackage( path );
		if ( currentPackage && bSeccussed && path == currentPackage->GetFileName() )
		{
			currentPackage.SafeRelease();
		}
	}

	// If current package not closed, we restore viewer
	if ( currentPackage )
	{
		SetCurrentPackage( currentPackage );
	}

	// If we have dirty package - print message
	if ( !dirtyPackages.empty() )
	{
		OpenPopup<CDialogWindow>( TEXT( "Warning" ), CString::Format( TEXT( "The following packages have been modified and cannot be unloaded. Saving these packages will allow them to be unloaded\n\nPackages:\n%s\n" ), ArrayFilenamesToString( dirtyPackages ).c_str() ), CDialogWindow::BT_Ok );
	}
}

void CContentBrowserWindow::PopupMenu_Package_Reload( const std::vector<TSharedPtr<CFileTreeNode>>& InSelectedNodes )
{
	for ( uint32 index = 0, count = InSelectedNodes.size(); index < count; ++index )
	{
		GPackageManager->ReloadPackage( InSelectedNodes[index]->GetPath() );
	}
}

void CContentBrowserWindow::PopupMenu_Package_Delete()
{
	bool	bDirtyTOC = false;

	// Getting list files to delete
	std::vector<CFilename>							usedPackages;
	std::vector<std::wstring>						filesToDelete;
	std::vector<std::wstring>						dirsToDelete;
	{
		std::vector<TSharedPtr<CFileTreeNode>>		selectedNode;
		engineRoot->GetSelectedNodes( selectedNode, true );
		gameRoot->GetSelectedNodes( selectedNode, true );
		
		for ( uint32 index = 0, count = selectedNode.size(); index < count; ++index )
		{
			CFilename		filename = selectedNode[index]->GetPath();
			if ( GFileSystem->IsDirectory( filename.GetFullPath() ) )
			{
				GetFilesInDirectory( filename.GetFullPath(), filesToDelete, dirsToDelete );
				dirsToDelete.push_back( filename.GetFullPath() );
			}
			else
			{
				filesToDelete.push_back( filename.GetFullPath() );
			}
		}
	}

	// Delete all files
	for ( uint32 index = 0, count = filesToDelete.size(); index < count; ++index )
	{
		CFilename		filename = filesToDelete[index];

		// If this package, we remove entry from TOC file
		if ( filename.GetExtension() == TEXT( "pak" ) )
		{
			// We try unload package for remove unused assets.
			// If package still used, we skip him
			if ( !GPackageManager->UnloadPackage( filename.GetFullPath() ) )
			{
				usedPackages.push_back( filename.GetBaseFilename() );
				continue;
			}

			// Else we remove entry from TOC file
			GTableOfContents.RemoveEntry( filename.GetFullPath() );
			bDirtyTOC = true;

			// If this is package and him opened in package browser, we close it
			if ( package && filename.GetFullPath() == CFilename( package->GetFileName() ).GetFullPath() )
			{
				SetCurrentPackage( nullptr );
			}
		}

		GFileSystem->Delete( filename.GetFullPath(), true );
	}

	// Delete all directories if they is empty
	for ( uint32 index = 0, count = dirsToDelete.size(); index < count; ++index )
	{
		const std::wstring&		fullDirPath = dirsToDelete[index];
		if ( IsEmptyDirectory( fullDirPath, true ) )
		{
			GFileSystem->DeleteDirectory( fullDirPath, true );
		}
	}

	// If TOC file is dirty, we serialize him to cache
	if ( bDirtyTOC )
	{
		GEditorEngine->SerializeTOC( true );
	}

	// If we have used package - print message
	if ( !usedPackages.empty() )
	{
		OpenPopup<CDialogWindow>( TEXT( "Warning" ), CString::Format( TEXT( "The following packages in using or modifided and cannot be delete. Close all assets from this package or save him for will allow them to be deleted\n\nPackages:\n%s" ), ArrayFilenamesToString( usedPackages ).c_str() ), CDialogWindow::BT_Ok );
	}
}

void CContentBrowserWindow::PopupMenu_Package_ShowInExplorer( const std::vector<TSharedPtr<CFileTreeNode>>& InSelectedNodes )
{
	check( InSelectedNodes.size() == 1 );

	TSharedPtr<CFileTreeNode>		selectedNode	= InSelectedNodes[0];
	std::wstring					pathToDirectory = selectedNode->GetPath();
	if ( selectedNode->GetType() == FNT_Folder )
	{
		pathToDirectory = CFilename( selectedNode->GetPath() ).GetPath();
	}

	appShowFileInExplorer( pathToDirectory );
}

void CContentBrowserWindow::PopupMenu_Asset_Reload( const std::vector<CAssetNode>& InAssets )
{
	std::wstring		errorMessages;
	for ( uint32 index = 0, count = InAssets.size(); index < count; ++index )
	{
		const SAssetInfo*		assetInfo	= InAssets[index].GetAssetInfo();
		TAssetHandle<CAsset>	asset		= package->Find( assetInfo->name );
		if ( !package->ReloadAsset( asset ) )
		{
			errorMessages += CString::Format( TEXT( "\n%s : Failed reload" ), assetInfo->name.c_str() );
		}
	}

	// If exist errors, show popup window
	if ( !errorMessages.empty() )
	{
		OpenPopup<CDialogWindow>( TEXT( "Error" ), CString::Format( TEXT( "The following assets not reloaded with following errors\n\nAssets:%s" ), errorMessages.c_str() ), CDialogWindow::BT_Ok );
	}
}

void CContentBrowserWindow::PopupMenu_Asset_Import()
{
	CFileDialogSetup		fileDialogSetup;
	SOpenFileDialogResult	openFileDialogResult;

	// Init file dialog settings
	fileDialogSetup.SetMultiselection( true );
	fileDialogSetup.SetTitle( TEXT( "Import Asset" ) );
	fileDialogSetup.SetDirectory( gameRoot->GetPath() );
	for ( uint32 index = AT_FirstType; index < AT_Count; ++index )
	{
		const CAssetFactory::SAssetImporterInfo& importerInfo = GAssetFactory.GetImporterInfo( ( EAssetType )index );
		if ( importerInfo.bValid )
		{
			fileDialogSetup.AddFormat( importerInfo, ConvertAssetTypeToText( ( EAssetType )index ) );
		}
	}

	// Show open file dialog
	if ( appShowOpenFileDialog( fileDialogSetup, openFileDialogResult ) )
	{
		GThreadFactory->CreateThread( new CImportAssetsRunnable( this, openFileDialogResult.files ), TEXT( "ImportAssets" ), true, true );
	}
}

void CContentBrowserWindow::PopupMenu_Asset_Reimport( const std::vector<CAssetNode>& InAssets )
{
	std::wstring		errorMessages;
	for ( uint32 index = 0, count = InAssets.size(); index < count; ++index )
	{
		const SAssetInfo*		assetInfo	= InAssets[index].GetAssetInfo();
		TAssetHandle<CAsset>	asset		= package->Find( assetInfo->name );	
		if ( asset.IsAssetValid() )
		{
			std::wstring		errorMsg;
			TSharedPtr<CAsset>	assetPtr = asset.ToSharedPtr();
			if ( !GAssetFactory.Reimport( assetPtr, errorMsg ) )
			{
				errorMessages += CString::Format( TEXT( "\n%s : %s" ), assetPtr->GetAssetName().c_str(), errorMsg.c_str() );
			}
		}
	}

	// If exist errors, show popup window
	if ( !errorMessages.empty() )
	{
		OpenPopup<CDialogWindow>( TEXT( "Error" ), CString::Format( TEXT( "The following assets not reimported with following errors\n\nAssets:%s" ), errorMessages.c_str() ), CDialogWindow::BT_Ok );
	}
}

void CContentBrowserWindow::PopupMenu_Asset_ReimportWithNewFile( const CAssetNode& InAsset )
{
	std::wstring			errorMessage;
	const SAssetInfo*		assetInfo = InAsset.GetAssetInfo();
	if ( assetInfo->data )
	{
		// Select new source file
		CFileDialogSetup		fileDialogSetup;
		SOpenFileDialogResult	openFileDialogResult;

		// Init file dialog settings
		fileDialogSetup.SetMultiselection( false );
		fileDialogSetup.SetTitle( TEXT( "Reimport Asset" ) );
		fileDialogSetup.SetDirectory( gameRoot->GetPath() );
		fileDialogSetup.AddFormat( GAssetFactory.GetImporterInfo( assetInfo->data->GetType() ), ConvertAssetTypeToText( assetInfo->data->GetType() ) );

		// Show open file dialog
		if ( appShowOpenFileDialog( fileDialogSetup, openFileDialogResult ) && !openFileDialogResult.files.empty() )
		{
			// Let's try to reimport asset
			std::wstring	oldSourceFile = assetInfo->data->GetAssetSourceFile();
			assetInfo->data->SetAssetSourceFile( openFileDialogResult.files[0] );

			// If failed we return old source file
			if ( !GAssetFactory.Reimport( assetInfo->data, errorMessage ) )
			{
				assetInfo->data->SetAssetSourceFile( oldSourceFile );
			}
		}
	}
	else
	{
		errorMessage = CString::Format( TEXT( "Not found asset in package '%s'" ), assetInfo->name.c_str(), package->GetName().c_str() );
	}

	// If exist error, show popup window
	if ( !errorMessage.empty() )
	{
		OpenPopup<CDialogWindow>( TEXT( "Error" ), CString::Format( TEXT( "The asset '%s' not reimported.\n\nError: %s" ), errorMessage.c_str() ), CDialogWindow::BT_Ok );
	}
}

void CContentBrowserWindow::PopupMenu_Asset_Delete()
{
	// Remove all selected assets from package
	std::wstring		usedAssets;
	for ( uint32 index = 0; index < assets.size(); ++index )
	{
		CAssetNode&	assetNode = assets[index];
		if ( assetNode.IsSelected() )
		{
			const SAssetInfo*	assetInfo = assetNode.GetAssetInfo();
			if ( !package->Remove( assetInfo->name, false, true ) )
			{
				usedAssets += CString::Format( TEXT( "\n%s" ), assetInfo->name.c_str() );
			}
			else
			{
				assetNode.MarkIsDeleted();
			}
		}
	}

	// If we have used assets - print message
	if ( !usedAssets.empty() )
	{
		OpenPopup<CDialogWindow>( TEXT( "Warning" ), CString::Format( TEXT( "The following assets in using and cannot be delete. Close all assets will allow them to be deleted\n\nAssets:%s" ), usedAssets.c_str() ), CDialogWindow::BT_Ok );
	}
}

void CContentBrowserWindow::PopupMenu_Asset_CopyReference( const CAssetNode& InAsset )
{
	check( package );
	const SAssetInfo*	assetInfo = InAsset.GetAssetInfo();
	std::wstring		assetReference;

	if ( MakeReferenceToAsset( package->GetName(), assetInfo->name, assetInfo->type, assetReference ) )
	{
		ImGui::SetClipboardText( TCHAR_TO_ANSI( assetReference.c_str() ) );
	}
}

std::string CContentBrowserWindow::SFilterInfo::GetPreviewFilterAssetType() const
{
	if ( IsShowAllAssetTypes() )
	{
		return "All";
	}

	std::wstring	result;
	for ( uint32 index = AT_FirstType; index < AT_Count; ++index )
	{
		if ( assetTypes[index - 1] )
		{
			result += CString::Format( TEXT( "%s%s" ), result.empty() ? TEXT( "" ) : TEXT( ", " ), ConvertAssetTypeToText( ( EAssetType )index ).c_str() );
		}
	}

	return TCHAR_TO_ANSI( result.c_str() );
}

CContentBrowserWindow::CFileTreeNode::CFileTreeNode( EFileNodeType InType, const std::wstring& InName, const std::wstring& InPath, CContentBrowserWindow* InOwner )
	: bAllowDropTarget( true )
	, bFreshed( false )
	, bSelected( false )
	, bDragging( false )
	, type( InType )
	, path( InPath )
	, name( InName )
	, owner( InOwner )
{
	check( owner );
}

void CContentBrowserWindow::CFileTreeNode::Tick()
{
	// Reset freshed flag in node and allow drop target
	bFreshed = false;

	// Refresh info about file system
	Refresh();

	// If after refreshing of the node flag is staying in FALSE - this is folder/file in FS not exist already
	if ( !bFreshed )
	{
		RemoveFromParent();
		return;
	}

	// Draw interface
	switch ( type )
	{
		// Draw folder node
	case FNT_Folder:
	{
		// Set style for selected node
		bool	bNeedPopStyleColor = false;
		if ( bSelected )
		{
			bNeedPopStyleColor = true;
			ImGui::PushStyleColor( ImGuiCol_Header,			CONTENTBROWSER_SELECTCOLOR );
			ImGui::PushStyleColor( ImGuiCol_HeaderActive,	CONTENTBROWSER_SELECTCOLOR );
			ImGui::PushStyleColor( ImGuiCol_HeaderHovered,	CONTENTBROWSER_SELECTCOLOR );
		}
		
		bool bTreeNode = ImGui::TreeNodeEx( TCHAR_TO_ANSI( name.c_str() ), ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_OpenOnArrow  | ImGuiTreeNodeFlags_OpenOnDoubleClick );

		// Drag n drop handle
		DragNDropHandle();

		// Item event handles
		ProcessEvents();

		// Pop selection node style
		if ( bNeedPopStyleColor )
		{
			ImGui::PopStyleColor( 3 );
		}

		// Draw tree
		if ( bTreeNode )
		{
			std::vector<TSharedPtr<CFileTreeNode>>		childrenOnTick = children;
			for ( uint32 index = 0, count = childrenOnTick.size(); index < count; ++index )
			{
				childrenOnTick[index]->Tick();
			}
			ImGui::TreePop();
		}
		break;
	}

		// Draw file node
	case FNT_File:
	{
		// Set style for selected node
		bool	bNeedPopStyleColor = false;
		if ( bSelected )
		{
			bNeedPopStyleColor = true;
			ImGui::PushStyleColor( ImGuiCol_Button,			CONTENTBROWSER_SELECTCOLOR );
			ImGui::PushStyleColor( ImGuiCol_ButtonHovered,	CONTENTBROWSER_SELECTCOLOR );
			ImGui::PushStyleColor( ImGuiCol_ButtonActive,	CONTENTBROWSER_SELECTCOLOR );
		}

		// If package loaded we check is changed in memory. In successed case mark this package by star
		std::wstring		packageName = name;
		if ( GPackageManager->IsPackageLoaded( path ) )
		{
			PackageRef_t	package = GPackageManager->LoadPackage( path );
			check( package );
			packageName = ( package->IsDirty() ? TEXT( "*" ) : TEXT( "" ) ) + packageName;
		}
		ImGui::Button( TCHAR_TO_ANSI( packageName.c_str() ) );

		// Drag n drop handle
		DragNDropHandle();

		// Item event handles
		ProcessEvents();

		// Pop style color
		if ( bNeedPopStyleColor )
		{
			ImGui::PopStyleColor( 3 );
		}
		break;
	}
	}
}

void CContentBrowserWindow::CFileTreeNode::ProcessEvents()
{
	const bool		bCtrlDown = GInputSystem->IsKeyDown( BC_KeyLControl ) || GInputSystem->IsKeyDown( BC_KeyRControl );
	if ( ImGui::IsItemHovered() )
	{
		// Select node if we press left mouse button
		if ( ImGui::IsMouseReleased( ImGuiMouseButton_Left ) )
		{		
			if ( !bCtrlDown )
			{
				owner->engineRoot->SetSelect( false );
				owner->gameRoot->SetSelect( false );
			}

			switch ( type )
			{
			case FNT_Folder:
				SetSelect( !bSelected && bCtrlDown );
				break;

			case FNT_File:
				SetSelect( !bSelected );
				break;
			}
		}
		// Select node for popup menu if we press right mouse button
		else if ( ImGui::IsMouseDown( ImGuiMouseButton_Right ) )
		{
			if ( !bSelected )
			{
				owner->engineRoot->SetSelect( false );
				owner->gameRoot->SetSelect( false );
				SetSelect( true, false );
			}				
		}

		// If we double clicked by left mouse button, we must open package (if node is file)
		if ( type == FNT_File && !GInputSystem->IsKeyDown( BC_KeyLControl ) && !GInputSystem->IsKeyDown( BC_KeyRControl ) && ImGui::IsMouseDoubleClicked( ImGuiMouseButton_Left ) )
		{
			owner->SetCurrentPackage( GPackageManager->LoadPackage( path ) );
		}

		owner->hoveredNode = AsShared();
	}

	// Reset selection if clicked not on mode
	if ( ImGui::IsWindowHovered() && !ImGui::IsAnyItemHovered() && ( ImGui::IsMouseClicked( ImGuiMouseButton_Left ) || ImGui::IsMouseDown( ImGuiMouseButton_Right ) ) )
	{
		owner->SetCurrentPackage( nullptr );
		owner->engineRoot->SetSelect( false );
		owner->gameRoot->SetSelect( false );
	}
}

void CContentBrowserWindow::CFileTreeNode::DragNDropHandle()
{
	// Begin drag n drop folder/package to other, if current node is not engine/game root and DND not started
	if ( !GImGui->DragDropActive && owner->engineRoot != this && owner->gameRoot != this && ImGui::BeginDragDropSource() )
	{
		// Update selection flags
		if ( !bSelected )
		{
			owner->engineRoot->SetSelect( false );
			owner->gameRoot->SetSelect( false );
			SetSelect( true );
		}

		// Get all selected nodes
		std::vector<CFileTreeNode*>			selectedNodes;
		owner->engineRoot->GetNodesToDragNDrop( selectedNodes );
		owner->gameRoot->GetNodesToDragNDrop( selectedNodes );
		if ( !selectedNodes.empty() )
		{
			selectedNodes.push_back( nullptr );		// Terminate element
			ImGui::SetDragDropPayload( CONTENTBROWSER_DND_FILENODETYPE, &selectedNodes[0], sizeof( CFileTreeNode* ) * selectedNodes.size(), ImGuiCond_Once );

			// Block drop targets to children and parent
			switch ( GetType() )
			{
			case FNT_File:
				for ( uint32 index = 0, count = children.size(); index < count; ++index )
				{
					children[index]->SetAllowDropTarget( false );
				}
				break;

			case FNT_Folder:
			{
				TSharedPtr<CFileTreeNode>	parentPtr = parent.Pin();
				if ( parentPtr )
				{
					parentPtr->SetAllowDropTarget( false );
				}
				break;
			}

			default:
				appErrorf( TEXT( "Unknown node type 0x%X" ), GetType() );
				break;
			}
		}

		MarkDragging( true );		// We start dragging this node
		ImGui::EndDragDropSource();
	}

	// Drag n drop target for packages/folders
	if ( bAllowDropTarget && ImGui::BeginDragDropTarget() )
	{
		const ImGuiPayload*		imguiPayload = ImGui::AcceptDragDropPayload( CONTENTBROWSER_DND_FILENODETYPE );
		if ( imguiPayload )
		{
			// Prepare data to copy/move
			PackageRef_t		currentPackage	= owner->GetCurrentPackage();
			CFileTreeNode**		pData			= ( CFileTreeNode** )imguiPayload->Data;
			check( pData );

			// Get destination folder
			CFilename		filenamePackage = currentPackage ? currentPackage->GetFileName() : TEXT( "" );
			CFilename		nodeFilename	= path;
			std::wstring	dstDirectory	= nodeFilename.GetPath() + PATH_SEPARATOR;
			if ( type == FNT_Folder )
			{
				dstDirectory += nodeFilename.GetBaseFilename() + PATH_SEPARATOR;
			}

			// Make array of files to copy/move
			std::vector<CMoveCopyFilesRunnable::SFileInfo>		filesToMoveCopy;
			bool												bNeedResetCurrentPackage = false;
			for ( uint32 index = 0; pData[index]; ++index )
			{
				CMoveCopyFilesRunnable::SFileInfo	fileInfo;
				fileInfo.bDirectory					= pData[index]->GetType() == FNT_Folder;
				fileInfo.srcPath					= pData[index]->path;
				fileInfo.dstPath					= dstDirectory + fileInfo.srcPath.GetFilename();
				if ( !bNeedResetCurrentPackage && currentPackage )
				{
					if ( ( fileInfo.bDirectory && filenamePackage.IsInDirectory( fileInfo.srcPath.GetFullPath() ) ) ||
						 ( !fileInfo.bDirectory && fileInfo.srcPath.GetFullPath() == filenamePackage.GetFullPath() ) )
					{
						bNeedResetCurrentPackage = true;
					}
				}

				filesToMoveCopy.push_back( fileInfo );
				pData[index]->MarkDragging( false );
			}

			// Reset current package
			if ( bNeedResetCurrentPackage )
			{
				owner->SetCurrentPackage( nullptr );
			}

			// Create and start thread for copy/move files
			GThreadFactory->CreateThread( new CMoveCopyFilesRunnable( owner, filesToMoveCopy ), TEXT( "MoveCopyFilesThread" ), true, true );

			// Allow drop targets for all nodes
			owner->engineRoot->SetAllowDropTarget( true );
			owner->gameRoot->SetAllowDropTarget( true );
		}

		ImGui::EndDragDropTarget();
	}

	// If in this node flag 'bDragging' is set, but drag'n'drap already disable - this mean what we droped data in empty space and need restore all nodes
	if ( bDragging && !GImGui->DragDropActive )
	{
		SetSelect( false );
		SetAllowDropTarget( true );
		MarkDragging( false );
	}
}

void CContentBrowserWindow::CFileTreeNode::Refresh()
{
	// If node not exist in file system or need filter him, we mark him not freshed
	if ( ( type == FNT_File && !CString::InString( name, ANSI_TO_TCHAR( owner->filterInfo.fileName.c_str() ), true ) ) || !GFileSystem->IsExistFile( path, type == FNT_Folder ? true : false ) )
	{
		bFreshed = false;
		return;
	}
	
	std::vector<std::wstring>	files			= GFileSystem->FindFiles( path, true, true );
	for ( uint32 index = 0, count = files.size(); index < count; ++index )
	{
		std::wstring					file		= files[index];
		std::size_t						dotPos		= file.find_last_of( TEXT( "." ) );
		std::wstring					fullPath	= path + PATH_SEPARATOR + file;
		TSharedPtr<CFileTreeNode>		childNode;

		// Add child node if this is directory
		if ( dotPos == std::wstring::npos )
		{
			childNode = FindChild( fullPath );
			if ( !childNode )
			{
				childNode			= MakeSharedPtr<CFileTreeNode>( FNT_Folder, file, fullPath, owner );
				childNode->parent	= AsWeak();
				children.push_back( childNode );
			}

			childNode->Refresh();
			continue;
		}

		// If this package file we add new child node
		std::wstring		extension = file;
		extension.erase( 0, dotPos + 1 );
		if ( extension == TEXT( "pak" ) )
		{
			childNode = FindChild( fullPath );
			if ( !childNode )
			{
				childNode			= MakeSharedPtr<CFileTreeNode>( FNT_File, file, fullPath, owner );
				childNode->parent	= AsWeak();
				children.push_back( childNode );
			}

			childNode->bFreshed = true;
		}
	}

	bFreshed = true;
}

void CContentBrowserWindow::CFileTreeNode::GetSelectedNodes( std::vector<TSharedPtr<CFileTreeNode>>& OutSelectedNodes, bool InIsIgnoreChildren /* = false */ ) const
{
	if ( bSelected )
	{
		OutSelectedNodes.push_back( AsShared() );
	}

	if ( !bSelected || !InIsIgnoreChildren )
	{
		for ( uint32 index = 0, count = children.size(); index < count; ++index )
		{
			children[index]->GetSelectedNodes( OutSelectedNodes, InIsIgnoreChildren );
		}
	}
}

void CContentBrowserWindow::CFileTreeNode::GetNodesToDragNDrop( std::vector<CFileTreeNode*>& OutSelectedNodes ) const
{
	if ( bSelected )
	{
		OutSelectedNodes.push_back( const_cast<CFileTreeNode*>( this ) );
	}
	else
	{
		for ( uint32 index = 0, count = children.size(); index < count; ++index )
		{
			children[index]->GetNodesToDragNDrop( OutSelectedNodes );
		}
	}
}

void CContentBrowserWindow::CAssetNode::Tick()
{
	check( info );
	ImGui::PushStyleVar( ImGuiStyleVar_FrameBorderSize, CONTENTBROWSER_ASSET_BORDERSIZE );
	ImGui::PushStyleColor( ImGuiCol_Border, GAssetBorderColors[info->type] );

	// If asset is selected, we set him button color to CONTENTBROWSER_SELECTCOLOR
	bool		bNeedPopStyleColor = false;
	if ( bSelected )
	{
		bNeedPopStyleColor = true;
		ImGui::PushStyleColor( ImGuiCol_Button, CONTENTBROWSER_SELECTCOLOR );
		ImGui::PushStyleColor( ImGuiCol_ButtonHovered, CONTENTBROWSER_SELECTCOLOR );
		ImGui::PushStyleColor( ImGuiCol_ButtonActive, CONTENTBROWSER_SELECTCOLOR );
	}

	// Draw of the asset's button, if him's icon isn't existing we draw simple button
	const TAssetHandle<CTexture2D>&		assetIconTexture	= owner->assetIcons[info->type];
	if ( assetIconTexture.IsAssetValid() )
	{
		ImGui::PushID( TCHAR_TO_ANSI( info->name.c_str() ) );
		ImGui::ImageButton( GImGUIEngine->LockTexture( assetIconTexture.ToSharedPtr()->GetTexture2DRHI() ), { owner->thumbnailSize, owner->thumbnailSize } );
	}
	else
	{
		ImGui::Button( TCHAR_TO_ANSI( info->name.c_str() ), { owner->thumbnailSize, owner->thumbnailSize } );
	}

	// Drag n drop handle
	DragNDropHandle();

	// Process events
	ProcessEvents();

	// If asset loaded we check is changed in memory. In successed case mark this asset by star
	std::wstring		assetName = info->name;
	if ( info->data && info->data->IsDirty() )
	{
		assetName = TEXT( "*" ) + assetName;
	}
	ImGui::TextWrapped( TCHAR_TO_ANSI( assetName.c_str() ) );

	if ( assetIconTexture.IsAssetValid() )
	{
		ImGui::PopID();
	}
	ImGui::PopStyleVar();
	ImGui::PopStyleColor( !bNeedPopStyleColor ? 1 : 4 );
}

void CContentBrowserWindow::CAssetNode::DragNDropHandle()
{
	// Begin drag n drop asset references
	if ( !GImGui->DragDropActive && ImGui::BeginDragDropSource() )
	{
		// Update selection flags
		if ( !bSelected )
		{
			// Unselect all asset
			for ( uint32 index = 0, count = owner->assets.size(); index < count; ++index )
			{
				owner->assets[index].SetSelect( false );
			}

			// Select only that asset
			SetSelect( true );
		}

		PackageRef_t		package = owner->GetCurrentPackage();
		check( package );

		// Generate string with all asset references (only selected)
		std::vector<std::wstring>	assetReferences;
		for ( uint32 index = 0, count = owner->assets.size(); index < count; ++index )
		{
			const CAssetNode&		assetNode = owner->assets[index];
			if ( assetNode.IsSelected() )
			{
				const SAssetInfo*	assetInfo = owner->assets[index].GetAssetInfo();
				check( assetInfo );

				// Make reference to asset
				std::wstring		resultRef;
				if ( MakeReferenceToAsset( package->GetName(), assetInfo->name, assetInfo->type, resultRef ) )
				{
					assetReferences.push_back( resultRef );
				}
			}
		}

		// Make dragging data if array of asset references not empty, then we copy him to ImGUI
		if ( !assetReferences.empty() )
		{
			std::vector<tchar>		draggingData;
			DND_MakeAssetReferenceData( assetReferences, draggingData );
			ImGui::SetDragDropPayload( DND_ASSETREFERENCEETYPE, &draggingData[0], sizeof( tchar ) * draggingData.size(), ImGuiCond_Once );
		}

		ImGui::EndDragDropSource();
	}
}

void CContentBrowserWindow::CAssetNode::ProcessEvents()
{
	const bool		bCtrlDown = GInputSystem->IsKeyDown( BC_KeyLControl ) || GInputSystem->IsKeyDown( BC_KeyRControl );
	if ( ImGui::IsItemHovered() )
	{
		// Select asset if we press left mouse button
		if ( ImGui::IsMouseReleased( ImGuiMouseButton_Left ) )
		{
			if ( !bCtrlDown )
			{
				for ( uint32 index = 0, count = owner->assets.size(); index < count; ++index )
				{
					owner->assets[index].SetSelect( false );
				}
			}

			SetSelect( !bSelected );
		}
		// Select asset for popup menu if we press right mouse button
		else if ( ImGui::IsMouseDown( ImGuiMouseButton_Right ) )
		{
			if ( !bSelected )
			{
				for ( uint32 index = 0, count = owner->assets.size(); index < count; ++index )
				{
					owner->assets[index].SetSelect( false );
				}
				SetSelect( true );
			}
		}

		// If we double clicked by left mouse button, we must open editor for this type asset
		if ( ImGui::IsMouseDoubleClicked( ImGuiMouseButton_Left ) )
		{
			// Get asset
			TSharedPtr<CAsset>	asset;
			if ( info->data )
			{
				asset = info->data;
			}
			else
			{
				asset = owner->package->Find( info->name ).ToSharedPtr();
			}	

			// Open editor window
			check( asset );
			switch ( info->type )
			{
			case AT_Texture2D:	MakeSharedPtr<CTextureEditorWindow>( asset )->Init();		break;
			case AT_Material:	MakeSharedPtr<CMaterialEditorWindow>( asset )->Init();		break;
			case AT_StaticMesh:	MakeSharedPtr<CStaticMeshEditorWindow>( asset )->Init();	break;
			default:
				appErrorf( TEXT( "Unsupported asset type 0x%X" ), info->type );
				break;
			}
		}
	}

	// If pressed Ctrl+A we select all assets in the current package
	if ( ImGui::IsWindowHovered() && ( bCtrlDown && GInputSystem->IsKeyDown( BC_KeyA ) ) )
	{
		for ( uint32 index = 0, count = owner->assets.size(); index < count; ++index )
		{
			owner->assets[index].SetSelect( true );
		}
	}
}