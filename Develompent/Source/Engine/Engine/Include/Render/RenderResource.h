/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright BSOD-Games, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef RENDERRESOURCE_H
#define RENDERRESOURCE_H

#include "Core.h"

/**
 * @ingroup Engine
 * @brief A rendering resource which is owned by the rendering thread
 */
class FRenderResource
{
public:
	/**
	 * @brief Constructor
	 */
	FRenderResource();

	/**
	 * @brief Destructor
	 */
	virtual ~FRenderResource();

	/**
	 * @brief Initializes the RHI resources used by this resource.
	 * Called when the resource is initialized.
	 * This is only called by the rendering thread.
	 */
	virtual void InitRHI() {}

	/**
	 * @brief Releases the RHI resources used by this resource.
	 * Called when the resource is released.
	 * This is only called by the rendering thread.
	 */
	virtual void ReleaseRHI() {}

	/**
	 * @brief If the resource's RHI has been initialized, then release and reinitialize it.  Otherwise, do nothing.
	 * This is only called by the rendering thread.
	 */
	virtual void UpdateRHI();

	/**
	 * @brief Initializes the resource.
	 * This is only called by the rendering thread.
	 */
	void InitResource();

	/**
	 * @brief Prepares the resource for deletion.
	 * This is only called by the rendering thread.
	 */
	void ReleaseResource();

	/**
	 * @brief Prepares the resource for update.
	 * This is only called by the rendering thread.
	 */
	void UpdateResource();

	/**
	 * @brief Is resource initialized
	 * @return Return true if resource is initialized, else false
	 */
	FORCEINLINE bool IsInitialized() const
	{
		return isInitialized;
	}

protected:
	volatile bool				isInitialized;		/**< Is resource initialized */
};

/**
 * @ingroup Engine
 * Sends message to rendering thread to initialize a resource
 * 
 * @param[in] InResource Pointer to resource
 */
extern void BeginInitResource( FRenderResource* InResource );

/**
 * @ingroup Engine
 * Sends message to rendering thread to update a resource
 * 
 * @param[in] InResource Pointer to resource
 */
extern void BeginUpdateResource( FRenderResource* InResource );

/**
 * @ingroup Engine
 * Sends message to rendering thread to release resource
 * 
 * @param[in] InResource Pointer to resource
 */
extern void BeginReleaseResource( FRenderResource* InResource );

#endif // !RENDERRESOURCE_H
