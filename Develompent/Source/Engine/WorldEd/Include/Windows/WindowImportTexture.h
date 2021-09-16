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
#include "Widgets/InputText.h"
#include "Widgets/SameLine.h"

/**
 * @ingroup WorldEd
 * Class of work window import texture
 */
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
	/**
	 * Event button clocked of select input texture
	 * 
	 * @param[in] InButton Clicked button
	 */
	void ButtonClicked_SelectInputTexture( class WButton* InButton );

	/**
	 * Event button clicked of cancel
	 * 
	 * @param[in] InButton Clicked button
	 */
	void ButtonClicked_Cancel( class WButton* InButton );

	TRefCountPtr< WSameLine >		sameLine_InputSection;			/**< Pointer to same line widget for input section */
	TRefCountPtr< WButton >			button_InputTexture;			/**< Pointer to button 'Select input texture' */
	TRefCountPtr< WInputText >		inputText_InputTexture;			/**< Pointer to area for show path to selected importing texture */
	TRefCountPtr< WSameLine >		sameLine_OutputSection;			/**< Pointer to same line widget for output section */
	TRefCountPtr< WButton >			button_OutputTexture;			/**< Pointer to button 'Select output path' */
	TRefCountPtr< WInputText >		inputText_OutputText;			/**< Pointer to area for show path to output */
	TRefCountPtr< WSameLine >		sameLine_CommandSection;		/**< Pointer to same line widget for command section */
	TRefCountPtr< WButton >			button_Cancel;					/**< Pointer to button 'Cancel' */
	TRefCountPtr< WButton >			button_Convert;					/**< Pointer to button 'Convert' */
};

#endif // !WORLDED_WINDOWIMPORTTEXTURE_H