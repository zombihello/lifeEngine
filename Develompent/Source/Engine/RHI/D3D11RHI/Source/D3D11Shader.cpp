#include "D3D11RHI.h"
#include "D3D11Shader.h"

/**
 * Constructor
 */
FD3D11ShaderRHI::FD3D11ShaderRHI( EShaderFrequency InFrequency, const byte* InData, uint32 InSize ) :
	FBaseShaderRHI( InFrequency )
{
	ID3D11Device*		device = static_cast< FD3D11RHI* >( GRHI )->GetD3D11Device();
	HRESULT				result = 0;

	// Creating DirectX shader
	switch ( InFrequency )
	{
	case SF_Vertex:
		result = device->CreateVertexShader( InData, InSize, nullptr, &shader.vertexShader );		
		break;

	case SF_Hull:
		result = device->CreateHullShader( InData, InSize, nullptr, &shader.hullShader );
		break;

	case SF_Domain:
		result = device->CreateDomainShader( InData, InSize, nullptr, &shader.domainShader );
		break;

	case SF_Pixel:
		result = device->CreatePixelShader( InData, InSize, nullptr, &shader.pixelShader );
		break;

	case SF_Geometry:
		result = device->CreateGeometryShader( InData, InSize, nullptr, &shader.geometryShader );
		break;

	default:
		appErrorf( TEXT( "Unsupported shader frequency %i" ), InFrequency );
		break;
	}

	check( result == S_OK );
}