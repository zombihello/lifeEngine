/**
 * @file
 * @addtogroup WorldEd World editor
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef SELECTASSETWIDGET_H
#define SELECTASSETWIDGET_H

#include "Render/Texture.h"
#include "System/Delegate.h"

/**
 * @ingroup WorldEd
 * @brief Select asset widget
 */
class CSelectAssetWidget
{
public:
	/**
	 * @brief Enumeration icon types
	 */
	enum EIconType
	{
		IT_Insert,		/**< Icon for button 'Insert' */
		IT_Browse,		/**< Icon for button 'Browse' */
		IT_Remove,		/**< Icon for button 'Remove' */
		IT_Num			/**< Number of icon types */
	};

	/**
	 * @brief Delegate for called event when selected asset
	 */
	DECLARE_MULTICAST_DELEGATE( COnSelectedAsset, uint32 /*InAssetSlot*/, const std::wstring& /*InNewAssetReference*/ );

	/**
	 * @brief Delegate for called event when need open asset editor
	 */
	DECLARE_MULTICAST_DELEGATE( COnOpenAssetEditor, uint32 /*InAssetSlot*/ );

	/**
	 * @brief Constructor
	 * @param InAssetSlot	Asset slot
	 */
	CSelectAssetWidget( uint32 InAssetSlot );

	/**
	 * @brief Init
	 */
	void Init();

	/**
	 * @brief Tick
	 */
	void Tick();

	/**
	 * @brief Set asset reference
	 *
	 * @param InAssetReference	Asset reference in format <AssetType>'<PackageName>:<AssetName>
	 * @param InIsEmitEvent		Is need emit event of change asset reference
	 */
	void SetAssetReference( const std::wstring& InAssetReference, bool InIsEmitEvent = true );

	/**
	 * @brief Set preview texture
	 * @param InTexture2D	Preview texture. If NULL she will not drawed
	 */
	FORCEINLINE void SetPreviewTexture( Texture2DRHIParamRef_t InTexture2D )
	{
		previewTexture = InTexture2D;
	}

	/**
	 * @brief Set label of the widget
	 * @param InNewLabel	New label of the widget
	 */
	FORCEINLINE void SetLabel( const std::string& InNewLabel )
	{
		label = InNewLabel;
	}

	/**
	 * @brief Is widget inited
	 * @return Return TRUE if widget is inited
	 */
	FORCEINLINE bool IsInit() const
	{
		return bInit;
	}

	/**
	 * @brief Get preview texture
	 * @return Return preview texture, if not exist will return NULL
	 */
	FORCEINLINE Texture2DRHIRef_t GetPreviewTexture() const
	{
		return previewTexture;
	}

	/**
	 * @brief Get asset reference
	 * @return Return asset reference
	 */
	FORCEINLINE const std::string& GetAssetReference() const
	{
		return assetReference;
	}

	/**
	 * @brief Get delegate of selected asset
	 * @return Return delegate of selected asset
	 */
	FORCEINLINE COnSelectedAsset& OnSelectedAsset() const
	{
		return onSelectedAsset;
	}

	/**
	 * @brief Get label of the widget
	 * @return Return label of the widget
	 */
	FORCEINLINE const std::string& GetLabel() const
	{
		return label;
	}

	/**
	 * @brief Get delegate of open asset editor
	 * @return Return delegate of open asset editor
	 */
	FORCEINLINE COnOpenAssetEditor& OnOpenAssetEditor() const
	{
		return onOpenAssetEditor;
	}

private:
	bool						bInit;				/**< Is inited widget */
	uint32						assetSlot;			/**< Index of slot */
	TAssetHandle<CTexture2D>	icons[IT_Num];		/**< Array of icons */
	std::string					assetReference;		/**< Reference to asset in string type */
	std::string					label;				/**< Label */
	Texture2DRHIRef_t			previewTexture;		/**< Preview texture */
	mutable COnSelectedAsset	onSelectedAsset;	/**< Delegates of selected asset */
	mutable COnOpenAssetEditor	onOpenAssetEditor;	/**< Delegates of open asset editor */
};

#endif // !SELECTASSETWIDGET_H