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

#ifndef INPUTTEXTDIALOG_H
#define INPUTTEXTDIALOG_H

#include "Core/System/Delegate.h"
#include "UI/ImGUI/ImGUIEngine.h"

/**
 * @ingroup WorldEd
 * @brief Input text dialog
 */
class CInputTextDialog : public CImGUIPopup
{
public:
	/**
	 * @brief Delegate of entered text, called when pressed button 'Ok'
	 */
	DECLARE_MULTICAST_DELEGATE( COnTextEntered, const std::string& /*InText*/ );

	/**
	 * @brief Delegate of cancel enter text, called when pressed button 'Cancel'
	 */
	DECLARE_MULTICAST_DELEGATE( COnCenceled );

	/**
	 * @brief Constructor
	 *
	 * @param InName			Window name
	 * @param InMessage			Message
	 * @param InDefaultText		Default text in input field
	 */
	CInputTextDialog( const std::wstring& InName, const std::wstring& InMessage, const std::wstring& InDefaultText = TEXT( "" ) );

	/**
	 * @brief Get delegate of text entered
	 * @return Return delegate of text entered
	 */
	FORCEINLINE COnTextEntered& OnTextEntered() const
	{
		return onTextEntered;
	}

	/**
	 * @brief Get delegate of canceled enter text
	 * @return Return delegate of canceled enter text
	 */
	FORCEINLINE COnCenceled& OnCenceled() const
	{
		return onCenceled;
	}

protected:
	/**
	 * @brief Method tick interface of a layer
	 */
	virtual void OnTick() override;

private:
	std::wstring				message;			/**< Message */
	std::string					text;				/**< Entered text */
	mutable COnTextEntered		onTextEntered;		/**< Delegates of text entered */
	mutable COnCenceled			onCenceled;			/**< Delegates of canceled enter text */
};

#endif // !INPUTTEXTDIALOG_H