/**
 * @file
 * @addtogroup WorldEd World editor
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef INPUTTEXTDIALOG_H
#define INPUTTEXTDIALOG_H

#include "ImGUI/ImGUIEngine.h"
#include "System/Delegate.h"

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