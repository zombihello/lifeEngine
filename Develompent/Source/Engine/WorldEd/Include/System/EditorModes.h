/**
 * @file
 * @addtogroup WorldEd WorldEd
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef EDITORMODES_H
#define EDITORMODES_H

/**
 * @ingroup WorldEd
 * Enumeration of editor modes
 */
enum EEditorMode
{
	EM_None,				/**< Gameplay, editor disabled */
	EM_Default,				/**< Camera movement, actor placement */
	EM_Translate,			/**< Translate actors */
	EM_Rotate,				/**< Rotate actors */
	EM_Scale,				/**< Scale actors */
	EM_ScaleNonUniform		/**< Non uniform scale actors */
};

#endif // !EDITORMODES_H