/**
 * @file
 * @addtogroup WorldEd World editor
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef AUDIOBANKEDITORWINDOW_H
#define AUDIOBANKEDITORWINDOW_H

#include "ImGUI/ImGUIEngine.h"
#include "System/AudioBank.h"
#include "WorldEd.h"

/**
 * @ingroup WorldEd
 * @brief Audio bank editor window
 */
class CAudioBankEditorWindow : public CImGUILayer
{
public:
	/**
	 * @brief Constructor
	 * @param InAudioBank	Audio bank
	 */
	CAudioBankEditorWindow( const TSharedPtr<CAudioBank>& InAudioBank );

	/**
	 * @brief Destructor
	 */
	~CAudioBankEditorWindow();

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
	 * @brief Update asset info
	 */
	void UpdateAssetInfo();

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

	TSharedPtr<CAudioBank>									audioBank;				/**< Audio bank */
	AudioBankInfo											audioBankInfo;			/**< Audio bank info */
	AudioBankHandle_t										audioBankHandle;		/**< Audio bank handle */
	class CAudioComponent*									audioComponent;			/**< Audio component */
	EditorDelegates::COnAssetsCanDelete::DelegateType_t*	assetsCanDeleteHandle;	/**< Handle delegate of assets can delete */
	EditorDelegates::COnAssetsReloaded::DelegateType_t*	assetsReloadedHandle;	/**< Handle delegate of reloaded assets */
};

#endif // !AUDIOBANKEDITORWINDOW_H