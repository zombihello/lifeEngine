/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright BSOD-Games, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef RENDERRESOURCE_H
#define RENDERRESOURCE_H

#include <set>
#include "Core.h"

/**
 * @ingroup Engine
 * @brief A rendering resource which is owned by the rendering thread
 */
class CRenderResource
{
public:
	/**
	 * @brief Constructor
	 */
	CRenderResource();

	/**
	 * @brief Destructor
	 */
	virtual ~CRenderResource();

	/**
	 * @brief Get global resource list
	 * @return Return reference to list of global resources
	 */
	static std::set< CRenderResource* >& GetResourceList();

	/**
	 * @brief Initializes the resource.
	 * @warning This is only called by the rendering thread.
	 */
	void InitResource();

	/**
	 * @brief Prepares the resource for deletion.
	 * @warning This is only called by the rendering thread.
	 */
	void ReleaseResource();

	/**
	 * @brief Prepares the resource for update.
	 * @warning This is only called by the rendering thread.
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

	/**
	 * @brief Is global resource
	 * @return Return true if this resource is global, else return false
	 */
	virtual bool IsGlobal() const;

protected:
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

private:
	volatile bool				isInitialized;		/**< Is resource initialized */
	bool						bInGlobalList;		/**< Is resource already containing in global list */
};

/**
 * @ingroup Engine
 * Sends message to rendering thread to initialize a resource
 * 
 * @param[in] InResource Pointer to resource
 */
extern void BeginInitResource( CRenderResource* InResource );

/**
 * @ingroup Engine
 * Sends message to rendering thread to update a resource
 * 
 * @param[in] InResource Pointer to resource
 */
extern void BeginUpdateResource( CRenderResource* InResource );

/**
 * @ingroup Engine
 * Sends message to rendering thread to release resource
 * 
 * @param[in] InResource Pointer to resource
 */
extern void BeginReleaseResource( CRenderResource* InResource );

#endif // !RENDERRESOURCE_H
