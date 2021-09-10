/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright BSOD-Games, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef BASERESOURCERHI_H
#define BASERESOURCERHI_H

#include "Misc/RefCounted.h"

/**
 * @ingroup Engine
 * Base class for all RHI resources
 */
class FBaseResourceRHI : public FRefCounted
{
public:
	/**
	 * Destructor
	 */
	virtual ~FBaseResourceRHI()
	{}
};

#endif // !BASERESOURCERHI_H