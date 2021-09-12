/**
 * @file
 * @addtogroup WorldEd World editor
 *
 * Copyright BSOD-Games, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef WORLDEDTYPES_H
#define WORLDEDTYPES_H

#include "Misc/RefCountPtr.h"

 /**
  * @ingroup WorldEd
  * Reference to WBaseWidget object
  */
typedef TRefCountPtr< class WBaseWidget >				FWBaseWidgetRef;

/**
 * @ingroup WorldEd
 * Pointer to FBaseWidget object
 */
typedef class WBaseWidget*								FWBaseWidgetParamRef;

#endif // !WORLDEDTYPES_H
