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
class CBaseResourceRHI : public CRefCounted
{
public:
	/**
	 * Destructor
	 */
	virtual ~CBaseResourceRHI()
	{}
};

#endif // !BASERESOURCERHI_H