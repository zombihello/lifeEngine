/**
 * @file
 * @addtogroup Engine Engine
 *
 * @authors: Yehor Pohuliaka
 * @date: 4/19/2021
 */

#ifndef BASERHI_H
#define BASERHI_H

#include "Misc/Types.h"

/**
 * @ingroup Engine
 * @brief Base class of RHI
 */
class BaseRHI
{
public:
	/**
	 * @brief Destructor
	 */
	virtual										~BaseRHI() {}

	/**
	 * @brief Initialize RHI
	 * 
	 * @param[in] InIsEditor Is current application editor
	 */
	virtual void								Init( bool InIsEditor ) {}

	/**
	 * @brief Create viewport
	 * 
	 * @param[in] InWindowHandle OS handle on window
	 * @param[in] InWidth Width of viewport
	 * @param[in] InHeight Height of viewport
	 * @return Pointer on viewport
	 */
	virtual class BaseViewportRHI*				CreateViewport( void* InWindowHandle, uint32 InWidth, uint32 InHeight ) { return nullptr; }
};

#endif // !BASERHI_H
