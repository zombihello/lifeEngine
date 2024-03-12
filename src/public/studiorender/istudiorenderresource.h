/**
 * @file
 * @addtogroup studiorender studiorender
 *
 * ************************************************************
 *                  This file is part of:
 *                      LIFEENGINE
 *          https://github.com/zombihello/lifeEngine
 * ************************************************************
 * Copyright (C) 2024 Yehor Pohuliaka.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef ISTUDIORENDERRESOURCE_H
#define ISTUDIORENDERRESOURCE_H

#include "interfaces/interfaces.h"
#include "core/types.h"
#include "core/platform.h"
#include "studiorender/istudiorendercmd.h"
#include "studiorender/istudiorender.h"

/**
 * @ingroup studiorender
 * @brief A render resource interface which is owned by the render thread
 */
class IStudioRenderResource
{
public:
	/**
	 * @brief Initializes the resource
	 * @warning This is only called by the render thread
	 * 
	 * Call R_BeginInitResource for init the resource
	 */
	virtual void InitResource() = 0;

	/**
	 * @brief Prepares the resource for deletion
	 * @warning This is only called by the render thread
	 * 
	 * Call R_BeginReleaseResource for release the resource
	 */
	virtual void ReleaseResource() = 0;

	/**
	 * @brief Prepares the resource for update
	 * @warning This is only called by the render thread
	 * 
	 * Call R_BeginUpdateResource for update the resource
	 */
	virtual void UpdateResource() = 0;

	/**
	 * @brief Is resource initialized
	 * @return Return TRUE if resource is initialized, otherwise FALSE
	 */
	virtual bool IsInitialized() const = 0;

protected:
	/**
	 * @brief Initializes the StudioAPI resources used by this resource
	 * @warning This is only called by the render thread
	 * 
	 * Called when the resource is initialized
	 */
	virtual void InitStudioAPI() = 0;

	/**
	 * @brief Releases the StudioAPI resources used by this resource
	 * @warning This is only called by the render thread
	 * 
	 * Called when the resource is released
	 */
	virtual void ReleaseStudioAPI() = 0;

	/**
	 * @brief If the resource's StudioAPI has been initialized, then release and reinitialize it. Otherwise, do nothing
	 * @warning This is only called by the render thread
	 */
	virtual void UpdateStudioAPI() = 0;
};

/**
 * @ingroup studiorender
 * @brief Base class to implement a render resource
 * @warning For use g_pStudioAPI must be valid
 */
template<class TBaseClass>
class TStudioRenderResource : public TBaseClass
{
public:
	/**
	 * @brief Constructor
	 */
	TStudioRenderResource()
		: bInitialized( false )
	{}

	/**
	 * @brief Destructor
	 */
	virtual ~TStudioRenderResource()
	{
		if ( !bInitialized )
		{
			return;
		}

		// Deleting an initialized IStudioRenderResource will result in a crash later since it is still linked
		AssertMsg( false, "An IStudioRenderResource was deleted without being released first!" );
	}

	/**
	 * @brief Initializes the resource
	 * @warning This is only called by the render thread
	 *
	 * Call R_BeginInitResource for init the resource
	 */
	virtual void InitResource() override
	{
		if ( bInitialized )
		{
			return;
		}

		Assert( g_pStudioAPI );
		InitStudioAPI();
		bInitialized = true;
	}

	/**
	 * @brief Prepares the resource for deletion
	 * @warning This is only called by the render thread
	 *
	 * Call R_BeginReleaseResource for release the resource
	 */
	virtual void ReleaseResource() override
	{
		if ( !bInitialized )
		{
			return;
		}

		Assert( g_pStudioAPI )
		ReleaseStudioAPI();
		bInitialized = false;
	}

	/**
	 * @brief Prepares the resource for update
	 * @warning This is only called by the render thread
	 *
	 * Call R_BeginUpdateResource for update the resource
	 */
	virtual void UpdateResource() override
	{
		if ( !bInitialized )
		{
			InitResource();
		}
		else
		{
			UpdateStudioAPI();
		}
	}

	/**
	 * @brief Is resource initialized
	 * @return Return TRUE if resource is initialized, otherwise FALSE
	 */
	virtual bool IsInitialized() const override
	{
		return bInitialized;
	}

protected:
	/**
	 * @brief Initializes the StudioAPI resources used by this resource
	 * @warning This is only called by the render thread
	 *
	 * Called when the resource is initialized
	 */
	virtual void InitStudioAPI() override
	{}

	/**
	 * @brief Releases the StudioAPI resources used by this resource
	 * @warning This is only called by the render thread
	 *
	 * Called when the resource is released
	 */
	virtual void ReleaseStudioAPI() override
	{}

	/**
	 * @brief If the resource's StudioAPI has been initialized, then release and reinitialize it. Otherwise, do nothing
	 * @warning This is only called by the render thread
	 */
	virtual void UpdateStudioAPI() override
	{
		ReleaseStudioAPI();
		InitStudioAPI();
	}

private:
	volatile bool	bInitialized;		/**< Is resource initialized */
};

/**
 * @ingroup studiorender
 * @brief Sends message to render thread to initialize a resource
 * @warning For use g_pStudioRender must be valid
 * 
 * @param pResource		Render resource
 */
FORCEINLINE void R_BeginInitResource( IStudioRenderResource* pResource )
{
	Assert( pResource );
	UNIQUE_RENDER_COMMAND_ONEPARAMETER( CInitResourceCommand, IStudioRenderResource*, pResource, pResource,
										{
											pResource->InitResource();
										} );
}

/**
 * @ingroup studiorender
 * @brief Sends message to render thread to update a resource
 * @warning For use g_pStudioRender must be valid
 * 
 * @param pResource		Render resource
 */
FORCEINLINE void R_BeginUpdateResource( IStudioRenderResource* pResource )
{
	Assert( pResource );
	UNIQUE_RENDER_COMMAND_ONEPARAMETER( CReleaseResourceCommand, IStudioRenderResource*, pResource, pResource,
										{
											pResource->UpdateResource();
										} );
}

/**
 * @ingroup studiorender
 * @brief Sends message to render thread to release resource
 * @warning For use g_pStudioRender must be valid
 * 
 * @param pResource		Render resource
 */
FORCEINLINE void R_BeginReleaseResource( IStudioRenderResource* pResource )
{
	Assert( pResource );
	UNIQUE_RENDER_COMMAND_ONEPARAMETER( CUpdateResourceCommand, IStudioRenderResource*, pResource, pResource,
										{
											pResource->ReleaseResource();
										} );
}

#endif // !ISTUDIORENDERRESOURCE_H