#include "Core.h"
#include "D3D11DeviceContext.h"

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
	check( d3d11DeviceContext );
	d3d11DeviceContext->Release();
}