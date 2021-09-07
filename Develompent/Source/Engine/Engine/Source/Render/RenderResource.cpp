#include "Render/RenderResource.h"

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
{}

/**
 * Prepares the resource for deletion
 */
void FRenderResource::ReleaseResource()
{}

/**
 * If the resource's RHI has been initialized, then release and reinitialize it.  Otherwise, do nothing
 */
void FRenderResource::UpdateRHI()
{}