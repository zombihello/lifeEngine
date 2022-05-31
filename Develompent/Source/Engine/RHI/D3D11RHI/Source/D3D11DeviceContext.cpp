#include "Core.h"
#include "Math/Color.h"
#include "D3D11DeviceContext.h"
#include "D3D11Surface.h"

/**
 * Constructor
 */
CD3D11DeviceContext::CD3D11DeviceContext( ID3D11DeviceContext* InD3D11DeviceContext ) :
	d3d11DeviceContext( InD3D11DeviceContext )
{}

/**
 * Destructor
 */
CD3D11DeviceContext::~CD3D11DeviceContext()
{
	d3d11DeviceContext->Release();
	d3d11DeviceContext = nullptr;
}

/**
 * Clear surface
 */
void CD3D11DeviceContext::ClearSurface( SurfaceRHIParamRef_t InSurface, const class ÑColor& InColor )
{
	check( d3d11DeviceContext && InSurface );
	CD3D11Surface*			d3d11Surface = ( CD3D11Surface* )InSurface;

	float		clearColor[ 4 ] = { InColor.r / 255.f, InColor.g / 255.f, InColor.b / 255.f, InColor.a / 255.f };
	d3d11DeviceContext->ClearRenderTargetView( d3d11Surface->GetRenderTargetView(), clearColor );
}

void CD3D11DeviceContext::ClearDepthStencil( SurfaceRHIParamRef_t InSurface, bool InIsClearDepth /* = true */, bool InIsClearStencil /* = true */, float InDepthValue /* = 1.f */, uint8 InStencilValue /* = 0 */ )
{
	check( d3d11DeviceContext && InSurface );
	ID3D11DepthStencilView*			d3d11DepthStencilView = ( ( CD3D11Surface* )InSurface )->GetDepthStencilView();
	check( d3d11DepthStencilView );

	uint32			clearFlags = 0;
	if ( InIsClearDepth )
	{
		clearFlags |= D3D11_CLEAR_DEPTH;
	}
	
	if ( InIsClearStencil )
	{
		clearFlags |= D3D11_CLEAR_STENCIL;
	}

	d3d11DeviceContext->ClearDepthStencilView( d3d11DepthStencilView, clearFlags, InDepthValue, InStencilValue );
}