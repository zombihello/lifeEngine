#include "D3D11Surface.h"

/**
 * Constructor
 */
D3D11Surface::D3D11Surface( ID3D11RenderTargetView* InRenderTargetView ) :
	d3d11RenderTargetView( InRenderTargetView )
{}

/**
 * Destructor
 */
D3D11Surface::~D3D11Surface()
{
	d3d11RenderTargetView->Release();
}