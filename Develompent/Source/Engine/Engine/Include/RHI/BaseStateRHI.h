/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright BSOD-Games, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef BASESTATERHI_H
#define BASESTATERHI_H

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
 * @brief Struct for create resterize state in RHI
 */
struct FRasterizerStateInitializerRHI
{
	ERasterizerFillMode			fillMode;				/**< Fill mode */
	ERasterizerCullMode			cullMode;				/**< Cull mode */
	float						depthBias;				/**< Depth bias */
	float						slopeScaleDepthBias;	/**< Slope scale depth bias */
	bool						isAllowMSAA;			/**< Is allow MSAA */
};

/**
 * @ingroup Engine
 * @brief Struct for create depth state in RHI
 */
struct FDepthStateInitializerRHI
{
	bool						bEnableDepthWrite;		/**< Is eneable depth write */
	ECompareFunction			depthTest;				/**< Depth test compare function */
};

/**
 * @ingroup Engine
 * @brief Base class of resterize state
 */
class FBaseRasterizerStateRHI : public FBaseResourceRHI
{
public:
	/**
	 * @brief Destructor
	 */
	virtual ~FBaseRasterizerStateRHI()
	{}
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
	SF_AnisotropicLinear
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
	SAM_Border
};

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
struct FSamplerStateInitializerRHI
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
class FBaseSamplerStateRHI : public FBaseResourceRHI
{
public:
	/**
	 * Destructor
	 */
	virtual ~FBaseSamplerStateRHI()
	{}
};

/**
 * @ingroup Engine
 * Base class of depth state
 */
class FBaseDepthStateRHI : public FBaseResourceRHI
{
public:
	/**
	 * Destructor
	 */
	virtual ~FBaseDepthStateRHI()
	{}
};

//
// Serialization
//

FORCEINLINE FArchive& operator<<( FArchive& InArchive, ESamplerAddressMode& InValue )
{
	InArchive.Serialize( &InValue, sizeof( InValue ) );
	return InArchive;
}

FORCEINLINE FArchive& operator<<( FArchive& InArchive, const ESamplerAddressMode& InValue )
{
	check( InArchive.IsSaving() );
	InArchive.Serialize( ( void* ) &InValue, sizeof( InValue ) );
	return InArchive;
}

FORCEINLINE FArchive& operator<<( FArchive& InArchive, ESamplerFilter& InValue )
{
	InArchive.Serialize( &InValue, sizeof( InValue ) );
	return InArchive;
}

FORCEINLINE FArchive& operator<<( FArchive& InArchive, const ESamplerFilter& InValue )
{
	check( InArchive.IsSaving() );
	InArchive.Serialize( ( void* ) &InValue, sizeof( InValue ) );
	return InArchive;
}

#endif // !BASESTATERHI_H
