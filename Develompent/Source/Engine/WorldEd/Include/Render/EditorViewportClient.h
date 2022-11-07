/**
 * @file
 * @addtogroup WorldEd WorldEd
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef EDITORVIEWPORTCLIENT_H
#define EDITORVIEWPORTCLIENT_H

#include "Render/Viewport.h"

/**
 * @ingroup WorldEd
 * @brief Viewport client for render editor windows
 */
class CEditorViewportClient : public CViewportClient
{
public:
	/**
	 * @brief Draw viewport
	 *
	 * @param InViewport	Viewport
	 */
	virtual void Draw( CViewport* InViewport ) override;
};

#endif // !EDITORVIEWPORTCLIENT_H