#include "Core.h"
#include "Math/Color.h"
#include "D3D11DeviceContext.h"
#include "D3D11Surface.h"

/**
 * Constructor
 */
D3D11DeviceContext::D3D11DeviceContext( ID3D11DeviceContext* InD3D11DeviceContext ) :
	d3d11DeviceContext( InD3D11DeviceContext )
{}

/**
 * Destructor
 */
D3D11DeviceContext::~D3D11DeviceContext()
{
	d3d11DeviceContext->Release();
	d3d11DeviceContext = nullptr;
}

/**
 * Clear surface
 */
void D3D11DeviceContext::ClearSurface( class BaseSurfaceRHI* InSurface, const class Color& InColor )
{
	check( d3d11DeviceContext && InSurface );
	D3D11Surface*			d3d11Surface = ( D3D11Surface* )InSurface;

	float		clearColor[ 4 ] = { InColor.GetR() / 255.f, InColor.GetG() / 255.f, InColor.GetB() / 255.f, InColor.GetA() / 255.f };
	d3d11DeviceContext->ClearRenderTargetView( d3d11Surface->GetD3D11RenderTargetView(), clearColor );
}