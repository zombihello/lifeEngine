#include "Logger/LoggerMacros.h"
#include "Misc/Template.h"
#include "D3D11RHI.h"
#include "D3D11State.h"

/*
==================
TranslateCullMode
==================
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

/*
==================
TranslateFillMode
==================
*/
static FORCEINLINE D3D11_FILL_MODE TranslateFillMode( ERasterizerFillMode InFillMode )
{
	switch ( InFillMode )
	{
	case FM_Wireframe:		return D3D11_FILL_WIREFRAME;
	default:				return D3D11_FILL_SOLID;
	};
}

/*
==================
TranslateAddressMode
==================
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

/*
==================
TranslateSamplerCompareFunction
==================
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

/*
==================
TranslateCompareFunction
==================
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

/*
==================
TranslateStencilOp
==================
*/
static FORCEINLINE D3D11_STENCIL_OP TranslateStencilOp( EStencilOp InStencilOp )
{
	switch ( InStencilOp )
	{
	case SO_Zero:				return D3D11_STENCIL_OP_ZERO;
	case SO_Replace:			return D3D11_STENCIL_OP_REPLACE;
	case SO_SaturatedIncrement: return D3D11_STENCIL_OP_INCR_SAT;
	case SO_SaturatedDecrement: return D3D11_STENCIL_OP_DECR_SAT;
	case SO_Invert:				return D3D11_STENCIL_OP_INVERT;
	case SO_Increment:			return D3D11_STENCIL_OP_INCR;
	case SO_Decrement:			return D3D11_STENCIL_OP_DECR;
	default:					return D3D11_STENCIL_OP_KEEP;
	};
}

/*
==================
TranslateMipBias
==================
*/
static FORCEINLINE float TranslateMipBias( ESamplerMipMapLODBias InMipBias )
{
	switch ( InMipBias )
	{
	case SMMLODBias_Get4:				return 0.f;
	default:							return ( float )InMipBias;
	};
}

/*
==================
TranslateBlendOp
==================
*/
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

/*
==================
TranslateBlendFactor
==================
*/
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


/*
==================
D3D11StateCache::D3D11StateCache
==================
*/
D3D11StateCache::D3D11StateCache()
	: inputLayout( nullptr )
	, vertexShader( nullptr )
	, pixelShader( nullptr )
	, geometryShader( nullptr )
	, hullShader( nullptr )
	, domainShader( nullptr )
	, rasterizerState( nullptr )
	, primitiveTopology( D3D_PRIMITIVE_TOPOLOGY_UNDEFINED )
	, depthStencilView( nullptr )
	, stencilRef( 0 )
{
	Reset();
}

/*
==================
D3D11StateCache::Reset
==================
*/
void D3D11StateCache::Reset()
{
	inputLayout = nullptr;
	vertexShader = nullptr;
	pixelShader = nullptr;
	geometryShader = nullptr;
	hullShader = nullptr;
	domainShader = nullptr;
	rasterizerState = nullptr;
	primitiveTopology = D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
	depthStencilView = nullptr;
	stencilRef = 0;

	Sys_Memzero( &vertexBuffers, sizeof( vertexBuffers ) );
	Sys_Memzero( &psSamplerStates, sizeof( psSamplerStates ) );
	Sys_Memzero( &psShaderResourceViews, sizeof( psShaderResourceViews ) );
	Sys_Memzero( &indexBuffer, sizeof( CD3D11StateIndexBuffer ) );
	Sys_Memzero( &renderTargetViews, sizeof( renderTargetViews ) );
	Sys_Memzero( &depthState, sizeof( D3D11_DEPTH_STENCIL_DESC ) );
	Sys_Memzero( &stencilState, sizeof( D3D11_DEPTH_STENCIL_DESC ) );
	Sys_Memzero( &blendState, sizeof( D3D11_BLEND_DESC ) );
	memset( &colorWriteMasks, CW_RGBA, sizeof( colorWriteMasks ) );
}


/*
==================
CD3D11RasterizerStateRHI::CD3D11RasterizerStateRHI
==================
*/
CD3D11RasterizerStateRHI::CD3D11RasterizerStateRHI( const RasterizerStateInitializerRHI& InInitializer ) 
	: CBaseRasterizerStateRHI( InInitializer )
	, d3d11RasterizerState( nullptr )
{
	D3D11_RASTERIZER_DESC			d3d11RasterizerDesc;
	Sys_Memzero( &d3d11RasterizerDesc, sizeof( D3D11_RASTERIZER_DESC ) );

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

	ID3D11Device*		d3d11Device = ( ( CD3D11RHI* )g_RHI )->GetD3D11Device();

#if ENABLED_ASSERT
	HRESULT				result = d3d11Device->CreateRasterizerState( &d3d11RasterizerDesc, &d3d11RasterizerState );
	Assert( result == S_OK );
#else
	d3d11Device->CreateRasterizerState( &d3d11RasterizerDesc, &d3d11RasterizerState );
#endif // ENABLED_ASSERT
}

/*
==================
CD3D11RasterizerStateRHI::~CD3D11RasterizerStateRHI
==================
*/
CD3D11RasterizerStateRHI::~CD3D11RasterizerStateRHI()
{
	d3d11RasterizerState->Release();
}


/*
==================
CD3D11SamplerStateRHI::CD3D11SamplerStateRHI
==================
*/
CD3D11SamplerStateRHI::CD3D11SamplerStateRHI( const SSamplerStateInitializerRHI& InInitializer ) :
	d3d11SamplerState( nullptr )
{
	D3D11_SAMPLER_DESC			d3d11SamplerDesc;
	Sys_Memzero( &d3d11SamplerDesc, sizeof( D3D11_SAMPLER_DESC ) );

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

	ID3D11Device*		d3d11Device = ( ( CD3D11RHI* )g_RHI )->GetD3D11Device();

#if ENABLED_ASSERT
	HRESULT				result = d3d11Device->CreateSamplerState( &d3d11SamplerDesc, &d3d11SamplerState );
	Assert(result == S_OK);
#else
	d3d11Device->CreateSamplerState( &d3d11SamplerDesc, &d3d11SamplerState );
#endif // ENABLED_ASSERT
}

/*
==================
CD3D11SamplerStateRHI::~CD3D11SamplerStateRHI
==================
*/
CD3D11SamplerStateRHI::~CD3D11SamplerStateRHI()
{
	d3d11SamplerState->Release();
}


/*
==================
CD3D11DepthStateRHI::CD3D11DepthStateRHI
==================
*/
CD3D11DepthStateRHI::CD3D11DepthStateRHI( const DepthStateInitializerRHI& InInitializer )
{
	Sys_Memzero( &d3d11DepthStateInfo, sizeof( D3D11_DEPTH_STENCIL_DESC ) );
	d3d11DepthStateInfo.DepthEnable		= InInitializer.depthTest != CF_Always || InInitializer.bEnableDepthWrite;
	d3d11DepthStateInfo.DepthWriteMask	= InInitializer.bEnableDepthWrite ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;
	d3d11DepthStateInfo.DepthFunc		= TranslateCompareFunction( InInitializer.depthTest );
}


/*
==================
CD3D11StencilStateRHI::CD3D11StencilStateRHI
==================
*/
CD3D11StencilStateRHI::CD3D11StencilStateRHI( const StencilStateInitializerRHI& InInitializer )
	: stencilRef( InInitializer.stencilRef )
{
	Sys_Memzero( &d3d11StencilStateInfo, sizeof( D3D11_DEPTH_STENCIL_DESC ) );
	d3d11StencilStateInfo.StencilEnable						= InInitializer.bEnableFrontFaceStencil || InInitializer.bEnableBackFaceStencil;
	d3d11StencilStateInfo.StencilReadMask					= InInitializer.stencilReadMask;
	d3d11StencilStateInfo.StencilWriteMask					= InInitializer.stencilWriteMask;
	d3d11StencilStateInfo.FrontFace.StencilFunc				= TranslateCompareFunction( InInitializer.frontFaceStencilTest );
	d3d11StencilStateInfo.FrontFace.StencilFailOp			= TranslateStencilOp( InInitializer.frontFaceStencilFailStencilOp );
	d3d11StencilStateInfo.FrontFace.StencilDepthFailOp		= TranslateStencilOp( InInitializer.frontFaceDepthFailStencilOp );
	d3d11StencilStateInfo.FrontFace.StencilPassOp			= TranslateStencilOp( InInitializer.frontFacePassStencilOp );
	
	if ( InInitializer.bEnableBackFaceStencil )
	{
		d3d11StencilStateInfo.BackFace.StencilFunc			= TranslateCompareFunction( InInitializer.backFaceStencilTest );
		d3d11StencilStateInfo.BackFace.StencilFailOp		= TranslateStencilOp( InInitializer.backFaceStencilFailStencilOp );
		d3d11StencilStateInfo.BackFace.StencilDepthFailOp	= TranslateStencilOp( InInitializer.backFaceDepthFailStencilOp );
		d3d11StencilStateInfo.BackFace.StencilPassOp		= TranslateStencilOp( InInitializer.backFacePassStencilOp );
	}
	else
	{
		d3d11StencilStateInfo.BackFace = d3d11StencilStateInfo.FrontFace;
	}
}


/*
==================
CD3D11BlendStateRHI::CD3D11BlendStateRHI
==================
*/
CD3D11BlendStateRHI::CD3D11BlendStateRHI( const BlendStateInitializerRHI& InInitializer, bool InIsColorWriteEnable /* = true */ )
	: bColorWrite( InIsColorWriteEnable )
{
	// Init descriptor
	Sys_Memzero( &d3d11blendStateInfo, sizeof( D3D11_BLEND_DESC ) );
	d3d11blendStateInfo.AlphaToCoverageEnable					= false;
	d3d11blendStateInfo.IndependentBlendEnable					= true;
	d3d11blendStateInfo.RenderTarget[0].BlendEnable				= InInitializer.colorBlendOperation != BO_Add || InInitializer.colorDestBlendFactor != BF_Zero || InInitializer.colorSourceBlendFactor != BF_One || InInitializer.alphaBlendOperation != BO_Add || InInitializer.alphaDestBlendFactor != BF_Zero || InInitializer.alphaSourceBlendFactor != BF_One;
	d3d11blendStateInfo.RenderTarget[0].BlendOp					= TranslateBlendOp( InInitializer.colorBlendOperation );
	d3d11blendStateInfo.RenderTarget[0].SrcBlend				= TranslateBlendFactor( InInitializer.colorSourceBlendFactor );
	d3d11blendStateInfo.RenderTarget[0].DestBlend				= TranslateBlendFactor( InInitializer.colorDestBlendFactor );
	d3d11blendStateInfo.RenderTarget[0].BlendOpAlpha			= TranslateBlendOp( InInitializer.alphaBlendOperation );
	d3d11blendStateInfo.RenderTarget[0].SrcBlendAlpha			= TranslateBlendFactor( InInitializer.alphaSourceBlendFactor );
	d3d11blendStateInfo.RenderTarget[0].DestBlendAlpha			= TranslateBlendFactor( InInitializer.alphaDestBlendFactor );
	d3d11blendStateInfo.RenderTarget[0].RenderTargetWriteMask	= InIsColorWriteEnable ? D3D11_COLOR_WRITE_ENABLE_ALL : 0;

	for ( uint32 renderTargetIndex = 1; renderTargetIndex < D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT; ++renderTargetIndex )
	{
		memcpy( &d3d11blendStateInfo.RenderTarget[renderTargetIndex], &d3d11blendStateInfo.RenderTarget[0], sizeof( d3d11blendStateInfo.RenderTarget[0] ) );
	}
}