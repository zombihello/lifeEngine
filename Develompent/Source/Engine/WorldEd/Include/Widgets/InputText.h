/**
 * @file
 * @addtogroup WorldEd World editor
 *
 * Copyright BSOD-Games, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef WORLDED_INPUTTEXT_H
#define WORLDED_INPUTTEXT_H

#include <string>

#include "ImGUI/ImGUIEngine.h"
#include "Containers/StringConv.h"
#include "Widgets/BaseWidget.h"

/**
 * @ingroup WorldEd
 * Class for work with input text
 */
class WInputText : public WBaseWidget
{
public:
	/**
	 * Constructor
	 * 
	 * @param[in] InLabel Label of input area
	 * @param[in] InMaxSize The maximum size of the text entered in the field
	 */
	WInputText( const tchar* InLabel, uint32 InMaxSize = 256 );

	/**
	 * Update logic of widget
	 */
	virtual void Tick() override;

	/**
	 * Set label of input area
	 * 
	 * @param[in] InLabel Label of input area
	 */
	FORCEINLINE void SetLable( const tchar* InLabel )
	{
		label = TCHAR_TO_ANSI( InLabel );
	}

	/**
	 * Set max size of entered text in field
	 * @param[in] InMaxSize The maximum size of the text entered in the field
	 */
	FORCEINLINE void SetMaxSize( uint32 InMaxSize )
	{
		enteredText.resize( InMaxSize );
	}

	/**
	 * Set entered text
	 * @param[in] InText Entered text to widget
	 */
	FORCEINLINE void SetEnteredText( const tchar* InText )
	{
		memcpy( ( achar* )enteredText.data(), TCHAR_TO_ANSI( InText ), enteredText.size() );
	}

	/**
	 * Get label of input area
	 * @return Return label of input area
	 */
	FORCEINLINE std::wstring GetLable() const
	{
		return ANSI_TO_TCHAR( label.c_str() );
	}

	/**
	 * Get max size of entered text in field
	 * @return Return max size of entered text in field
	 */
	FORCEINLINE uint32 GetMaxSize() const
	{
		return ( uint32 )enteredText.size();
	}

	/**
	 * Get entered text
	 * @return Return entered text in widget
	 */
	FORCEINLINE std::wstring GetEnteredText() const
	{
		return ANSI_TO_TCHAR( enteredText.c_str() );
	}

private:
	std::string			label;				/**< Label of input area */
	std::string			enteredText;		/**< Entered text to widget */
};

#endif