#include "Core.h"
#include "Misc/EngineGlobals.h"
#include "D3D11RHI.h"
#include "D3D11ViewportRHI.h"

/**
 * Constructor
 */
D3D11ViewportRHI::D3D11ViewportRHI( void* InWindowHandle, uint32 InWidth, uint32 InHeight ) :
	d3d11RenderTargetView( nullptr ),
	dxgiSwapChain( nullptr )
{
	check( GRHI );
	D3D11RHI*				rhi = ( D3D11RHI* )GRHI;
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

	result = dxgiFactory->MakeWindowAssociation( ( HWND )InWindowHandle, DXGI_MWA_NO_WINDOW_CHANGES );
	check( result == S_OK );

	// BG yehor.pohuliaka - This for test, need implement D3D11Surface for abstraction
	ID3D11Texture2D*			backBuffer = NULL;
	result = dxgiSwapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), ( void** ) &backBuffer );

	result = rhi->GetD3D11Device()->CreateRenderTargetView( backBuffer, NULL, &d3d11RenderTargetView );
	backBuffer->Release();

	rhi->GetD3D11DeviceContext()->OMSetRenderTargets( 1, &d3d11RenderTargetView, nullptr );
}

/**
 * Destructor
 */
D3D11ViewportRHI::~D3D11ViewportRHI()
{
	// TODO BG yehor.pohuliaka - Add release objects
}

/**
 * Presents the swap chain
 */
void D3D11ViewportRHI::Present()
{
	check( dxgiSwapChain );
	dxgiSwapChain->Present( 0, 0 );
}