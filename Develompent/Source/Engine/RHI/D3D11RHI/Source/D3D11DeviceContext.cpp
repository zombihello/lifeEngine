#include "Core.h"
#include "Math/Color.h"
#include "D3D11DeviceContext.h"
#include "D3D11Surface.h"

/**
 * Constructor
 */
FD3D11DeviceContext::FD3D11DeviceContext( ID3D11DeviceContext* InD3D11DeviceContext ) :
	d3d11DeviceContext( InD3D11DeviceContext )
{}

/**
 * Destructor
 */
FD3D11DeviceContext::~FD3D11DeviceContext()
{
	d3d11DeviceContext->Release();
	d3d11DeviceContext = nullptr;
}

/**
 * Clear surface
 */
void FD3D11DeviceContext::ClearSurface( FSurfaceRHIParamRef InSurface, const class FColor& InColor )
{
	check( d3d11DeviceContext && InSurface );
	FD3D11Surface*			d3d11Surface = ( FD3D11Surface* )InSurface;

	float		clearColor[ 4 ] = { InColor.GetR() / 255.f, InColor.GetG() / 255.f, InColor.GetB() / 255.f, InColor.GetA() / 255.f };
	d3d11DeviceContext->ClearRenderTargetView( d3d11Surface->GetD3D11RenderTargetView(), clearColor );
}