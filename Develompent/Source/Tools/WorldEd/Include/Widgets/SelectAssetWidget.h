/**
 * @file
 * @addtogroup WorldEd World editor
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

#ifndef SELECTASSETWIDGET_H
#define SELECTASSETWIDGET_H

#include "Core/System/Delegate.h"
#include "Engine/Render/Texture.h"

/**
 * @ingroup WorldEd
 * @brief Select asset widget
 */
class CSelectAssetWidget
{
public:
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
	FORCEINLINE void SetLabel( const std::wstring& InNewLabel )
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
	FORCEINLINE const std::wstring& GetAssetReference() const
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
	FORCEINLINE const std::wstring& GetLabel() const
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

	/**
	 * @brief Is selected asset
	 * @return Return true if asset is selected
	 */
	FORCEINLINE bool IsSelectedAsset() const
	{
		return bSelectedAsset;
	}

private:
	bool						bInit;				/**< Is inited widget */
	bool						bSelectedAsset;		/**< Is selected asset */
	uint32						assetSlot;			/**< Index of slot */
	std::wstring				assetReference;		/**< Reference to asset in string type */
	std::wstring				label;				/**< Label */
	Texture2DRHIRef_t			previewTexture;		/**< Preview texture */
	mutable COnSelectedAsset	onSelectedAsset;	/**< Delegates of selected asset */
	mutable COnOpenAssetEditor	onOpenAssetEditor;	/**< Delegates of open asset editor */
};

#endif // !SELECTASSETWIDGET_H