/**
 * @file
 * @addtogroup Physics Physics
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef PHYSICSMATERIALDELEGATES_H
#define PHYSICSMATERIALDELEGATES_H

#include "Misc/SharedPointer.h"
#include "System/Delegate.h"

/**
 * @ingroup Physics
 * @brief Delegate for called event when physics materials is updated
 */
DECLARE_MULTICAST_DELEGATE( COnPhysicsMaterialUpdate, const TSharedPtr<class CPhysicsMaterial>& )

/**
 * @ingroup Physics
 * @brief Delegate for called event when physics materials is destroyed
 */
DECLARE_MULTICAST_DELEGATE( COnPhysicsMaterialDestroyed, const TSharedPtr<class CPhysicsMaterial>& )

#endif // !PHYSICSMATERIALDELEGATES_H