/**
 * @file
 * @addtogroup WorldEd WorldEd
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef EDITORINTERFACEVIEWPORTCLIENT_H
#define EDITORINTERFACEVIEWPORTCLIENT_H

#include "Render/Viewport.h"

/**
 * @ingroup WorldEd
 * @brief Viewport client for render editor interface
 */
class CEditorInterfaceViewportClient : public CViewportClient
{
public:
	/**
	 * @brief Draw viewport
	 *
	 * @param InViewport	Viewport
	 */
	virtual void Draw( CViewport* InViewport ) override;
};

#endif // !EDITORINTERFACEVIEWPORTCLIENT_H