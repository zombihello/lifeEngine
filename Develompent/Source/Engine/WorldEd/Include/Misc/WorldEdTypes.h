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
 * Pointer to WBaseWidget object
 */
typedef class WBaseWidget*								FWBaseWidgetParamRef;

/**
 * @ingroup WorldEd
 * Reference to WBaseWindow object
 */
typedef TRefCountPtr< class WBaseWindow >				FWBaseWindowRef;

/**
 * @ingroup WorldEd
 * Pointer to WBaseWindow object
 */
typedef class WBaseWindow*								FWBaseWindowParamRef;

#endif // !WORLDEDTYPES_H
