/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef VIEWPORT_H
#define VIEWPORT_H

#include "RenderResource.h"
#include "Misc/RefCounted.h"
#include "RHI/BaseViewportRHI.h"
#include "RHI/TypesRHI.h"

/**
 * @ingroup Engine
 * Implementation for work with viewport
 */
class FViewport : public FRenderResource, public FRefCounted
{
public:
	/**
	 * Constructor
	 */
	FViewport();

	/**
	 * Destructor
	 */
	~FViewport();

	/**
	 * @brief Initializes the RHI resources used by this resource.
	 * Called when the resource is initialized.
	 * This is only called by the rendering thread.
	 */
	virtual void InitRHI() override;

	/**
	 * @brief Releases the RHI resources used by this resource.
	 * Called when the resource is released.
	 * This is only called by the rendering thread.
	 */
	virtual void ReleaseRHI() override;

	/**
	 * Update RHI viewport
	 * 
	 * @param[in] InIsDestroyed Is need destroy viewport
	 * @param[in] InNewSizeX New width of viewport
	 * @param[in] InNewSizeY New height of viewport
	 * @param[in] InNewWindowHandle New window handle of viewport
	 */
	void Update( bool InIsDestroyed, uint32 InNewSizeX, uint32 InNewSizeY, void* InNewWindowHandle );

	/**
	 * Draw scene
	 * 
	 * @param[in] InIsShouldPresent Is frame need present
	 */
	void Draw( bool InIsShouldPresent = true );

	/**
	 * Is initialized viewport RHI
	 * @return Return true if viewport RHI is allocated, else return false
	 */
	FORCEINLINE bool IsValid() const
	{
		return viewportRHI.IsValid();
	}

	/**
	 * Get viewport RHI
	 * @return Return pointer to RHI viewport
	 */
	FViewportRHIRef GetViewportRHI() const
	{
		return viewportRHI;
	}

	/**
	 * Get width of viewport
	 * @return Return width viewport
	 */
	uint32 GetSizeX() const
	{
		return sizeX;
	}

	/**
	 * Get height of viewport
	 * @return Return height viewport
	 */
	uint32 GetSizeY() const
	{
		return sizeY;
	}

	/**
	 * Get window handle of viewport
	 * @return Return window handle
	 */
	void* GetWindowHandle() const
	{
		return windowHandle;
	}

private:
	void*						windowHandle;		/**< Handle to window for render */
	uint32						sizeX;				/**< Width of viewport */
	uint32						sizeY;				/**< Height of viewport */
	FViewportRHIRef				viewportRHI;		/**< Pointer to viewport of RHI */
};

#endif // !VIEWPORT_H