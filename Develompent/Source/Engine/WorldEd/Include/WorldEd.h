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

#include "LEBuild.h"
#include "Misc/Types.h"
#include "Misc/SharedPointer.h"
#include "Actors/Actor.h"
#include "System/Delegate.h"
#include "System/EditorModes.h"

/**
 * @ingroup WorldEd
 * Get WorldEd name
 * 
 * @return Return WorldEd name
 */
std::wstring Sys_GetWorldEdName();

/**
 * @ingroup WorldEd
 * @note Need implement on each platform
 * @brief Show file in file explorer
 * 
 * @param InPath	Path to file
 */
void Sys_ShowFileInExplorer( const std::wstring& InPath );

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

	/**
	 * @brief Delegate for called event when changed editor mode
	 */
	DECLARE_MULTICAST_DELEGATE( COnEditorModeChanged, EEditorMode /*InEditorMode*/ );

	/**
	 * @brief Delegate for called event when loaded map
	 */
	DECLARE_MULTICAST_DELEGATE( COnEditorLoadedMap );

	/**
	 * @brief Delegate for called event when saved map
	 */
	DECLARE_MULTICAST_DELEGATE( COnEditorSavedMap );

	/**
	 * @brief Delegate for called event when actors selected
	 */
	DECLARE_MULTICAST_DELEGATE( COnActorsSelected, const std::vector<ActorRef_t>& /*InActors*/ );

	/**
	 * @brief Delegate for called event when actors unselected
	 */
	DECLARE_MULTICAST_DELEGATE( COnActorsUnselected, const std::vector<ActorRef_t>& /*InActors*/ );

	/**
	 * @brief Delegate for called event when map was marked dirty
	 */
	DECLARE_MULTICAST_DELEGATE( COnEditorMapMarkedDirty );

	/**
	 * @brief Delegate for called event when created new map
	 */
	DECLARE_MULTICAST_DELEGATE( COnEditorCreatedNewMap );

	static COnAssetsCanDelete		onAssetsCanDelete;			/**< Called when one or more assets try delete */
	static COnAssetsDeleted			onAssetsDeleted;			/**< Called when one or more assets have been deleted */
	static COnAssetsReloaded		onAssetsReloaded;			/**< Called when one or more assets is reloaded */
	static COnActorsSpawned			onActorsSpawned;			/**< Called when actors spawned */
	static COnActorsDestroyed		onActorsDestroyed;			/**< Called when actors destroyed */
	static COnEditorModeChanged		onEditorModeChanged;		/**< Called when editor mode is changed */
	static COnEditorCreatedNewMap	onEditorCreatedNewMap;		/**< Called when map created new map */
	static COnEditorLoadedMap		onEditorLoadedMap;			/**< Called when editor loaded map */
	static COnEditorSavedMap		onEditorSavedMap;			/**< Called when editor saved map */
	static COnActorsSelected		onActorsSelected;			/**< Called when actors selected */
	static COnActorsUnselected		onActorsUnselected;			/**< Called when actors unselected */
	static COnEditorMapMarkedDirty	onEditorMapMarkedDirty;		/**< Called when map was marked dirty */
};

#if !WITH_IMGUI
	#error For work editor need ImGui
#endif // !WITH_IMGUI

#endif // !WORLDED_H