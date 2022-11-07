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
std::wstring appGetWorldEdName();

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
	 * @brief Delegate for called event when actors selected
	 */
	DECLARE_MULTICAST_DELEGATE( COnActorsSelected, const std::vector<ActorRef_t>& /*InActors*/ );

	/**
	 * @brief Delegate for called event when actors unselected
	 */
	DECLARE_MULTICAST_DELEGATE( COnActorsUnselected, const std::vector<ActorRef_t>& /*InActors*/ );

	static COnAssetsCanDelete		onAssetsCanDelete;		/**< Called when one or more assets try delete */
	static COnAssetsDeleted			onAssetsDeleted;		/**< Called when one or more assets have been deleted */
	static COnAssetsReloaded		onAssetsReloaded;		/**< Called when one or more assets is reloaded */
	static COnActorsSpawned			onActorsSpawned;		/**< Called when actors spawned */
	static COnActorsDestroyed		onActorsDestroyed;		/**< Called when actors destroyed */
	static COnEditorModeChanged		onEditorModeChanged;	/**< Called when editor mode is changed */
	static COnEditorLoadedMap		onEditorLoadedMap;		/**< Called when editor loaded map */
	static COnActorsSelected		onActorsSelected;		/**< Called when actors slected */
	static COnActorsUnselected		onActorsUnselected;		/**< Called when actors unselected */
};

#if !WITH_IMGUI
	#error For work editor need ImGui
#endif // !WITH_IMGUI

#endif // !WORLDED_H