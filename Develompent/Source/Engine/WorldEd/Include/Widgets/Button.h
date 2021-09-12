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

class WButton : public WBaseWidget
{
public:
	/**
	 * Callback function type
	 */
	typedef std::function< void() >				FCallbackFunction;

	/**
	 * Constructor
	 */
	WButton();

	/**
	 * Constructor
	 *
	 * @param[in] InName Name button
	 * @param[in] InSize Size button
	 * @param[in] InCallback Function called when button clicked
	 */
	WButton( const tchar* InName, const ImVec2& InSize, FCallbackFunction InCallback );

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
	 * Set callback
	 * 
	 * @param[in] InCallback Function called when button clicked
	 */
	FORCEINLINE void SetCallback( FCallbackFunction InCallback )
	{
		callback = InCallback;
	}

	/**
	 * Get callback
	 * @return Return callback for clicked event
	 */
	FORCEINLINE FCallbackFunction GetCallback() const
	{
		return callback;
	}

private:
	ImVec2					size;		/**< Button size */
	FCallbackFunction		callback;	/**< Function called when button clicked */
	std::string				name;		/**< Name button */
};

#endif // !WORLDED_BUTTON_H