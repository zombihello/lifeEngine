/**
 * @file
 * @addtogroup WorldEd World editor
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef TEXTUREEDITORWINDOW_H
#define TEXTUREEDITORWINDOW_H

#include "ImGUI/ImGUIEngine.h"
#include "Render/Texture.h"
#include "Widgets/ViewportWidget.h"

/**
 * @ingroup WorldEd
 * @brief Texture editor window
 */
class CTextureEditorWindow : public CImGUILayer
{
public:
	/**
	 * @brief Enumeration icon types
	 */
	enum EIconType
	{
		IT_Import,		/**< Icon for button 'Import' */
		IT_R,			/**< Icon for button 'R' */
		IT_G,			/**< Icon for button 'G' */
		IT_B,			/**< Icon for button 'B' */
		IT_A,			/**< Icon for button 'A' */
		IT_Num			/**< Number of icon types */
	};

	/**
	 * @brief Constructor
	 * @param InTexture2D	Texture 2D
	 */
	CTextureEditorWindow( const TSharedPtr<CTexture2D>& InTexture2D );

	/**
	 * @brief Destructor
	 */
	~CTextureEditorWindow();

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
	TAssetHandle<CTexture2D>				icons[IT_Num];		/**< Array of icons */
	TSharedPtr<CTexture2D>					texture2D;			/**< Texture 2D */
	CViewportWidget							viewportWidget;		/**< Viewport widget */
	class CTexturePreviewViewportClient*	viewportClient;		/**< Viewport client */
	uint32									currentMipmap;		/**< Current mipmap to view */
};

#endif // !TEXTUREEDITORWINDOW_H