/**
 * @file
 * @addtogroup WorldEd World editor
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef STATUSBARS_H
#define STATUSBARS_H

#include "System/Controls.h"

/**
 * @ingroup WorldEd
 * @brief The standard editor status bar. Displayed during normal editing
 */
class WxStatusBarStandard : public WxStatusBar
{
public:
	/**
	 * @brief Init UI
	 */
	virtual void SetUp() override;

	/**
	 * @brief Update UI
	 */
	virtual void UpdateUI() override;

	/**
	 * @brief Update the position and size of the status bar
	 */
	virtual void RefreshPositionAndSize() override;

private:
	//
	// Event handlers
	//
	void OnSize( wxSizeEvent& InEvent );

	DECLARE_EVENT_TABLE()
};


#endif // !STATUSBARS_H