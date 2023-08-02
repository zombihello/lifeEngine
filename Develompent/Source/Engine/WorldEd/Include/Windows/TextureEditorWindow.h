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
	TSharedPtr<CTexture2D>					texture2D;			/**< Texture 2D */
	CViewportWidget							viewportWidget;		/**< Viewport widget */
	class CTexturePreviewViewportClient*	viewportClient;		/**< Viewport client */
	uint32									currentMipmap;		/**< Current mipmap to view */
};

#endif // !TEXTUREEDITORWINDOW_H