/**
 * @file
 * @addtogroup Editor Editor
 *
 * ************************************************************
 *                  This file is part of:
 *                      LIFEENGINE
 *          https://github.com/zombihello/lifeEngine
 * ************************************************************
 * Copyright (C) 2024 Yehor Pohuliaka.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef EDITOR_H
#define EDITOR_H

#include <string>

#include "Core/LEBuild.h"
#include "Core/Misc/Types.h"
#include "Core/Misc/SharedPointer.h"
#include "Core/System/Delegate.h"
#include "Engine/Actors/Actor.h"
#include "Editor/System/EditorModes.h"

/**
 * @ingroup Editor
 * @note Need implement on each platform
 * @brief Show file in file explorer
 * 
 * @param InPath	Path to file
 */
void Sys_ShowFileInExplorer( const std::wstring& InPath );

/**
 * @ingroup Editor
 * @brief Helper struct for the COnAssetsCanDelete delegate
 */
struct CanDeleteAssetResult
{
public:
	CanDeleteAssetResult( const CanDeleteAssetResult& )	= delete;
	CanDeleteAssetResult( CanDeleteAssetResult&& )		= delete;

	/**
	 * Constructor
	 */
	CanDeleteAssetResult() : 
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
 * @ingroup Editor
 * @brief Delegates used by the editor
 */
struct EditorDelegates
{
	/**
	 * @brief Delegate for called event when assets try delete
	 */
	DECLARE_MULTICAST_DELEGATE( COnAssetsCanDelete, const std::vector< TSharedPtr<class CAsset> >& /*InAssets*/, CanDeleteAssetResult& /*OutResult*/ );

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

#endif // !EDITOR_H