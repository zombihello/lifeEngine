/**
 * @file
 * @addtogroup WorldEd World editor
 *
 * Copyright BSOD-Games, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef WORLDED_WINDOWFILEDIALOG_H
#define WORLDED_WINDOWFILEDIALOG_H

#include "Windows/BaseWindow.h"

 /**
  * @ingroup WorldEd
  * Class of work window file dialog
  */
class WWindowFileDialog : public WBaseWindow
{
public:
	/**
	 * Constructor
	 * 
	 * @param[in] InTitle Title window
	 */
	WWindowFileDialog( const tchar* InTitle );

	/**
	 * Initialize widget
	 */
	virtual void Init() override;

	/**
	 * Update logic of widget
	 */
	virtual void Tick() override;
};

#endif // !WORLDED_WINDOWFILEDIALOG_H