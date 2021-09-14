#include "Core.h"
#include "Misc/EngineGlobals.h"
#include "D3D11RHI.h"
#include "D3D11Viewport.h"
#include "D3D11Surface.h"

/**
 * Creates a D3D11Surface to represent a swap chain's back buffer
 */
static class FD3D11Surface* GetSwapChainSurface( IDXGISwapChain* InSwapChain )
{
	check( InSwapChain );

	// Grab the back buffer
	ID3D11Texture2D*		backBuffer = NULL;
	HRESULT result = InSwapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), ( void** ) &backBuffer );
	check( result == S_OK );

	// Create the render target view
	ID3D11RenderTargetView*				backBufferRenderTargetView = nullptr;
	D3D11_RENDER_TARGET_VIEW_DESC		rtvDesc;

	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Texture2D.MipSlice = 0;

	result = static_cast< FD3D11RHI* >( GRHI )->GetD3D11Device()->CreateRenderTargetView( backBuffer, &rtvDesc, &backBufferRenderTargetView );
	check( result == S_OK );
	
	D3D11SetDebugName( backBuffer, "SwapChainBackBuffer" );
	D3D11SetDebugName( backBufferRenderTargetView, "BackBuffer" );
	
	backBuffer->Release();
	return new FD3D11Surface( backBufferRenderTargetView );
}

/**
 * Constructor
 */
FD3D11Viewport::FD3D11Viewport( void* InWindowHandle, uint32 InWidth, uint32 InHeight ) :
	backBuffer( nullptr ),
	dxgiSwapChain( nullptr ),
	width( InWidth ),
	height( InHeight )
{
	check( GRHI );
	FD3D11RHI*				rhi = ( FD3D11RHI* )GRHI;
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
	check( result == S_OK );

	D3D11SetDebugName( dxgiSwapChain, "SwapChain" );
	result = dxgiFactory->MakeWindowAssociation( ( HWND )InWindowHandle, DXGI_MWA_NO_WINDOW_CHANGES );
	check( result == S_OK );

	// Create a RHI surface to represent the viewport's back buffer
	backBuffer = GetSwapChainSurface( dxgiSwapChain );
}

/**
 * Destructor
 */
FD3D11Viewport::~FD3D11Viewport()
{
	backBuffer.SafeRelease();
	dxgiSwapChain->Release();
	dxgiSwapChain = nullptr;
}

/**
 * Presents the swap chain
 */
void FD3D11Viewport::Present( bool InLockToVsync )
{
	check( dxgiSwapChain );
	dxgiSwapChain->Present( InLockToVsync ? 1 : 0, 0 );
}

void FD3D11Viewport::Resize( uint32 InWidth, uint32 InHeight )
{
	if ( width == InWidth && height == InHeight )
	{
		return;
	}

	// Release our backbuffer reference, as required by DXGI before calling ResizeBuffers.
	backBuffer.SafeRelease();
	check( dxgiSwapChain );

#if DO_CHECK
	HRESULT			result = dxgiSwapChain->ResizeBuffers( 1, InWidth, InHeight, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH );
	check( result == S_OK );
#else
	dxgiSwapChain->ResizeBuffers( 1, InWidth, InHeight, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH );
#endif // DO_CHECK

	width = InWidth;
	height = InHeight;
	backBuffer = GetSwapChainSurface( dxgiSwapChain );
}

/**
 * Get width
 */
uint32 FD3D11Viewport::GetWidth() const
{
	return width;
}

/**
 * Get height
 */
uint32 FD3D11Viewport::GetHeight() const
{
	return height;
}

/**
 * Get surface of viewport
 */
FSurfaceRHIRef FD3D11Viewport::GetSurface() const
{
	return backBuffer;
}