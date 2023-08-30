/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright BSOD-Games, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef BASESTATERHI_H
#define BASESTATERHI_H

#include <string>

#include "Math/Color.h"
#include "RHI/BaseResourceRHI.h"
#include "System/Archive.h"

 /**
  * @ingroup Engine
  * @brief Enumeration of types fill mode
  */
enum ERasterizerFillMode
{
	FM_Point,			/**< Point */
	FM_Wireframe,		/**< Wireframe */
	FM_Solid			/**< Solid */
};

/**
 * @ingroup Engine
 * @brief Enumeration of types cull mode
 */
enum ERasterizerCullMode
{
	CM_None,				/**< Cull mode is disabled */
	CM_NoneReversed,		/**< In this mode enabled front counter clockwise */
	CM_CW,					/**< Cull back */
	CM_CCW					/**< Cull front */
};

/**
 * @ingroup Engine
 * @brief Enumeration of compare function
 */
enum ECompareFunction
{
	CF_Less,				/**< Less */
	CF_LessEqual,			/**< Less equal */
	CF_Greater,				/**< Greater */
	CF_GreaterEqual,		/**< Greater equal */
	CF_Equal,				/**< Equal */
	CF_NotEqual,			/**< Not equal */
	CF_Never,				/**< Never */
	CF_Always				/**< Always */
};

/**
 * @ingroup Engine
 * @brief Enumeration of blend operation
 */
enum EBlendOperation
{
	BO_Add,					/**< Add */
	BO_Subtract,			/**< Subtract */
	BO_Min,					/**< Min */
	BO_Max,					/**< Max */
	BO_ReverseSubtract		/**< Reveerse subtract */
};

/**
 * @ingroup Engine
 * @brief Enumeration of blend factor
 */
enum EBlendFactor
{
	BF_Zero,						/**< Zero */
	BF_One,							/**< One */
	BF_SourceColor,					/**< Source color */
	BF_InverseSourceColor,			/**< Inverse source color */
	BF_SourceAlpha,					/**< Source alpha */
	BF_InverseSourceAlpha,			/**< Inverse source alpha */
	BF_DestAlpha,					/**< Dest alpha */
	BF_InverseDestAlpha,			/**< Inverse dest alpha */
	BF_DestColor,					/**< Dest color */
	BF_InverseDestColor,			/**< Inverse dest color */
	BF_SourceAlphaSaturate,			/**< Source alpha saturate */
	BF_ConstantBlendColor,			/**< Constant blend color */
	BF_InverseConstantBlendColor,	/**< Inverse constant blend color */
	BF_Source1Color,				/**< Source 1 color */
	BF_InverseSource1Color,			/**< Inverse source 1 color */
	BF_Source1Alpha,				/**< Source 1 alpha */
	BF_InverseSource1Alpha			/**< Inverse source 1 alpha */
};

/**
 * @ingroup Engine
 * @brief Enumeration of stencil operation
 */
enum EStencilOp
{
	SO_Keep,					/**< Keep */
	SO_Zero,					/**< Zero */
	SO_Replace,					/**< Replace */
	SO_SaturatedIncrement,		/**< Saturated increment */
	SO_SaturatedDecrement,		/**< Saturated decrement */
	SO_Invert,					/**< Invert */
	SO_Increment,				/**< Increment */
	SO_Decrement				/**< Decrement */
};

/**
 * @ingroup Engine
 * @brief Enumeration of color write mask
 */
enum EColorWriteMask
{
	CW_Red		= 1 << 0,								/**< Red channel */
	CW_Green	= 1 << 1,								/**< Green channel */
	CW_Blue		= 1 << 2,								/**< Blue channel */
	CW_Alpha	= 1 << 3,								/**< Alpha channel */

	CW_RGB = CW_Red | CW_Green | CW_Blue,				/**< RGB combination */
	CW_RGBA = CW_Red | CW_Green | CW_Blue | CW_Alpha,	/**< RGBA combination */
};

/**
 * @ingroup Engine
 * @brief Struct for create resterize state in RHI
 */
struct RasterizerStateInitializerRHI
{
	ERasterizerFillMode			fillMode;				/**< Fill mode */
	ERasterizerCullMode			cullMode;				/**< Cull mode */
	float						depthBias;				/**< Depth bias */
	float						slopeScaleDepthBias;	/**< Slope scale depth bias */
	bool						isAllowMSAA;			/**< Is allow MSAA */

	/**
	 * Overload operator ==
	 */
	FORCEINLINE bool operator==( const RasterizerStateInitializerRHI& InRight ) const
	{
		return	fillMode == InRight.fillMode && 
				cullMode == InRight.cullMode &&
				depthBias == InRight.depthBias &&
				slopeScaleDepthBias == InRight.slopeScaleDepthBias &&
				isAllowMSAA == InRight.isAllowMSAA;
	}

	/**
	 * Overload operator !=
	 */
	FORCEINLINE bool operator!=( const RasterizerStateInitializerRHI& InRight ) const
	{
		return !( *this == InRight );
	}
};

/**
 * @ingroup Engine
 * @brief Struct for create depth state in RHI
 */
struct DepthStateInitializerRHI
{
	bool						bEnableDepthWrite;		/**< Is eneable depth write */
	ECompareFunction			depthTest;				/**< Depth test compare function */
};

/**
 * @ingroup Engine
 * @brief Struct for create blend state in RHI
 */
struct BlendStateInitializerRHI
{
	/**
	 * @brief Constructor
	 */
	BlendStateInitializerRHI()
	{}

	/**
	 * @brief Constructor
	 * 
	 * @param InColorBlendOp			Color blend operation
	 * @param InColorSourceBlendFactor	Color source blend factor
	 * @param InColorDestBlendFactor	Color dest blend factor
	 * @param InAlphaBlendOp			Alpha blend operation
	 * @param InAlphaSourceBlendFactor	Alpha source blend factor
	 * @param InAlphaDestBlendFactor	Alpha dest blend factor
	 * @param InAlphaTest				Alpha test
	 * @param InAlphaRef				Alpha reference
	 */
	BlendStateInitializerRHI( EBlendOperation InColorBlendOp, EBlendFactor InColorSourceBlendFactor, EBlendFactor InColorDestBlendFactor, EBlendOperation InAlphaBlendOp, EBlendFactor InAlphaSourceBlendFactor, EBlendFactor InAlphaDestBlendFactor, ECompareFunction InAlphaTest, byte InAlphaRef )
		: colorBlendOperation( InColorBlendOp )
		, colorSourceBlendFactor( InColorSourceBlendFactor )
		, colorDestBlendFactor( InColorDestBlendFactor )
		, alphaBlendOperation( InAlphaBlendOp )
		, alphaSourceBlendFactor( InAlphaSourceBlendFactor )
		, alphaDestBlendFactor( InAlphaDestBlendFactor )
		, alphaTest( InAlphaTest )
		, alphaRef( InAlphaRef )
	{}

	EBlendOperation		colorBlendOperation;		/**< Color blend operation */
	EBlendFactor		colorSourceBlendFactor;		/**< Color source blend factor */
	EBlendFactor		colorDestBlendFactor;		/**< Color dest blend factor */
	EBlendOperation		alphaBlendOperation;		/**< Alpha blend operation */
	EBlendFactor		alphaSourceBlendFactor;		/**< Alpha source blend factor */
	EBlendFactor		alphaDestBlendFactor;		/**< Alpha dest blend factor */
	ECompareFunction	alphaTest;					/**< Alpha test */
	byte				alphaRef;					/**< Alpha reference */
};

/**
 * @ingroup Engine
 * @brief Struct for create stencil state in RHI
 */
struct StencilStateInitializerRHI
{
	/**
	 * @brief Constructor
	 */
	StencilStateInitializerRHI( bool InEnableFrontFaceStencil = false, ECompareFunction InFrontFaceStencilTest = CF_Always, EStencilOp InFrontFaceStencilFailStencilOp = SO_Keep, EStencilOp InFrontFaceDepthFailStencilOp = SO_Keep, EStencilOp InFrontFacePassStencilOp  = SO_Keep, bool InEnableBackFaceStencil = false, ECompareFunction InBackFaceStencilTest = CF_Always, EStencilOp InBackFaceStencilFailStencilOp = SO_Keep, EStencilOp InBackFaceDepthFailStencilOp  = SO_Keep, EStencilOp InBackFacePassStencilOp = SO_Keep, uint32 InStencilReadMask = 0xFFFFFFFF, uint32 InStencilWriteMask = 0xFFFFFFFF, uint32 InStencilRef = 0 )
		: bEnableFrontFaceStencil( InEnableFrontFaceStencil )
		, frontFaceStencilTest( InFrontFaceStencilTest )
		, frontFaceStencilFailStencilOp( InFrontFaceStencilFailStencilOp )
		, frontFaceDepthFailStencilOp( InFrontFaceDepthFailStencilOp )
		, frontFacePassStencilOp( InFrontFacePassStencilOp )
		, bEnableBackFaceStencil( InEnableBackFaceStencil )
		, backFaceStencilTest( InBackFaceStencilTest)
		, backFaceStencilFailStencilOp( InBackFaceStencilFailStencilOp )
		, backFaceDepthFailStencilOp( InBackFaceDepthFailStencilOp )
		, backFacePassStencilOp( InBackFacePassStencilOp )
		, stencilReadMask( InStencilReadMask )
		, stencilWriteMask( InStencilWriteMask )
		, stencilRef( InStencilRef )
	{}

	bool				bEnableFrontFaceStencil;			/**< Is enable front face stencil */
	ECompareFunction	frontFaceStencilTest;				/**< Front face stencil test */
	EStencilOp			frontFaceStencilFailStencilOp;		/**< Front face stencil fail stencil operation */
	EStencilOp			frontFaceDepthFailStencilOp;		/**< Front face depth fail stencil operation */
	EStencilOp			frontFacePassStencilOp;				/**< Front face pass stencil operation */
	bool				bEnableBackFaceStencil;				/**< Is enable back face stencil */
	ECompareFunction	backFaceStencilTest;				/**< Back face stencil test */
	EStencilOp			backFaceStencilFailStencilOp;		/**< Back face stencil fail stencil operation */
	EStencilOp			backFaceDepthFailStencilOp;			/**< Back face depth fail stencil operation */
	EStencilOp			backFacePassStencilOp;				/**< Back face pass stencil operation */
	uint32				stencilReadMask;					/**< Stencil read mask */
	uint32				stencilWriteMask;					/**< Stencil write mask */
	uint32				stencilRef;							/**< Stencil reference */
};

/**
 * @ingroup Engine
 * @brief Base class of resterize state
 */
class CBaseRasterizerStateRHI : public CBaseResourceRHI
{
public:
	/**
	 * @brief Constructor
	 * @param InInitializer		Initializer of rasterizer state
	 */
	FORCEINLINE CBaseRasterizerStateRHI( const RasterizerStateInitializerRHI& InInitializer )
		: initializer( InInitializer )
	{}

	/**
	 * @brief Destructor
	 */
	virtual ~CBaseRasterizerStateRHI()
	{}

	/**
	 * @brief Get initializer of rasterizer state
	 * @return Return initializer of rasterizer state
	 */
	FORCEINLINE RasterizerStateInitializerRHI GetInitializer() const
	{
		return initializer;
	}

protected:
	RasterizerStateInitializerRHI		initializer;		/**< Initializer of rasterizer state */
};

/**
 * @ingroup Engine
 * Enumeration of type filters for sampler
 */
enum ESamplerFilter
{
	SF_Point,
	SF_Bilinear,
	SF_Trilinear,
	SF_AnisotropicPoint,
	SF_AnisotropicLinear,
	SF_Max
};

/**
 * @ingroup Engine
 * Enumeration of type address mode for sampler
 */
enum ESamplerAddressMode
{
	SAM_Wrap,
	SAM_Clamp,
	SAM_Mirror,
	SAM_Border,
	SAM_Max
};

/**
 * @ingroup Engine
 * Convert sampler filter to text
 * 
 * @param InSamplerFilter		Sampler filter
 */
FORCEINLINE std::wstring SamplerFilterToText( ESamplerFilter InSamplerFilter )
{
	switch ( InSamplerFilter )
	{
	case SF_Point:					return TEXT( "Point" );
	case SF_Bilinear:				return TEXT( "Bilinear" );
	case SF_Trilinear:				return TEXT( "Trilinear" );
	case SF_AnisotropicPoint:		return TEXT( "Anisotropic Point" );
	case SF_AnisotropicLinear:		return TEXT( "Anisotropic Linear" );
	default:						return TEXT( "Unknown" );
	}
}

/**
 * @ingroup Engine
 * Convert address mode for sampler to text
 *
 * @param InAddressMode		Address mode
 */
FORCEINLINE std::wstring SamplerAddressModeToText( ESamplerAddressMode InAddressMode )
{
	switch ( InAddressMode )
	{
	case SAM_Wrap:			return TEXT( "Wrap" );
	case SAM_Clamp:			return TEXT( "Clamp" );
	case SAM_Mirror:		return TEXT( "Mirror" );
	case SAM_Border:		return TEXT( "Border" );
	default:				return TEXT( "Unknown" );
	}
}

/**
 * @ingroup Engine
 * Enumeration of modes compare function in sampler
 */
enum ESamplerCompareFunction
{
	SCF_Never,
	SCF_Less
};

/**
 * @ingroup Engine
 * Enumeration of mip map LOD bias
 */
enum ESamplerMipMapLODBias
{
	SMMLODBias_None					= 0,		/**< No LOD bias */
	SMMLODBias_HigherResolution_1	= -1,		/**< Higher resolution 1 */
	SMMLODBias_HigherResolution_2	= -2,		/**< Higher resolution 2 */
	SMMLODBias_HigherResolution_3	= -3,		/**< Higher resolution 3 */
	SMMLODBias_HigherResolution_4	= -4,		/**< Higher resolution 4 */
	SMMLODBias_HigherResolution_5	= -5,		/**< Higher resolution 5 */
	SMMLODBias_HigherResolution_6	= -6,		/**< Higher resolution 6 */
	SMMLODBias_HigherResolution_7	= -7,		/**< Higher resolution 7 */
	SMMLODBias_HigherResolution_8	= -8,		/**< Higher resolution 8 */
	SMMLODBias_HigherResolution_9	= -9,		/**< Higher resolution 9 */
	SMMLODBias_HigherResolution_10	= -10,		/**< Higher resolution 10 */
	SMMLODBias_HigherResolution_11	= -11,		/**< Higher resolution 11 */
	SMMLODBias_HigherResolution_12	= -12,		/**< Higher resolution 12 */
	SMMLODBias_HigherResolution_13	= -13,		/**< Higher resolution 13 */
	SMMLODBias_LowerResolution_1	= 1,		/**< Lower resolution 1 */
	SMMLODBias_LowerResolution_2	= 2,		/**< Lower resolution 2 */
	SMMLODBias_LowerResolution_3	= 3,		/**< Lower resolution 3 */
	SMMLODBias_LowerResolution_4	= 4,		/**< Lower resolution 4 */
	SMMLODBias_LowerResolution_5	= 5,		/**< Lower resolution 5 */
	SMMLODBias_LowerResolution_6	= 6,		/**< Lower resolution 6 */
	SMMLODBias_LowerResolution_7	= 7,		/**< Lower resolution 7 */
	SMMLODBias_LowerResolution_8	= 8,		/**< Lower resolution 8 */
	SMMLODBias_LowerResolution_9	= 9,		/**< Lower resolution 9 */
	SMMLODBias_LowerResolution_10	= 10,		/**< Lower resolution 10 */
	SMMLODBias_LowerResolution_11	= 11,		/**< Lower resolution 11 */
	SMMLODBias_LowerResolution_12	= 12,		/**< Lower resolution 12 */
	SMMLODBias_LowerResolution_13	= 13,		/**< Lower resolution 13 */
	SMMLODBias_Get4					= 100		/**< Get 4 */
};

/**
 * @ingroup Engine
 * Structc for create sampler state in RHI
 */
struct SSamplerStateInitializerRHI
{
	ESamplerFilter			filter;						/**< Filter of sampler */
	ESamplerAddressMode		addressU;					/**< Address mode for U coord */
	ESamplerAddressMode		addressV;					/**< Address mode for V coord */
	ESamplerAddressMode		addressW;					/**< Address mode for W coord */
	ESamplerMipMapLODBias	mipBias;					/**< Mip bias */
	uint32					maxAnisotropy;				/**< Max anisotropy */
	uint32					borderColor;				/**< Border color */
	ESamplerCompareFunction	comparisonFunction;			/**< Comparison function */
};

/**
 * @ingroup Engine
 * Base class of sampler state
 */
class CBaseSamplerStateRHI : public CBaseResourceRHI
{
public:
	/**
	 * Destructor
	 */
	virtual ~CBaseSamplerStateRHI()
	{}
};

/**
 * @ingroup Engine
 * Base class of depth state
 */
class CBaseDepthStateRHI : public CBaseResourceRHI
{
public:
	/**
	 * Destructor
	 */
	virtual ~CBaseDepthStateRHI()
	{}
};

/**
 * @ingroup Engine
 * Base class of blend state
 */
class CBaseBlendStateRHI : public CBaseResourceRHI
{
public:
	/**
	 * Destructor
	 */
	virtual ~CBaseBlendStateRHI()
	{}
};

/**
 * @ingroup Engine
 * Base class of stencil state
 */
class CBaseStencilStateRHI : public CBaseResourceRHI
{
public:
	/**
	 * Destructor
	 */
	virtual ~CBaseStencilStateRHI()
	{}
};

//
// Serialization
//

FORCEINLINE CArchive& operator<<( CArchive& InArchive, ESamplerAddressMode& InValue )
{
	InArchive.Serialize( &InValue, sizeof( InValue ) );
	return InArchive;
}

FORCEINLINE CArchive& operator<<( CArchive& InArchive, const ESamplerAddressMode& InValue )
{
	Assert( InArchive.IsSaving() );
	InArchive.Serialize( ( void* ) &InValue, sizeof( InValue ) );
	return InArchive;
}

FORCEINLINE CArchive& operator<<( CArchive& InArchive, ESamplerFilter& InValue )
{
	InArchive.Serialize( &InValue, sizeof( InValue ) );
	return InArchive;
}

FORCEINLINE CArchive& operator<<( CArchive& InArchive, const ESamplerFilter& InValue )
{
	Assert( InArchive.IsSaving() );
	InArchive.Serialize( ( void* ) &InValue, sizeof( InValue ) );
	return InArchive;
}

#endif // !BASESTATERHI_H
