#include "Logger/LoggerMacros.h"
#include "Misc/Template.h"
#include "D3D11RHI.h"
#include "D3D11State.h"

/**
 * Translate cull mode from engine to DirectX 11
 */
static FORCEINLINE D3D11_CULL_MODE TranslateCullMode( ERasterizerCullMode InCullMode )
{
	switch ( InCullMode )
	{
	case CM_CW:			return D3D11_CULL_BACK;
	case CM_CCW:		return D3D11_CULL_FRONT;
	default:			return D3D11_CULL_NONE;
	};
}

/**
 * Translate fill mode from engine to DirectX 11
 */
static FORCEINLINE D3D11_FILL_MODE TranslateFillMode( ERasterizerFillMode InFillMode )
{
	switch ( InFillMode )
	{
	case FM_Wireframe:		return D3D11_FILL_WIREFRAME;
	default:				return D3D11_FILL_SOLID;
	};
}

/**
 * Translate address mode from engine to DirectX 11
 */
static FORCEINLINE D3D11_TEXTURE_ADDRESS_MODE TranslateAddressMode( ESamplerAddressMode InAddressMode )
{
	switch ( InAddressMode )
	{
	case SAM_Clamp:		return D3D11_TEXTURE_ADDRESS_CLAMP;
	case SAM_Mirror:	return D3D11_TEXTURE_ADDRESS_MIRROR;
	case SAM_Border:	return D3D11_TEXTURE_ADDRESS_BORDER;
	default:			return D3D11_TEXTURE_ADDRESS_WRAP;
	};
}

/**
 * Translate sampler compare function from engine to DirectX 11
 */
static FORCEINLINE D3D11_COMPARISON_FUNC TranslateSamplerCompareFunction( ESamplerCompareFunction InSamplerComparisonFunction )
{
	switch ( InSamplerComparisonFunction )
	{
	case SCF_Less:		return D3D11_COMPARISON_LESS;
	case SCF_Never:
	default:			return D3D11_COMPARISON_NEVER;
	};
}

/**
 * Translate compare function from engine to DirectX 11
 */
static FORCEINLINE D3D11_COMPARISON_FUNC TranslateCompareFunction( ECompareFunction InCompareFunction )
{
	switch ( InCompareFunction )
	{
	case CF_Less:			return D3D11_COMPARISON_LESS;
	case CF_LessEqual:		return D3D11_COMPARISON_LESS_EQUAL;
	case CF_Greater:		return D3D11_COMPARISON_GREATER;
	case CF_GreaterEqual:	return D3D11_COMPARISON_GREATER_EQUAL;
	case CF_Equal:			return D3D11_COMPARISON_EQUAL;
	case CF_NotEqual:		return D3D11_COMPARISON_NOT_EQUAL;
	case CF_Never:			return D3D11_COMPARISON_NEVER;
	default:				return D3D11_COMPARISON_ALWAYS;
	};
}

static FORCEINLINE float TranslateMipBias( ESamplerMipMapLODBias InMipBias )
{
	switch ( InMipBias )
	{
	case SMMLODBias_Get4:				return 0.f;
	default:							return ( float )InMipBias;
	};
}

static FORCEINLINE D3D11_BLEND_OP TranslateBlendOp( EBlendOperation InBlendOp )
{
	switch ( InBlendOp )
	{
	case BO_Subtract:			return D3D11_BLEND_OP_SUBTRACT;
	case BO_ReverseSubtract:	return D3D11_BLEND_OP_REV_SUBTRACT;
	case BO_Min:				return D3D11_BLEND_OP_MIN;
	case BO_Max:				return D3D11_BLEND_OP_MAX;
	default:					return D3D11_BLEND_OP_ADD;
	};
}

static FORCEINLINE D3D11_BLEND TranslateBlendFactor( EBlendFactor InBlendFactor )
{
	switch ( InBlendFactor )
	{
	case BF_One:						return D3D11_BLEND_ONE;
	case BF_SourceColor:				return D3D11_BLEND_SRC_COLOR;
	case BF_InverseSourceColor:			return D3D11_BLEND_INV_SRC_COLOR;
	case BF_SourceAlpha:				return D3D11_BLEND_SRC_ALPHA;
	case BF_InverseSourceAlpha:			return D3D11_BLEND_INV_SRC_ALPHA;
	case BF_DestAlpha:					return D3D11_BLEND_DEST_ALPHA;
	case BF_InverseDestAlpha:			return D3D11_BLEND_INV_DEST_ALPHA;
	case BF_DestColor:					return D3D11_BLEND_DEST_COLOR;
	case BF_InverseDestColor:			return D3D11_BLEND_INV_DEST_COLOR;
	case BF_SourceAlphaSaturate:		return D3D11_BLEND_SRC_ALPHA_SAT;
	case BF_ConstantBlendColor:			return D3D11_BLEND_BLEND_FACTOR;
	case BF_InverseConstantBlendColor:	return D3D11_BLEND_INV_BLEND_FACTOR;
	case BF_Source1Color:				return D3D11_BLEND_SRC1_COLOR;
	case BF_InverseSource1Color:		return D3D11_BLEND_INV_SRC1_COLOR;
	case BF_Source1Alpha:				return D3D11_BLEND_SRC1_ALPHA;
	case BF_InverseSource1Alpha:		return D3D11_BLEND_INV_SRC1_ALPHA;
	default:							return D3D11_BLEND_ZERO;
	};
}

SD3D11StateCache::SD3D11StateCache()
	: inputLayout( nullptr )
	, vertexShader( nullptr )
	, pixelShader( nullptr )
	, geometryShader( nullptr )
	, hullShader( nullptr )
	, domainShader( nullptr )
	, rasterizerState( nullptr )
	, primitiveTopology( D3D_PRIMITIVE_TOPOLOGY_UNDEFINED )
	, depthStencilView( nullptr )
	, depthStencilState( nullptr )
	, bColorWrite( true )
{
	appMemzero( &vertexBuffers, sizeof( vertexBuffers ) );
	appMemzero( &psSamplerStates, sizeof( psSamplerStates ) );
	appMemzero( &psShaderResourceViews, sizeof( psShaderResourceViews ) );
	appMemzero( &indexBuffer, sizeof( CD3D11StateIndexBuffer ) );
	appMemzero( &renderTargetViews, sizeof( renderTargetViews ) );
}

/**
 * Constructor CD3D11RasterizerStateRHI
 */
CD3D11RasterizerStateRHI::CD3D11RasterizerStateRHI( const SRasterizerStateInitializerRHI& InInitializer ) 
	: CBaseRasterizerStateRHI( InInitializer )
	, d3d11RasterizerState( nullptr )
{
	D3D11_RASTERIZER_DESC			d3d11RasterizerDesc;
	appMemzero( &d3d11RasterizerDesc, sizeof( D3D11_RASTERIZER_DESC ) );

	d3d11RasterizerDesc.CullMode					= TranslateCullMode( InInitializer.cullMode );
	d3d11RasterizerDesc.FillMode					= TranslateFillMode( InInitializer.fillMode );
	d3d11RasterizerDesc.SlopeScaledDepthBias		= InInitializer.slopeScaleDepthBias;
	d3d11RasterizerDesc.DepthBias					= ( int32 )floor( InInitializer.depthBias * ( float )( 1 << 24 ) );
	d3d11RasterizerDesc.DepthClipEnable				= true;
	d3d11RasterizerDesc.MultisampleEnable			= InInitializer.isAllowMSAA;

	if ( InInitializer.cullMode == CM_NoneReversed )
	{
		d3d11RasterizerDesc.FrontCounterClockwise	= false;
	}
	else
	{
		d3d11RasterizerDesc.FrontCounterClockwise	= true;
	}

	ID3D11Device*		d3d11Device = ( ( CD3D11RHI* )GRHI )->GetD3D11Device();

#if DO_CHECK
	HRESULT				result = d3d11Device->CreateRasterizerState( &d3d11RasterizerDesc, &d3d11RasterizerState );
	check( result == S_OK );
#else
	d3d11Device->CreateRasterizerState( &d3d11RasterizerDesc, &d3d11RasterizerState );
#endif // DO_CHECK
}

/**
 * Destructor CD3D11RasterizerStateRHI
 */
CD3D11RasterizerStateRHI::~CD3D11RasterizerStateRHI()
{
	d3d11RasterizerState->Release();
}

CD3D11SamplerStateRHI::CD3D11SamplerStateRHI( const SSamplerStateInitializerRHI& InInitializer ) :
	d3d11SamplerState( nullptr )
{
	D3D11_SAMPLER_DESC			d3d11SamplerDesc;
	appMemzero( &d3d11SamplerDesc, sizeof( D3D11_SAMPLER_DESC ) );

	d3d11SamplerDesc.AddressU		= TranslateAddressMode( InInitializer.addressU );
	d3d11SamplerDesc.AddressV		= TranslateAddressMode( InInitializer.addressV );
	d3d11SamplerDesc.AddressW		= TranslateAddressMode( InInitializer.addressW );
	d3d11SamplerDesc.MipLODBias		= TranslateMipBias( InInitializer.mipBias );
	d3d11SamplerDesc.MaxAnisotropy	= Clamp( InInitializer.maxAnisotropy, ( uint32 )1, ( uint32 )16 );
	d3d11SamplerDesc.MaxLOD			= D3D11_FLOAT32_MAX;

	// Determine whether we should use one of the 
	bool			comparisonEnabled = InInitializer.comparisonFunction != SCF_Never;
	switch ( InInitializer.filter )
	{
	case SF_AnisotropicLinear:
	case SF_AnisotropicPoint:
		// D3D11 doesn't allow using point filtering for mip filter when using anisotropic filtering
		d3d11SamplerDesc.Filter = comparisonEnabled ? D3D11_FILTER_COMPARISON_ANISOTROPIC : D3D11_FILTER_ANISOTROPIC;
		break;

	case SF_Trilinear:
		d3d11SamplerDesc.Filter = comparisonEnabled ? D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR : D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		break;

	case SF_Bilinear:
		d3d11SamplerDesc.Filter = comparisonEnabled ? D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT : D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
		break;

	case SF_Point:
		d3d11SamplerDesc.Filter = comparisonEnabled ? D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT : D3D11_FILTER_MIN_MAG_MIP_POINT;
		break;
	}

	CColor		borderColor( InInitializer.borderColor );
	d3d11SamplerDesc.BorderColor[ 0 ] = borderColor.r;
	d3d11SamplerDesc.BorderColor[ 1 ] = borderColor.g;
	d3d11SamplerDesc.BorderColor[ 2 ] = borderColor.b;
	d3d11SamplerDesc.BorderColor[ 3 ] = borderColor.a;
	d3d11SamplerDesc.ComparisonFunc = TranslateSamplerCompareFunction( InInitializer.comparisonFunction );

	ID3D11Device*		d3d11Device = ( ( CD3D11RHI* )GRHI )->GetD3D11Device();

#if DO_CHECK
	HRESULT				result = d3d11Device->CreateSamplerState( &d3d11SamplerDesc, &d3d11SamplerState );
	check(result == S_OK);
#else
	d3d11Device->CreateSamplerState( &d3d11SamplerDesc, &d3d11SamplerState );
#endif // DO_CHECK
}

CD3D11SamplerStateRHI::~CD3D11SamplerStateRHI()
{
	d3d11SamplerState->Release();
}

CD3D11DepthStateRHI::CD3D11DepthStateRHI( const SDepthStateInitializerRHI& InInitializer )
{
	// Init descriptor
	D3D11_DEPTH_STENCIL_DESC		d3d11DepthStencilDesc;
	appMemzero( &d3d11DepthStencilDesc, sizeof( D3D11_DEPTH_STENCIL_DESC ) );
	d3d11DepthStencilDesc.DepthEnable		= InInitializer.depthTest != CF_Always || InInitializer.bEnableDepthWrite;
	d3d11DepthStencilDesc.DepthWriteMask	= InInitializer.bEnableDepthWrite ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;
	d3d11DepthStencilDesc.DepthFunc			= TranslateCompareFunction( InInitializer.depthTest );

	// Create DirectX resource
	ID3D11Device*		d3d11Device = ( ( CD3D11RHI* )GRHI )->GetD3D11Device();

#if DO_CHECK
	HRESULT				result = d3d11Device->CreateDepthStencilState( &d3d11DepthStencilDesc, &d3d11DepthState );
	check( result == S_OK );
#else
	d3d11Device->CreateDepthStencilState( &d3d11DepthStencilDesc, &d3d11DepthState );
#endif // DO_CHECK
}

CD3D11DepthStateRHI::~CD3D11DepthStateRHI()
{
	d3d11DepthState->Release();
}

CD3D11BlendStateRHI::CD3D11BlendStateRHI( const SBlendStateInitializerRHI& InInitializer, bool InIsColorWriteEnable /* = true */ )
	: blendStateInfo( InInitializer )
	, bColorWrite( InIsColorWriteEnable )
{
	// Calculate hash
	hash = appMemFastHash( blendStateInfo );
	hash = appMemFastHash( bColorWrite, hash );

	// Init descriptor
	D3D11_BLEND_DESC		d3d11BlendDesc;
	appMemzero( &d3d11BlendDesc, sizeof( D3D11_BLEND_DESC ) );
	d3d11BlendDesc.AlphaToCoverageEnable					= false;
	d3d11BlendDesc.IndependentBlendEnable					= true;
	d3d11BlendDesc.RenderTarget[0].BlendEnable				= InInitializer.colorBlendOperation != BO_Add || InInitializer.colorDestBlendFactor != BF_Zero || InInitializer.colorSourceBlendFactor != BF_One || InInitializer.alphaBlendOperation != BO_Add || InInitializer.alphaDestBlendFactor != BF_Zero || InInitializer.alphaSourceBlendFactor != BF_One;
	d3d11BlendDesc.RenderTarget[0].BlendOp					= TranslateBlendOp( InInitializer.colorBlendOperation );
	d3d11BlendDesc.RenderTarget[0].SrcBlend					= TranslateBlendFactor( InInitializer.colorSourceBlendFactor );
	d3d11BlendDesc.RenderTarget[0].DestBlend				= TranslateBlendFactor( InInitializer.colorDestBlendFactor );
	d3d11BlendDesc.RenderTarget[0].BlendOpAlpha				= TranslateBlendOp( InInitializer.alphaBlendOperation );
	d3d11BlendDesc.RenderTarget[0].SrcBlendAlpha			= TranslateBlendFactor( InInitializer.alphaSourceBlendFactor );
	d3d11BlendDesc.RenderTarget[0].DestBlendAlpha			= TranslateBlendFactor( InInitializer.alphaDestBlendFactor );
	d3d11BlendDesc.RenderTarget[0].RenderTargetWriteMask	= InIsColorWriteEnable ? D3D11_COLOR_WRITE_ENABLE_ALL : 0;

	LE_LOG( LT_Warning, LC_RHI, TEXT( "AHTUNG! CD3D11BlendStateRHI::CD3D11BlendStateRHI :: Need implement color write mask" ) );

	for ( uint32 renderTargetIndex = 1; renderTargetIndex < D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT; ++renderTargetIndex )
	{
		memcpy( &d3d11BlendDesc.RenderTarget[renderTargetIndex], &d3d11BlendDesc.RenderTarget[0], sizeof( d3d11BlendDesc.RenderTarget[0] ) );		
	}

	// Create DirectX resource
	ID3D11Device*		d3d11Device = ( ( CD3D11RHI* )GRHI )->GetD3D11Device();

#if DO_CHECK
	HRESULT				result = d3d11Device->CreateBlendState( &d3d11BlendDesc, &d3d11BlendState );
	check( result == S_OK );
#else
	d3d11Device->CreateBlendState( &d3d11BlendDesc, &d3d11BlendState );
#endif // DO_CHECK
}

CD3D11BlendStateRHI::~CD3D11BlendStateRHI()
{
	d3d11BlendState->Release();
}