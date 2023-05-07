#include <set>

#include "Render/RenderResource.h"
#include "Render/RenderingThread.h"
#include "Misc/EngineGlobals.h"
#include "RHI/BaseRHI.h"

/*
==================
GetGlobalResourcesCS
==================
*/
FORCEINLINE CCriticalSection& GetGlobalResourcesCS()
{
	// Critical section of add/remove in CRenderResource::GetResourceList
	static CCriticalSection		globalResourcesCS;
	return globalResourcesCS;
}

/*
==================
CRenderResource::CRenderResource
==================
*/
CRenderResource::CRenderResource() 
	: isInitialized( false )
	, bInGlobalList( false )
{}

/*
==================
CRenderResource::~CRenderResource
==================
*/
CRenderResource::~CRenderResource()
{
	if ( !isInitialized )
	{
		return;
	}

	// Deleting an initialized CRenderResource will result in a crash later since it is still linked
	//Sys_Errorf( TEXT( "An CRenderResource was deleted without being released first!" ) );
}

/*
==================
CRenderResource::GetResourceList
==================
*/
std::set< CRenderResource* >& CRenderResource::GetResourceList()
{
	static std::set< CRenderResource* >		globalResources;
	return globalResources;
}

/*
==================
CRenderResource::UpdateRHI
==================
*/
void CRenderResource::UpdateRHI()
{
	ReleaseRHI();
	InitRHI();
}

/*
==================
CRenderResource::InitResource
==================
*/
void CRenderResource::InitResource()
{
	if ( isInitialized )
	{
		return;
	}

	// If resource is global - add he to global list of resource
	if ( IsGlobal() && !bInGlobalList )
	{
		CCriticalSection&		criticalSection = GetGlobalResourcesCS();
		criticalSection.Lock();
		GetResourceList().insert( this );
		criticalSection.Unlock();

		bInGlobalList = true;
	}

	// Init resource if RHI is initialized
	if ( g_RHI && g_RHI->IsInitialize() )
	{
		InitRHI();
		isInitialized = true;
	}
}

/*
==================
CRenderResource::ReleaseResource
==================
*/
void CRenderResource::ReleaseResource()
{
	if ( !isInitialized )
	{
		return;
	}

	// If resource is global - remove he from global list of resource
	if ( IsGlobal() && bInGlobalList )
	{
		CCriticalSection&		criticalSection = GetGlobalResourcesCS();
		criticalSection.Lock();
		GetResourceList().erase( this );
		criticalSection.Unlock();

		bInGlobalList = false;
	}

	// Release resource if RHI is initialized
	if ( g_RHI && g_RHI->IsInitialize() )
	{
		ReleaseRHI();
		isInitialized = false;
	}
}

/*
==================
CRenderResource::UpdateResource
==================
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

/*
==================
CRenderResource::IsGlobal
==================
*/
bool CRenderResource::IsGlobal() const
{
	return false;
}


/*
==================
BeginInitResource
==================
*/
void BeginInitResource( CRenderResource* InResource )
{
	UNIQUE_RENDER_COMMAND_ONEPARAMETER( CInitResourceCommand, CRenderResource*, resource, InResource,
		{
			resource->InitResource();
		} );
}

/*
==================
BeginUpdateResource
==================
*/
void BeginUpdateResource( CRenderResource* InResource )
{
	UNIQUE_RENDER_COMMAND_ONEPARAMETER( CUpdateResourceCommand, CRenderResource*, resource, InResource,
		{
			resource->UpdateResource();
		} );
}

/*
==================
BeginReleaseResource
==================
*/
void BeginReleaseResource( CRenderResource* InResource )
{
	UNIQUE_RENDER_COMMAND_ONEPARAMETER( CReleaseResourceCommand, CRenderResource*, resource, InResource,
		{
			resource->ReleaseResource();
		} );
}