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

 /**
  * @ingroup WorldEd
  * @brief Texture editor window
  */
class CTextureEditorWindow : public CImGUILayer
{
public:
	/**
	 * @brief Constructor
	 *
	 * @param InTexture2D	Texture 2D
	 * @param InName		Window name
	 */
	CTextureEditorWindow( const TSharedPtr<CTexture2D>& InTexture2D, const std::wstring& InName );

protected:
	/**
	 * @brief Method tick interface of a layer
	 */
	virtual void OnTick() override;

private:
	TSharedPtr<CTexture2D>		texture2D;		/**< Texture 2D */
};

#endif // !TEXTUREEDITORWINDOW_H