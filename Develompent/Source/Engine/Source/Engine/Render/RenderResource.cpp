/**
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

#include <set>

#include "Engine/Render/RenderResource.h"
#include "Engine/Render/RenderingThread.h"
#include "Engine/Misc/EngineGlobals.h"
#include "RHI/BaseRHI.h"

/*
==================
GetGlobalResourcesMutex
==================
*/
static FORCEINLINE CThreadMutex& GetGlobalResourcesMutex()
{
	// Critical section of add/remove in CRenderResource::GetResourceList
	static CThreadMutex		globalResourcesMutex;
	return globalResourcesMutex;
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
		CThreadMutex&		mutex = GetGlobalResourcesMutex();
		mutex.Lock();
		GetResourceList().insert( this );
		mutex.Unlock();

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
		CThreadMutex&		mutex = GetGlobalResourcesMutex();
		mutex.Lock();
		GetResourceList().erase( this );
		mutex.Unlock();

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