/**
 * @file
 * @addtogroup Core Core
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef AXIS_H
#define AXIS_H

/**
 * @ingroup Core
 * Generic axis enum
 */
enum EAxis
{
	A_X			= 1 << 0,		/**< X axis */
	A_Y			= 1 << 1,		/**< Y axis */
	A_Z			= 1 << 2,		/**< Z axis */

	A_Right		= A_X,			/**< Right axis */
	A_Up		= A_Y,			/**< Up axis */
	A_Forward	= A_Z,			/**< Forward axis */
};

#endif // !AXIS_H