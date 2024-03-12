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

#include "pch_studioapi.h"
#include "core/icommandline.h"
#include "studiorender/studioapi/dx11/dx11_studioviewport.h"
#include "studiorender/studioapi/dx11/dx11_studioapi.h"

CStudioAPIDx11		g_StudioAPIDx11;
EXPOSE_SINGLE_INTERFACE_GLOBALVAR( CStudioAPIDx11, IStudioAPI, STUDIOAPI_INTERFACE_VERSION, g_StudioAPIDx11 );


/*
==================
CStudioAPIDx11::CStudioAPIDx11
==================
*/
CStudioAPIDx11::CStudioAPIDx11()
	: bInited( false )
	, pDx11Device( nullptr )
	, pDx11DeviceContext( nullptr )
	, pDxgiFactory( nullptr )
	, pDxgiAdapter( nullptr )
{}

/*
==================
CStudioAPIDx11::Init
==================
*/
bool CStudioAPIDx11::Init( CreateInterfaceFn_t pFactory )
{
	// We cannot init the Studio API twice
	if ( bInited )
	{
		Error( "CStudioAPIDx11: Cannot initialize the Studio API twice!" );
	}

	// Connect StdLib and register cvars
	if ( !ConnectStdLib( pFactory ) )
	{
		return false;
	}
	ConVar_Register();

	uint32	deviceFlags = 0;

	// In Direct3D 11, if you are trying to create a hardware or a software device, set pAdapter != NULL which constrains the other inputs to be:
	//		DriverType must be D3D_DRIVER_TYPE_UNKNOWN 
	//		Software must be NULL.
	D3D_DRIVER_TYPE			driverType = D3D_DRIVER_TYPE_UNKNOWN;
	if ( CommandLine()->HasParam( "dxdebug" ) || DEBUG )
	{
		deviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
	}

	// Create DXGI factory and adapter
	HRESULT			result = CreateDXGIFactory( __uuidof( IDXGIFactory ), ( void** )&pDxgiFactory );
	if ( result != S_OK )
	{
		Warning( "CStudioAPIDx11: Failed to create DXGI factory" );
		return false;
	}

	// Find adapter
	uint32			currentAdapter = 0;
	while ( pDxgiFactory->EnumAdapters( currentAdapter, &pDxgiAdapter ) == DXGI_ERROR_NOT_FOUND )
	{
		++currentAdapter;
	}

	// If pDxgiAdapter then we not found a GPU adapter
	if ( !pDxgiAdapter )
	{
		Warning( "CStudioAPIDx11: GPU adapter not found" );
		return false;
	}

	// Create DX11 device
	D3D_FEATURE_LEVEL	maxFeatureLevel = D3D_FEATURE_LEVEL_11_0;
	result = D3D11CreateDevice( pDxgiAdapter, driverType, nullptr, deviceFlags, &maxFeatureLevel, 1, D3D11_SDK_VERSION, &pDx11Device, &dxFeatureLevel, &pDx11DeviceContext );
	if ( result != S_OK )
	{
		Warning( "CStudioAPIDx11: Failed to create DirectX 11 device" );
		return false;
	}

	// Print info adapter
	DXGI_ADAPTER_DESC	adapterDesc;
	pDxgiAdapter->GetDesc( &adapterDesc );

	Msg( "CStudioAPIDx11: Found adapter: %s", WCHAR_TO_ANSI( adapterDesc.Description ) );
	Msg( "CStudioAPIDx11: Adapter has %uMB of dedicated video memory, %uMB of dedicated system memory, and %uMB of shared system memory",
		 adapterDesc.DedicatedVideoMemory / ( 1024 * 1024 ),
		 adapterDesc.DedicatedSystemMemory / ( 1024 * 1024 ),
		 adapterDesc.SharedSystemMemory / ( 1024 * 1024 ) );

	// We are done!
	bInited = true;
	return true;
}

/*
==================
CStudioAPIDx11::Shutdown
==================
*/
void CStudioAPIDx11::Shutdown()
{
	// Destroy DirectX 11 device if it need
	if ( bInited )
	{
		Msg( "CStudioAPIDx11: DirectX 11 device destroyed" );
		pDx11DeviceContext->Release();
		pDx11Device->Release();
		pDxgiAdapter->Release();
		pDxgiFactory->Release();

		pDx11DeviceContext = nullptr;
		pDx11Device = nullptr;
		pDxgiAdapter = nullptr;
		pDxgiFactory = nullptr;
	}

	// Unregister cvars and disconnect StdLib
	ConVar_Unregister();
	DisconnectStdLib();
	bInited = false;
}

/*
==================
CStudioAPIDx11::AcquireThreadOwnership
==================
*/
void CStudioAPIDx11::AcquireThreadOwnership()
{}

/*
==================
CStudioAPIDx11::ReleaseThreadOwnership
==================
*/
void CStudioAPIDx11::ReleaseThreadOwnership()
{}

/*
==================
CStudioAPIDx11::CreateViewport
==================
*/
TRefPtr<IStudioViewport> CStudioAPIDx11::CreateViewport( windowHandle_t windowHandle, uint32 width, uint32 height ) const
{
	return new CStudioViewportDx11( windowHandle, width, height );
}

/*
==================
CStudioAPIDx11::CreateViewport
==================
*/
TRefPtr<IStudioViewport> CStudioAPIDx11::CreateViewport( IStudioSurface* pTargetSurface, uint32 width, uint32 height ) const
{
	return new CStudioViewportDx11( pTargetSurface, width, height );
}

/*
==================
CStudioAPIDx11::BeginDrawingViewport
==================
*/
void CStudioAPIDx11::BeginDrawingViewport( IStudioViewport* pViewport )
{
	SetRenderTarget( pViewport->GetSurface(), nullptr );
	SetViewport( 0, 0, 0.f, pViewport->GetWidth(), pViewport->GetHeight(), 1.f );
}

/*
==================
CStudioAPIDx11::EndDrawingViewport
==================
*/
void CStudioAPIDx11::EndDrawingViewport( IStudioViewport* pViewport, bool bPresent, bool bLockToVsync )
{
	if ( bPresent )
	{
		( ( CStudioViewportDx11* )pViewport )->Present( bLockToVsync );
	}
}

/*
==================
CStudioAPIDx11::SetRenderTarget
==================
*/
void CStudioAPIDx11::SetRenderTarget( IStudioSurface* pNewRenderTarget, IStudioSurface* pNewDepthStencilTarget )
{
	CStudioSurfaceDx11* pRenderTarget		= ( CStudioSurfaceDx11* )pNewRenderTarget;
	CStudioSurfaceDx11* pDepthStencilTarget = ( CStudioSurfaceDx11* )pNewDepthStencilTarget;

	// Set render target
	ID3D11RenderTargetView*		pDx11RenderTargetView = pRenderTarget ? pRenderTarget->GetDx11RenderTargetView() : nullptr;
	ID3D11DepthStencilView*		pDx11DepthStencilView = pDepthStencilTarget ? pDepthStencilTarget->GetDx11DepthStencilView() : nullptr;
	pDx11DeviceContext->OMSetRenderTargets( 1, &pDx11RenderTargetView, pDx11DepthStencilView );
}

/*
==================
CStudioAPIDx11::SetViewport
==================
*/
void CStudioAPIDx11::SetViewport( uint32 minX, uint32 minY, float minZ, uint32 maxX, uint32 maxY, float maxZ )
{
	D3D11_VIEWPORT			dx11Viewport = { ( float )minX, ( float )minY, ( float )maxX - minX, ( float )maxY - minY, ( float )minZ, maxZ };
	pDx11DeviceContext->RSSetViewports( 1, &dx11Viewport );
}


/*
==================
DX11_SetDebugName
==================
*/
void DX11_SetDebugName( ID3D11DeviceChild* pObject, const achar* pName )
{
	if ( pName )
	{
		pObject->SetPrivateData( WKPDID_D3DDebugObjectName, L_Strlen( pName ), pName );
	}
}

/*
==================
DX11_SetDebugName
==================
*/
void DX11_SetDebugName( IDXGIObject* pObject, const achar* pName )
{
	if ( pName )
	{
		pObject->SetPrivateData( WKPDID_D3DDebugObjectName, L_Strlen( pName ), pName );
	}
}