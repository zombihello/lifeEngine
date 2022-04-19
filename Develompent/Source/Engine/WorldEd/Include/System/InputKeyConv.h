/**
 * @file
 * @addtogroup WorldEd WorldEd
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef INPUTKEYCONV_H
#define INPUTKEYCONV_H

#include "System/ButtonCode.h"

#if 0
/**
 * @ingroup WorldEd
 * @brief Convert Qt virtual key to engine keycode
 * 
 * @param InQtKey Qt key
 * @param InModifiers Modifiers flags
 * @return Return engine keycode
 */
EButtonCode appQtKeyToButtonCode( Qt::Key InQtKey, uint32 InModifiers );

/**
 * @ingroup WorldEd
 * @brief Convert Qt mouse button to engine keycode
 * 
 * @param InButton Qt mouse button
 * @return Return engine keycode
 */
EButtonCode appQtMouseButtonToButtonCode( Qt::MouseButton InButton );
#endif // 0

#endif // !INPUTKEYCONV_H