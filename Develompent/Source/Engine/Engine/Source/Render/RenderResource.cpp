#include <set>

#include "Render/RenderResource.h"
#include "Render/RenderingThread.h"
#include "Misc/EngineGlobals.h"
#include "RHI/BaseRHI.h"

static std::set< FRenderResource* >				GGlobalResources;		/**< Global render resources */
FCriticalSection								GGlobalResourcesCS;		/**< Critical section of add/remove in GGlobalResources*/

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
	//appErrorf( TEXT( "An FRenderResource was deleted without being released first!" ) );
}

std::set< FRenderResource* >& FRenderResource::GetResourceList()
{
	return GGlobalResources;
}

/**
 * Update RHI resource
 */
void FRenderResource::UpdateRHI()
{
	ReleaseRHI();
	InitRHI();
}

/**
 * Initializes the resource
 */
void FRenderResource::InitResource()
{
	if ( isInitialized )
	{
		return;
	}

	// If resource is global - add he to global list of resource
	if ( IsGlobal() )
	{
		GGlobalResourcesCS.Lock();
		GGlobalResources.insert( this );
		GGlobalResourcesCS.Unlock();
	}

	// Init resource if RHI is initialized
	if ( GRHI && GRHI->IsInitialize() )
	{
		InitRHI();
	}

	isInitialized = true;
}

/**
 * Prepares the resource for deletion
 */
void FRenderResource::ReleaseResource()
{
	if ( !isInitialized )
	{
		return;
	}

	// If resource is global - remove he from global list of resource
	if ( IsGlobal() )
	{
		GGlobalResourcesCS.Lock();
		GGlobalResources.erase( this );
		GGlobalResourcesCS.Unlock();
	}

	// Release resource if RHI is initialized
	if ( GRHI && GRHI->IsInitialize() )
	{
		ReleaseRHI();
	}

	isInitialized = false;
}

/**
 * If the resource's RHI has been initialized, then release and reinitialize it.  Otherwise, do nothing
 */
void FRenderResource::UpdateResource()
{
	if ( !isInitialized )
	{
		InitResource();
	}
	else
	{
		UpdateRHI();
	}
}

bool FRenderResource::IsGlobal() const
{
	return false;
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
			resource->UpdateResource();
		} );
}

void BeginReleaseResource( FRenderResource* InResource )
{
	UNIQUE_RENDER_COMMAND_ONEPARAMETER( FReleaseResourceCommand, FRenderResource*, resource, InResource,
		{
			resource->ReleaseResource();
		} );
}