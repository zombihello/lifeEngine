#include <d3dcompiler.h>

#include "Containers/StringConv.h"

#include "Core.h"
#include "D3D11RHI.h"
#include "D3D11Shader.h"
#include "Containers/StringConv.h"
#include "Logger/LoggerMacros.h"
#include "Logger/BaseLogger.h"

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

/**
 * Constructor
 */
FD3D11VertexDeclarationRHI::FD3D11VertexDeclarationRHI( const FVertexDeclarationElementList& InElementList ) :
	FBaseVertexDeclarationRHI( InElementList ),
	streamCount( 0 )
{
	for ( uint32 elementIndex = 0, elementCount = ( uint32 )InElementList.size(); elementIndex < elementCount; ++elementIndex )
	{
		const FVertexElement&		element = InElementList[ elementIndex ];
		D3D11_INPUT_ELEMENT_DESC	d3dElement;
		d3dElement.InputSlot = element.streamIndex;
		d3dElement.AlignedByteOffset = element.offset;

		switch ( element.type )
		{
		case VET_Float1:		d3dElement.Format = DXGI_FORMAT_R32_FLOAT;														break;
		case VET_Float2:		d3dElement.Format = DXGI_FORMAT_R32G32_FLOAT;													break;
		case VET_Float3:		d3dElement.Format = DXGI_FORMAT_R32G32B32_FLOAT;												break;
		case VET_Float4:		d3dElement.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;												break;
		case VET_UByte4:		d3dElement.Format = DXGI_FORMAT_R8G8B8A8_UINT;													break;
		case VET_UByte4N:		d3dElement.Format = DXGI_FORMAT_R8G8B8A8_UNORM;													break;
		case VET_Color:			d3dElement.Format = DXGI_FORMAT_R8G8B8A8_UNORM;													break;
		default:				appErrorf( TEXT( "Unknown RHI vertex element type %u" ), InElementList[ elementIndex ].type );	break;
		}

		switch ( element.usage )
		{
		case VEU_Position:			d3dElement.SemanticName = "POSITION";		break;
		case VEU_TextureCoordinate:	d3dElement.SemanticName = "TEXCOORD";		break;
		case VEU_BlendWeight:		d3dElement.SemanticName = "BLENDWEIGHT";	break;
		case VEU_BlendIndices:		d3dElement.SemanticName = "BLENDINDICES";	break;
		case VEU_Normal:			d3dElement.SemanticName = "NORMAL";			break;
		case VEU_Tangent:			d3dElement.SemanticName = "TANGENT";		break;
		case VEU_Binormal:			d3dElement.SemanticName = "BINORMAL";		break;
		case VEU_Color:				d3dElement.SemanticName = "COLOR";			break;
		case VEU_SplitPos:			d3dElement.SemanticName = "SPLITPOS";		break;
		};

		d3dElement.SemanticIndex = element.usageIndex;
		d3dElement.InputSlotClass = element.isUseInstanceIndex ? D3D11_INPUT_PER_INSTANCE_DATA : D3D11_INPUT_PER_VERTEX_DATA;

		// This is a divisor to apply to the instance index used to read from this stream.
		d3dElement.InstanceDataStepRate = element.isUseInstanceIndex ? 1 : 0;

		vertexElements.push_back( d3dElement );
		streamCount = Max( streamCount, d3dElement.InputSlot );
	}
}

/**
 * Get hash of vertex declaration
 */
uint32 FD3D11VertexDeclarationRHI::GetHash( uint32 InHash /*= 0*/ ) const
{
	return appMemFastHash( vertexElements.data(), sizeof( D3D11_INPUT_ELEMENT_DESC ) * ( uint32 )vertexElements.size(), InHash );
}

/**
 * Constructor of FD3D11BoundShaderStateRHI
 */
FD3D11BoundShaderStateRHI::FD3D11BoundShaderStateRHI( const tchar* InDebugName, const FBoundShaderStateKey& InKey, FVertexDeclarationRHIRef InVertexDeclaration, FVertexShaderRHIRef InVertexShader, FPixelShaderRHIRef InPixelShader, FHullShaderRHIRef InHullShader /*= nullptr*/, FDomainShaderRHIRef InDomainShader /*= nullptr*/, FGeometryShaderRHIRef InGeometryShader /*= nullptr*/ ) :
	key( InKey ),
	d3d11InputLayout( nullptr ),
	vertexDeclaration( InVertexDeclaration ),
	vertexShader( InVertexShader ),
	pixelShader( InPixelShader ),
	hullShader( InHullShader ),
	domainShader( InDomainShader ),
	geometryShader( InGeometryShader )
{
	FD3D11RHI*		rhi = ( FD3D11RHI* )GRHI;
	check( rhi );

	ID3D11Device*	d3d11Device = rhi->GetD3D11Device();
	check( d3d11Device );

	const std::vector< D3D11_INPUT_ELEMENT_DESC >&		d3d11VertexElements = ( ( FD3D11VertexDeclarationRHI* )InVertexDeclaration.GetPtr() )->GetVertexElements();
	const std::vector< byte >&							vertexShaderBytecode = ( ( FD3D11VertexShaderRHI* )InVertexShader.GetPtr() )->GetCode();
	
	HRESULT			result = d3d11Device->CreateInputLayout( d3d11VertexElements.data(), ( uint32 )d3d11VertexElements.size(), vertexShaderBytecode.data(), vertexShaderBytecode.size(), &d3d11InputLayout );

#if !DO_CHECK
	check( result == S_OK );
#else
	if ( result != S_OK )
	{
		LE_LOG( LT_Warning, LC_Shader, TEXT( "d3d11Device->CreateInputLayout failed. Dumping the vertex-shader assembly and the vertex declaration.\n" ) );
		for ( uint32 index = 0, count = ( uint32 )d3d11VertexElements.size(); index < count; ++index )
		{
			const D3D11_INPUT_ELEMENT_DESC&			d3d11InputElementDesc = d3d11VertexElements[ index ];

			LE_LOG( LT_Warning, LC_Shader, TEXT( "VertexElements[%d]: SemanticName=%s SemanticIndex=%d InputSlot=%d AlignedByteOffset=%d" ),
											index,
											ANSI_TO_TCHAR( d3d11InputElementDesc.SemanticName ),
											d3d11InputElementDesc.SemanticIndex,
											d3d11InputElementDesc.InputSlot,
											d3d11InputElementDesc.AlignedByteOffset );
		}

		ID3D10Blob*			vertexShaderAssembly = nullptr;
		result = D3DDisassemble( vertexShaderBytecode.data(), vertexShaderBytecode.size(), false, nullptr, &vertexShaderAssembly );
		if ( result == S_OK )
		{
			LE_LOG( LT_Warning, LC_Shader, TEXT( "VertexShaderAssembly: %s" ), ANSI_TO_TCHAR( vertexShaderAssembly->GetBufferPointer() ) );
			vertexShaderAssembly->Release();
			vertexShaderAssembly = nullptr;
		}

		check( false );
	}
#endif // !DO_CHECK
}

/**
 * Destructor of FD3D11BoundShaderStateRHI
 */
FD3D11BoundShaderStateRHI::~FD3D11BoundShaderStateRHI()
{
	FD3D11RHI*		rhi = ( FD3D11RHI* )GRHI;
	check( rhi );

	rhi->GetBoundShaderStateHistory().Remove( key );
}