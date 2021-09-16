/**
 * @file
 * @addtogroup WorldEd World editor
 *
 * Copyright BSOD-Games, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef WORLDED_BUTTON_H
#define WORLDED_BUTTON_H

#include <string>
#include <functional>

#include "ImGUI/ImGUIEngine.h"
#include "Containers/StringConv.h"
#include "Widgets/BaseWidget.h"
#include "System/Delegate.h"

DECLARE_DELEGATE( FOnButtonClicked, class WButton* )

/**
 * @ingroup WorldEd
 * Class for work with button
 */
class WButton : public WBaseWidget
{
public:
	/**
	 * Constructor
	 */
	WButton();

	/**
	 * Constructor
	 *
	 * @param[in] InName Name button
	 * @param[in] InSize Size button
	 */
	WButton( const tchar* InName, const ImVec2& InSize );

	/**
	 * Update logic of widget
	 */
	virtual void Tick() override;

	/**
	 * Set size
	 * @param[in] InSize Size button
	 */
	FORCEINLINE void SetSize( const ImVec2& InSize )
	{
		size = InSize;
	}

	/**
	 * Get size
	 * @return Return size button
	 */
	FORCEINLINE ImVec2 GetSize() const
	{
		return size;
	}

	/**
	 * Set name
	 *
	 * @param[in] InName Name button
	 */
	FORCEINLINE void SetName( const tchar* InName )
	{
		name = TCHAR_TO_ANSI( InName );
	}

	/**
	 * Get name
	 *
	 * @return Return name button
	 */
	FORCEINLINE std::wstring GetName() const
	{
		return ANSI_TO_TCHAR( name.c_str() );
	}

	/**
	 * Get delegate to event of button clicked
	 * @return Return delegate to event of button clicked
	 */
	FORCEINLINE FOnButtonClicked& OnButtonClicked()
	{
		return onButtonClicked;
	}

private:
	ImVec2					size;				/**< Button size */
	FOnButtonClicked		onButtonClicked;	/**< Event dispatcher of button clicked */
	std::string				name;				/**< Name button */
};

#endif // !WORLDED_BUTTON_H