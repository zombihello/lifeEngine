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
 * @brief Enumeration of viewport type
 */
enum ELevelViewportType
{
	LVT_None = -1,			/**< None */
	LVT_OrthoXY,			/**< Ortho XY */
	LVT_OrthoXZ,			/**< Ortho XZ */
	LVT_OrthoYZ,			/**< Ortho YZ */
	LVT_Perspective,		/**< Perspective */
	LVT_Max					/**< Count viewport types */
};

/**
 * @ingroup Engine
 * An abstract interface to a viewport's client
 * The viewport's client processes input received by the viewport, and draws the viewport
 */
class CViewportClient
{
public:
	friend class CViewport;

	/**
	 * Constructor
	 */
	CViewportClient()
		: viewport( nullptr )
	{}

	/**
	 * Destructor
	 */
	virtual ~CViewportClient() {}

	/**
	 * Update logic of viewport client
	 */
	virtual void Tick( float InDeltaSeconds ) {}

	/**
	 * Draw viewport
	 *
	 * @param InViewport	Viewport
	 */
	virtual void Draw( class CViewport* InViewport ) {}

	/**
	 * @brief Process event
	 *
	 * @param InWindowEvent			Window event
	 */
	virtual void ProcessEvent( struct SWindowEvent& InWindowEvent ) {}

	/**
	 * @brief Get viewport
	 * @return Return owner viewport, if not exist returning NULL
	 */
	FORCEINLINE class CViewport* GetViewport() const
	{
		return viewport;
	}

private:
	class CViewport* viewport;		/**< Owner viewport */
};

/**
 * @ingroup Engine
 * Implementation for work with viewport
 */
class CViewport : public CRenderResource, public CRefCounted
{
public:
	/**
	 * Constructor
	 */
	CViewport();

	/**
	 * Destructor
	 */
	~CViewport();

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
	 * Update logic of viewport
	 */
	void Tick( float InDeltaSeconds );

	/**
	 * Set viewport client
	 * 
	 * @param InViewportClient		Viewport client
	 */
	FORCEINLINE void SetViewportClient( class CViewportClient* InViewportClient )
	{
		if ( viewportClient && viewportClient->viewport && viewportClient->viewport != this )
		{
			viewportClient->viewport->SetViewportClient( nullptr );
		}

		viewportClient = InViewportClient;
		if ( viewportClient )
		{
			viewportClient->viewport = this;
		}
	}

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
	ViewportRHIRef_t GetViewportRHI() const
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

	/**
	 * Get viewport client
	 * @return Return viewport client. If not exist return NULL
	 */
	FORCEINLINE class CViewportClient* GetViewportClient() const
	{
		return viewportClient;
	}

protected:
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
	 * @brief If the resource's RHI has been initialized, then release and reinitialize it.  Otherwise, do nothing.
	 * This is only called by the rendering thread.
	 */
	virtual void UpdateRHI() override;

private:
	void*						windowHandle;		/**< Handle to window for render */
	class CViewportClient*		viewportClient;		/**< Viewport client */
	uint32						sizeX;				/**< Width of viewport */
	uint32						sizeY;				/**< Height of viewport */
	ViewportRHIRef_t			viewportRHI;		/**< Pointer to viewport of RHI */
};

#endif // !VIEWPORT_H