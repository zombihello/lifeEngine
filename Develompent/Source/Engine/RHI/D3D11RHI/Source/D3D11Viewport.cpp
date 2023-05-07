#include "Core.h"
#include "Misc/EngineGlobals.h"
#include "D3D11RHI.h"
#include "D3D11Viewport.h"
#include "D3D11Surface.h"

/*
==================
GetSwapChainSurface
==================
*/
static class CD3D11Surface* GetSwapChainSurface( IDXGISwapChain* InSwapChain )
{
	Assert( InSwapChain );

	// Grab the back buffer
	ID3D11Texture2D*		backBuffer = NULL;
	HRESULT result = InSwapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), ( void** ) &backBuffer );
	Assert( result == S_OK );

	// Create the render target view
	ID3D11RenderTargetView*				backBufferRenderTargetView = nullptr;
	D3D11_RENDER_TARGET_VIEW_DESC		rtvDesc;

	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Texture2D.MipSlice = 0;

	result = static_cast< CD3D11RHI* >( g_RHI )->GetD3D11Device()->CreateRenderTargetView( backBuffer, &rtvDesc, &backBufferRenderTargetView );
	Assert( result == S_OK );
	
	D3D11SetDebugName( backBuffer, "SwapChainBackBuffer" );
	D3D11SetDebugName( backBufferRenderTargetView, "BackBuffer" );
	
	backBuffer->Release();
	return new CD3D11Surface( backBufferRenderTargetView );
}


/*
==================
CD3D11Viewport::CD3D11Viewport
==================
*/
CD3D11Viewport::CD3D11Viewport( WindowHandle_t InWindowHandle, uint32 InWidth, uint32 InHeight )
	: windowHandle( InWindowHandle )
	, backBuffer( nullptr )
	, dxgiSwapChain( nullptr )
	, width( InWidth )
	, height( InHeight )
{
	Assert( g_RHI );
	CD3D11RHI*				rhi = ( CD3D11RHI* )g_RHI;
	IDXGIFactory*			dxgiFactory = rhi->GetDXGIFactory();

	// Create the swapchain
	DXGI_SWAP_CHAIN_DESC			swapChainDesc;
	memset( &swapChainDesc, 0, sizeof( DXGI_SWAP_CHAIN_DESC ) );
	
	swapChainDesc.BufferCount = 1;
	swapChainDesc.BufferDesc.Width = InWidth;
	swapChainDesc.BufferDesc.Height = InHeight;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 75;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = ( HWND )InWindowHandle;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.Windowed = true;
	
	HRESULT		result = dxgiFactory->CreateSwapChain( rhi->GetD3D11Device(), &swapChainDesc, &dxgiSwapChain );
	Assert( result == S_OK );

	D3D11SetDebugName( dxgiSwapChain, "SwapChain" );
	result = dxgiFactory->MakeWindowAssociation( ( HWND )InWindowHandle, DXGI_MWA_NO_WINDOW_CHANGES );
	Assert( result == S_OK );

	// Create a RHI surface to represent the viewport's back buffer
	backBuffer = GetSwapChainSurface( dxgiSwapChain );
}

/*
==================
CD3D11Viewport::CD3D11Viewport
==================
*/
CD3D11Viewport::CD3D11Viewport( SurfaceRHIParamRef_t InTargetSurface, uint32 InWidth, uint32 InHeight )
	: windowHandle( nullptr )
	, backBuffer( InTargetSurface )
	, dxgiSwapChain( nullptr )
	, width( InWidth )
	, height( InHeight )
{}

/*
==================
CD3D11Viewport::~CD3D11Viewport
==================
*/
CD3D11Viewport::~CD3D11Viewport()
{
	backBuffer.SafeRelease();
	if ( dxgiSwapChain )
	{
		dxgiSwapChain->Release();
		dxgiSwapChain = nullptr;
	}
}

/*
==================
CD3D11Viewport::Present
==================
*/
void CD3D11Viewport::Present( bool InLockToVsync )
{
	if ( dxgiSwapChain )
	{
		dxgiSwapChain->Present( InLockToVsync ? 1 : 0, 0 );
	}
}

/*
==================
CD3D11Viewport::Resize
==================
*/
void CD3D11Viewport::Resize( uint32 InWidth, uint32 InHeight )
{
	if ( width == InWidth && height == InHeight )
	{
		return;
	}

	width = InWidth;
	height = InHeight;

	if ( dxgiSwapChain )
	{
		// Release our backbuffer reference, as required by DXGI before calling ResizeBuffers.
		backBuffer.SafeRelease();

#if ENABLED_ASSERT
		HRESULT			result = dxgiSwapChain->ResizeBuffers( 1, InWidth, InHeight, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH );
		Assert( result == S_OK );
#else
		dxgiSwapChain->ResizeBuffers( 1, InWidth, InHeight, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH );
#endif // ENABLED_ASSERT

		backBuffer = GetSwapChainSurface( dxgiSwapChain );
	}
}

/*
==================
CD3D11Viewport::SetSurface
==================
*/
void CD3D11Viewport::SetSurface( SurfaceRHIParamRef_t InSurfaceRHI )
{
	if ( !dxgiSwapChain )
	{
		backBuffer = InSurfaceRHI;
	}
}

/*
==================
CD3D11Viewport::GetWidth
==================
*/
uint32 CD3D11Viewport::GetWidth() const
{
	return width;
}

/*
==================
CD3D11Viewport::GetHeight
==================
*/
uint32 CD3D11Viewport::GetHeight() const
{
	return height;
}

/*
==================
CD3D11Viewport::GetSurface
==================
*/
SurfaceRHIRef_t CD3D11Viewport::GetSurface() const
{
	return backBuffer;
}

/*
==================
CD3D11Viewport::GetWindowHandle
==================
*/
WindowHandle_t CD3D11Viewport::GetWindowHandle() const
{
	return windowHandle;
}