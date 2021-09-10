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
 * @brief Structc for create resterize state in RHI
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
 * Structc for create sampler state in RHI
 */
struct FSamplerStateInitializerRHI
{
	ESamplerFilter			filter;						/**< Filter of sampler */
	ESamplerAddressMode		addressU;					/**< Address mode for U coord */
	ESamplerAddressMode		addressV;					/**< Address mode for V coord */
	ESamplerAddressMode		addressW;					/**< Address mode for W coord */
	float					mipBias;					/**< Mip bias */
	uint32					maxAnisotropy;				/**< Max anisotropy */
	FColor					borderColor;				/**< Border color */
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

#endif // !BASESTATERHI_H
