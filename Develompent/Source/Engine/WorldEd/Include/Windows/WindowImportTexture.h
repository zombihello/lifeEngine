/**
 * @file
 * @addtogroup WorldEd World editor
 *
 * Copyright BSOD-Games, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef WORLDED_WINDOWIMPORTTEXTURE_H
#define WORLDED_WINDOWIMPORTTEXTURE_H

#include "Windows/BaseWindow.h"
#include "Widgets/Button.h"
#include "Widgets/Text.h"

class WWindowImportTexture : public WBaseWindow
{
public:
	/**
	 * Constructor
	 */
	WWindowImportTexture();

	/**
	 * Initialize widget
	 */
	virtual void Init() override;

private:
	TRefCountPtr< WButton >			button_exit;
	TRefCountPtr< WText >			text_hello;
};

#endif // !WORLDED_WINDOWIMPORTTEXTURE_H