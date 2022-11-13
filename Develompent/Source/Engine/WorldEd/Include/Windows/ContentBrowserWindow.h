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

#include "ImGUI/ImGUIEngine.h"
#include "Render/Texture.h"
#include "System/Package.h"

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
};

#endif // !CONTENTBROWSERWINDOW_H