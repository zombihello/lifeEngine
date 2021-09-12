/**
 * @file
 * @addtogroup WorldEd World editor
 *
 * Copyright BSOD-Games, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef WORLDED_TEXT_H
#define WORLDED_TEXT_H

#include <string>

#include "Containers/StringConv.h"
#include "Widgets/BaseWidget.h"

class WText : public WBaseWidget
{
public:
	/**
	 * Constructor
	 * 
	 * @param[in] InText Text in widget
	 */
	WText( const tchar* InText = TEXT( "" ) );

	/**
	 * Update logic of widget
	 */
	virtual void Tick() override;

	/**
	 * Set text
	 *
	 * @param[in] InText New text in widget
	 */
	FORCEINLINE void SetText( const tchar* InText )
	{
		text = TCHAR_TO_ANSI( InText );
	}

	/**
	 * Get text
	 *
	 * @return Return text in widget
	 */
	FORCEINLINE std::wstring GetText() const
	{
		return ANSI_TO_TCHAR( text.c_str() );
	}

private:
	std::string			text;		/**< Text in widget */
};

#endif // !WORLDED_TEXT_H