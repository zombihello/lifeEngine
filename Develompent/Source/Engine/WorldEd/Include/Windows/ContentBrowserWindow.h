/**
 * @file
 * @addtogroup WorldEd World editor
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef CONTENTBROWSERWINDOW_H
#define CONTENTBROWSERWINDOW_H

#include <string>
#include <set>

#include "Misc/EngineGlobals.h"
#include "System/InputSystem.h"
#include "ImGUI/ImGUIEngine.h"
#include "Render/Texture.h"

 /**
  * @ingroup WorldEd
  * @brief Window content browser
  */
class CContentBrowserWindow : public CImGUILayer
{
public:
	/**
	 * @brief Constructor
	 *
	 * @param InName	Window name
	 */
	CContentBrowserWindow( const std::wstring& InName );

	/**
	 * @brief Init
	 */
	virtual void Init() override;

protected:
	/**
	 * @brief Method tick interface of a layer
	 */
	virtual void OnTick() override;

private:
	/**
	 * @brief Draw list of packages in file system
	 * 
	 * @param InRootDir		Root directory
	 */
	void DrawPackageList( const std::wstring& InRootDir );

	/**
	 * @brief Draw all assets in current package
	 */
	void DrawAssets();

	/**
	 * @brief Draw popup menu of assets
	 */
	void DrawAssetsPopupMenu();

	/**
	 * @brief Draw popup menu of packages
	 */
	void DrawPackagesPopupMenu();

	/**
	 * @brief Process select item
	 * 
	 * @param InItemName			Item name
	 * @param InOutSetItems			Array of unique selected items
	 * @param InMouseButtonType		Mouse button type
	 * @param InIsAlreadySelected	Is item already exist in InOutSetItems
	 */
	FORCEINLINE void ProcessSelectItem( const std::wstring InItemName, std::set<std::wstring>& InOutSetItems, ImGuiMouseButton InMouseButtonType, bool InIsAlreadySelected ) const
	{
		const bool		bCtrlDown = GInputSystem->IsKeyDown( BC_KeyLControl ) || GInputSystem->IsKeyDown( BC_KeyRControl );

		// Select item if pressd left mouse button
		if ( InMouseButtonType == ImGuiMouseButton_Left )
		{
			if ( !bCtrlDown )
			{
				InOutSetItems.clear();
			}

			if ( InIsAlreadySelected && bCtrlDown )
			{
				InOutSetItems.erase( InItemName );
			}
			else if ( !InIsAlreadySelected || InIsAlreadySelected && selectedAssets.empty() )
			{
				InOutSetItems.insert( InItemName );
			}
		}
		// Select item for popup menu if pressd right mouse button
		else if ( InMouseButtonType == ImGuiMouseButton_Right )
		{
			if ( !InIsAlreadySelected || InOutSetItems.size() < 2 )
			{
				InOutSetItems.clear();
			}

			if ( !InIsAlreadySelected || InIsAlreadySelected && InOutSetItems.empty() )
			{
				InOutSetItems.insert( InItemName );
			}
		}
	}

	/**
	 * @brief Is show all asset types
	 * @return Return TRUE if need show all asset types, otherwise will return FALSE
	 */
	FORCEINLINE bool IsShowAllAssetTypes() const
	{
		bool		bNeedForAll[AT_Count];
		memset( bNeedForAll, 1, ARRAY_COUNT( bNeedForAll ) * sizeof( bool ) );
		return !memcmp( filterAssetType, bNeedForAll, ARRAY_COUNT( bNeedForAll ) * sizeof( bool ) );
	}

	/**
	 * @brief Get preview filter asset type
	 * @return Return preview filter asset type
	 */
	std::string GetPreviewFilterAssetType() const;

	PackageRef_t				package;					/**< Current package in preview */
	std::string					filterPackage;				/**< Filter by package name */
	std::string					filterAsset;				/**< Filter by asset name */
	bool						filterAssetType[AT_Count];	/**< Filter by asset type */
	float						padding;					/**< Padding in asset section */
	float						thumbnailSize;				/**< Size of thumbnail */
	TAssetHandle<CTexture2D>	assetIcons[AT_Count];		/**< Array of asset icons */
	std::set<std::wstring>		selectedAssets;				/**< Set of selected assets */
	std::set<std::wstring>		selectedPackages;			/**< Set of selected packages */
};

#endif // !CONTENTBROWSERWINDOW_H