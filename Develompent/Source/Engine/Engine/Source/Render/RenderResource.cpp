#include <set>

#include "Render/RenderResource.h"
#include "Render/RenderingThread.h"
#include "Misc/EngineGlobals.h"
#include "RHI/BaseRHI.h"

/** Critical section of add/remove in CRenderResource::GetResourceList */
FORCEINLINE ÑCriticalSection& GetGlobalResourcesCS()
{
	static ÑCriticalSection		globalResourcesCS;
	return globalResourcesCS;
}

/**
 * Constructor
 */
CRenderResource::CRenderResource() :
	isInitialized( false )
{}

/**
 * Destructor
 */
CRenderResource::~CRenderResource()
{
	if ( !isInitialized )
	{
		return;
	}

	// Deleting an initialized CRenderResource will result in a crash later since it is still linked
	//appErrorf( TEXT( "An CRenderResource was deleted without being released first!" ) );
}

std::set< CRenderResource* >& CRenderResource::GetResourceList()
{
	static std::set< CRenderResource* >		globalResources;
	return globalResources;
}

/**
 * Update RHI resource
 */
void CRenderResource::UpdateRHI()
{
	ReleaseRHI();
	InitRHI();
}

/**
 * Initializes the resource
 */
void CRenderResource::InitResource()
{
	if ( isInitialized )
	{
		return;
	}

	// If resource is global - add he to global list of resource
	if ( IsGlobal() )
	{
		ÑCriticalSection&		criticalSection = GetGlobalResourcesCS();
		criticalSection.Lock();
		GetResourceList().insert( this );
		criticalSection.Unlock();
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
void CRenderResource::ReleaseResource()
{
	if ( !isInitialized )
	{
		return;
	}

	// If resource is global - remove he from global list of resource
	if ( IsGlobal() )
	{
		ÑCriticalSection&		criticalSection = GetGlobalResourcesCS();
		criticalSection.Lock();
		GetResourceList().erase( this );
		criticalSection.Unlock();
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
void CRenderResource::UpdateResource()
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

bool CRenderResource::IsGlobal() const
{
	return false;
}

void BeginInitResource( CRenderResource* InResource )
{
	UNIQUE_RENDER_COMMAND_ONEPARAMETER( CInitResourceCommand, CRenderResource*, resource, InResource,
		{
			resource->InitResource();
		} );
}

void BeginUpdateResource( CRenderResource* InResource )
{
	UNIQUE_RENDER_COMMAND_ONEPARAMETER( CUpdateResourceCommand, CRenderResource*, resource, InResource,
		{
			resource->UpdateResource();
		} );
}

void BeginReleaseResource( CRenderResource* InResource )
{
	UNIQUE_RENDER_COMMAND_ONEPARAMETER( CReleaseResourceCommand, CRenderResource*, resource, InResource,
		{
			resource->ReleaseResource();
		} );
}