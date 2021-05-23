#include "Containers/StringConv.h"

#include "D3D11RHI.h"
#include "D3D11Shader.h"

/**
 * Constructor
 */
FD3D11ShaderRHI::FD3D11ShaderRHI( EShaderFrequency InFrequency, const byte* InData, uint32 InSize, const tchar* InShaderName ) :
	FBaseShaderRHI( InFrequency, InShaderName )
{
	ID3D11Device*		device = static_cast< FD3D11RHI* >( GRHI )->GetD3D11Device();
	HRESULT				result = 0;

	// Creating DirectX shader
	switch ( InFrequency )
	{
	case SF_Vertex:
		result = device->CreateVertexShader( InData, InSize, nullptr, &shader.vertexShader );
		D3D11SetDebugName( shader.vertexShader, TCHAR_TO_ANSI( InShaderName ) );
		break;

	case SF_Hull:
		result = device->CreateHullShader( InData, InSize, nullptr, &shader.hullShader );
		D3D11SetDebugName( shader.hullShader, TCHAR_TO_ANSI( InShaderName ) );
		break;

	case SF_Domain:
		result = device->CreateDomainShader( InData, InSize, nullptr, &shader.domainShader );
		D3D11SetDebugName( shader.domainShader, TCHAR_TO_ANSI( InShaderName ) );
		break;

	case SF_Pixel:
		result = device->CreatePixelShader( InData, InSize, nullptr, &shader.pixelShader );
		D3D11SetDebugName( shader.pixelShader, TCHAR_TO_ANSI( InShaderName ) );
		break;

	case SF_Geometry:
		result = device->CreateGeometryShader( InData, InSize, nullptr, &shader.geometryShader );
		D3D11SetDebugName( shader.geometryShader, TCHAR_TO_ANSI( InShaderName ) );
		break;

	default:
		appErrorf( TEXT( "Unsupported shader frequency %i" ), InFrequency );
		break;
	}

	check( result == S_OK );
}