/**
 * @file
 * @addtogroup WorldEd World editor
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef PHYSICSMATERIALEDITORWINDOW_H
#define PHYSICSMATERIALEDITORWINDOW_H

#include "ImGUI/ImGUIEngine.h"
#include "System/PhysicsMaterial.h"
#include "WorldEd.h"

/**
 * @ingroup WorldEd
 * @brief Physics material editor window
 */
class CPhysicsMaterialEditorWindow : public CImGUILayer
{
public:
	/**
	 * @brief Constructor
	 * @param InPhysMaterial	Physics material
	 */
	CPhysicsMaterialEditorWindow( const TSharedPtr<CPhysicsMaterial>& InPhysMaterial );

	/**
	 * @brief Destructor
	 */
	~CPhysicsMaterialEditorWindow();

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
	 * @brief Called event when asset try delete
	 *
	 * @param InAssets	Array of assets to delete
	 * @param OutResult Result, we can is delete this assets?
	 */
	void OnAssetsCanDelete( const std::vector<TSharedPtr<CAsset>>& InAssets, struct CanDeleteAssetResult& OutResult );

	/**
	 * @brief Called event when asset is reloaded
	 * @param InAssets	Array of reloaded assets
	 */
	void OnAssetsReloaded( const std::vector<TSharedPtr<CAsset>>& InAssets );

	TSharedPtr<CPhysicsMaterial>							physMaterial;			/**< Physics material */
	EditorDelegates::COnAssetsCanDelete::DelegateType_t*	assetsCanDeleteHandle;	/**< Handle delegate of assets can delete */
};


#endif // !PHYSICSMATERIALEDITORWINDOW_H