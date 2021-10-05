#include "Render/RenderResource.h"
#include "Render/RenderingThread.h"

/**
 * Constructor
 */
FRenderResource::FRenderResource() :
	isInitialized( false )
{}

/**
 * Destructor
 */
FRenderResource::~FRenderResource()
{
	if ( !isInitialized )
	{
		return;
	}

	// Deleting an initialized FRenderResource will result in a crash later since it is still linked
	appErrorf( TEXT( "An FRenderResource was deleted without being released first!" ) );
}

/**
 * Initializes the resource
 */
void FRenderResource::InitResource()
{
	if ( !isInitialized )
	{
		InitRHI();
		isInitialized = true;
	}
}

/**
 * Prepares the resource for deletion
 */
void FRenderResource::ReleaseResource()
{
	if ( isInitialized )
	{
		ReleaseRHI();
		isInitialized = false;
	}
}

/**
 * If the resource's RHI has been initialized, then release and reinitialize it.  Otherwise, do nothing
 */
void FRenderResource::UpdateRHI()
{
	if ( isInitialized )
	{
		ReleaseRHI();
	}

	InitRHI();
}

void BeginInitResource( FRenderResource* InResource )
{
	UNIQUE_RENDER_COMMAND_ONEPARAMETER( FInitResourceCommand, FRenderResource*, resource, InResource,
		{
			resource->InitResource();
		} );
}

void BeginUpdateResource( FRenderResource* InResource )
{
	UNIQUE_RENDER_COMMAND_ONEPARAMETER( FUpdateResourceCommand, FRenderResource*, resource, InResource,
		{
			resource->UpdateRHI();
		} );
}

void BeginReleaseResource( FRenderResource* InResource )
{
	UNIQUE_RENDER_COMMAND_ONEPARAMETER( FReleaseResourceCommand, FRenderResource*, resource, InResource,
		{
			resource->ReleaseResource();
		} );
}