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

#ifndef DIALOGWINDOW_H
#define DIALOGWINDOW_H

#include "Core/System/Delegate.h"
#include "UI/ImGUI/ImGUIEngine.h"

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
		BT_Cancel	= 1 << 1,	/**< Button 'Cancel' */
		BT_Yes		= 1 << 2,	/**< Button 'Yes' */
		BT_YesToAll	= 1 << 3,	/**< Button 'Yes To All' */
		BT_No		= 1 << 4,	/**< Button 'No' */
		BT_NoToAll	= 1 << 5	/**< Button 'No To All' */
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