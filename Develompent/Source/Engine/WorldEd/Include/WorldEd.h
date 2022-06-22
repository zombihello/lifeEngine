/**
 * @file
 * @addtogroup WorldEd World editor
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef WORLDED_H
#define WORLDED_H

#include <string>
#include <qdir.h>
#include <qlabel.h>
#include <qstring.h>
#include <qmessagebox.h>

#include "Misc/Types.h"
#include "Misc/SharedPointer.h"
#include "Actors/Actor.h"
#include "System/Delegate.h"

/**
 * @ingroup WorldEd
 * Main function for start WorldEd
 * 
 * @param[in] InCmdLine Arguments of start
 * @return Returns the error level, 0 if successful and > 0 if there were errors
 */
int32 appWorldEdEntry( const tchar* InCmdLine );

/**
 * @ingroup WorldEd
 * Get WorldEd name
 * 
 * @return Return WorldEd name
 */
std::wstring appGetWorldEdName();

/**
 * @ingroup WorldEd
 * Get engine path from Qt absolute path
 * 
 * @param InPath Absolute path
 * @return Return engine path
 */
FORCEINLINE std::wstring appQtAbsolutePathToEngine( const QString& InPath )
{
	static QDir			baseDir( "./" );
	return baseDir.relativeFilePath( InPath ).toStdWString();
}

/**
 * @ingroup WorldEd
 * Show message box with list
 * 
 * @param InParent		Parent window
 * @param InTitle		Title message box
 * @param InText		Message
 * @param InListName	List name
 * @param InList		List
 * @param InIsError		Is error message
 * @param InMaxSizeList	Max show items in list
 */
QMessageBox::StandardButton ShowMessageBoxWithList( class QWidget* InParent, const QString& InTitle, const QString& InText, const QString& InListName, const std::vector< QString >& InList, bool InIsError = false, uint32 InMaxSizeList = 3 );

/**
 * @ingroup WorldEd
 * Helper struct for the COnAssetsCanDelete delegate
 */
struct SCanDeleteAssetResult
{
public:
	SCanDeleteAssetResult( const SCanDeleteAssetResult& )	= delete;
	SCanDeleteAssetResult( SCanDeleteAssetResult&& )		= delete;

	/**
	 * Constructor
	 */
	SCanDeleteAssetResult() : 
		bResult( true ) 
	{}

	/**
	 * Set value
	 * @param InValue	Value
	 */
	FORCEINLINE void Set( bool InValue )
	{ 
		bResult &= InValue; 
	}

	/**
	 * Get value
	 * @return Return result
	 */
	FORCEINLINE bool Get() const 
	{ 
		return bResult; 
	}

private:
	bool	bResult;	/**< Is can delete assets */
};

/**
 * @ingroup WorldEd
 * @brief Delegates used by the editor
 */
struct SEditorDelegates
{
	/**
	 * @brief Delegate for called event when assets try delete
	 */
	DECLARE_MULTICAST_DELEGATE( COnAssetsCanDelete, const std::vector< TSharedPtr<class CAsset> >& /*InAssets*/, SCanDeleteAssetResult& /*OutResult*/ );

	/**
	 * @brief Delegate for called event when assets deleted
	 */
	DECLARE_MULTICAST_DELEGATE( COnAssetsDeleted, const std::vector< TSharedPtr<class CAsset> >& /*InAssets*/ );

	/**
	 * @brief Delegate for called event when assets is reloaded
	 */
	DECLARE_MULTICAST_DELEGATE( COnAssetsReloaded, const std::vector< TSharedPtr<class CAsset> >& /*InAssets*/ );

	/**
	 * @brief Delegate for called event when actor spawned
	 */
	DECLARE_MULTICAST_DELEGATE( COnActorsSpawned, const std::vector<ActorRef_t>& /*InActors*/ );

	/**
	 * @brief Delegate for called event when actor destroyed
	 */
	DECLARE_MULTICAST_DELEGATE( COnActorsDestroyed, const std::vector<ActorRef_t>& /*InActors*/ );

	static COnAssetsCanDelete		onAssetsCanDelete;		/**< Called when one or more assets try delete */
	static COnAssetsDeleted			onAssetsDeleted;		/**< Called when one or more assets have been deleted */
	static COnAssetsReloaded		onAssetsReloaded;		/**< Called when one or more assets is reloaded */
	static COnActorsSpawned			onActorsSpawned;		/**< Called when actors spawned */
	static COnActorsDestroyed		onActorsDestroyed;		/**< Called when actors destroyed */
};

#endif // !WORLDED_H