/**
 * @file
 * @addtogroup WorldEd World editor
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef DIALOGWINDOW_H
#define DIALOGWINDOW_H

#include "ImGUI/ImGUIEngine.h"
#include "System/Delegate.h"

/**
 * @ingroup WorldEd
 * @brief Dialog window
 */
class CDialogWindow : public CImGUIPopup
{
public:
	/**
	 * @brief Enumeration button type
	 */
	enum EButtonType
	{
		BT_None		= 0,		/**< No button */
		BT_Ok		= 1 << 0,	/**< Button 'Ok' */
		BT_Cancel	= 1 << 1	/**< Button 'Cancel' */
	};

	/**
	 * @brief Delegate of press button
	 */
	DECLARE_MULTICAST_DELEGATE( COnButtonPressed, EButtonType /*InButtonType*/ );

	/**
	 * @brief Constructor
	 *
	 * @param InName	Window name
	 * @param InMessage	Message
	 * @param InButtons	Buttons (see EButtonType)
	 */
	CDialogWindow( const std::wstring& InName, const std::wstring& InMessage, uint32 InButtons = BT_Ok );

	/**
	 * @brief Get delegate of button pressed
	 * @return Return delegate of button pressed
	 */
	FORCEINLINE COnButtonPressed& OnButtonPressed() const
	{
		return onButtonPressed;
	}

protected:
	/**
	 * @brief Method tick interface of a layer
	 */
	virtual void OnTick() override;

private:
	uint32						buttons;			/**< Buttons (see EButtonType) */
	std::wstring				message;			/**< Message */
	mutable COnButtonPressed	onButtonPressed;	/**< Delegates of button pressed */
};

#endif // !DIALOGWINDOW_H