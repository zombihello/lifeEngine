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

#ifndef TEXTUREEDITORWINDOW_H
#define TEXTUREEDITORWINDOW_H

#include "UI/ImGUI/ImGUIEngine.h"
#include "Engine/Render/Texture.h"
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