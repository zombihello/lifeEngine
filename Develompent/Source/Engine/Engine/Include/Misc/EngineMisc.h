/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef ENGINEMISC_H
#define ENGINEMISC_H

#include <string>
#include "Misc/PhysicsTypes.h"

#if WITH_EDITOR
/**
 * @ingroup Engine
 * @brief Convert text to ESurfaceType
 * 
 * @param InString String
 * @return Return converted text to ESurfaceType. If unknown InString return ST_Default
 */
ESurfaceType Sys_TextToESurfaceType( const std::wstring& InString );
#endif // WITH_EDITOR

#endif // !ENGINEMISC_H