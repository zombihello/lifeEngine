#include "D3D11Surface.h"

/**
 * Constructor
 */
FD3D11Surface::FD3D11Surface( ID3D11RenderTargetView* InRenderTargetView ) :
	d3d11RenderTargetView( InRenderTargetView )
{}

/**
 * Destructor
 */
FD3D11Surface::~FD3D11Surface()
{
	d3d11RenderTargetView->Release();
	d3d11RenderTargetView = nullptr;
}