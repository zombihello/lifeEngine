/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef STATICSTATESRHI_H
#define STATICSTATESRHI_H

#include "Misc/EngineGlobals.h"
#include "Render/RenderResource.h"
#include "RHI/BaseStateRHI.h"
#include "RHI/BaseRHI.h"

/**
 * @ingroup Engine
 * @brief The base class of the static RHI state classes.
 */
template< typename TInitializerType, typename TRHIRefType, typename RHIParamRefType >
class TStaticStateRHI
{
public:
	/**
	 * @brief Get RHI object
	 * @return Return RHI object
	 */
	static RHIParamRefType GetRHI()
	{
		static FStaticStateResource		staticStateResource;
		return staticStateResource.GetRHI();
	};

private:
	/**
	 * @brief A resource which manages the RHI resource
	 */
	class FStaticStateResource : public FRenderResource
	{
	public:
		/**
		 * @brief Constructor
		 */
		FStaticStateResource()
		{
			InitResource();
		}

		/**
		 * @brief Destructor
		 */
		~FStaticStateResource()
		{
			ReleaseResource();
		}

		/**
		 * @brief Initializes the RHI resources used by this resource.
		 * Called when the resource is initialized.
		 * This is only called by the rendering thread.
		 */
		virtual void InitRHI()
		{
			stateRHI = TInitializerType::CreateRHI();
		}

		/**
		 * @brief Releases the RHI resources used by this resource.
		 * Called when the resource is released.
		 * This is only called by the rendering thread.
		 */
		virtual void ReleaseRHI()
		{
			stateRHI.SafeRelease();
		}

		/**
		 * @brief Get RHI object
		 * @return Return RHI object
		 */
		FORCEINLINE TRHIRefType GetRHI() const
		{
			return stateRHI;
		}

	private:
		TRHIRefType			stateRHI;		/**< RHI state reference */
	};
};

/**
 * @ingroup Engine
 * @brief A static RHI sampler state resource
 * @warning Should only be used from the rendering thread
 * 
 * TStaticSamplerStateRHI<...>::GetRHI() will return a FSamplerStateRHIRef to a sampler state with the desired settings
 */
template<	ESamplerFilter			InFilter						= SF_Point, 
			ESamplerAddressMode		InAddressU						= SAM_Clamp, 
			ESamplerAddressMode		InAddressV						= SAM_Clamp, 
			ESamplerAddressMode		InAddressW						= SAM_Clamp, 
			ESamplerMipMapLODBias	InMipBias						= SMMLODBias_None,
			uint32					InMaxAnisotropy					= 0,
			uint32					InBorderColor					= 0,
			ESamplerCompareFunction InSamplerComparisonFunction		= SCF_Never >
class TStaticSamplerStateRHI : public TStaticStateRHI< TStaticSamplerStateRHI< InFilter, InAddressU, InAddressV, InAddressW, InMipBias, InMaxAnisotropy, InBorderColor, InSamplerComparisonFunction >, FSamplerStateRHIRef, FSamplerStateRHIParamRef >
{
public:
	/**
	 * @brief Create RHI object
	 * @return Return created RHI object
	 */
	static FSamplerStateRHIRef CreateRHI()
	{
		FSamplerStateInitializerRHI		initializerRHI = 
		{
			InFilter, 
			InAddressU, 
			InAddressV, 
			InAddressW, 
			InMipBias, 
			InMaxAnisotropy, 
			InBorderColor, 
			InSamplerComparisonFunction 
		};
		return GRHI->CreateSamplerState( initializerRHI );
	}
};

/**
 * @ingroup Engine
 * @brief Typedef of linear sampler state RHI
 */
typedef TStaticSamplerStateRHI< SF_Bilinear, SAM_Wrap, SAM_Wrap, SAM_Wrap>		FLinearSamplerStateRHI;

/**
 * @ingroup Engine
 * @brief A static RHI rasterizer state resource
 * @warning Should only be used from the rendering thread
 * 
 * TStaticRasterizerStateRHI<...>::GetRHI() will return a FRasterizerStateRHIRef to a rasterizer state with the desired settings
 */
template<	ERasterizerFillMode		InFillMode		= FM_Solid,
			ERasterizerCullMode		InCullMode		= CM_None >
class TStaticRasterizerStateRHI : public TStaticStateRHI< TStaticRasterizerStateRHI< InFillMode, InCullMode >, FRasterizerStateRHIRef, FRasterizerStateRHIParamRef >
{
public:
	/**
	 * @brief Create RHI object
	 * @return Return created RHI object
	 */
	static FRasterizerStateRHIRef CreateRHI()
	{
		FRasterizerStateInitializerRHI initializerRHI = { InFillMode, InCullMode, 0.f, 0.f, true };
		return GRHI->CreateRasterizerState( initializerRHI );
	}
};

/**
 * @ingroup Engine
 * @brief A static RHI depth state resource
 * @warning Should only be used from the rendering thread
 * 
 * TStaticDepthStateRHI<...>::GetRHI() will return a FDepthStateRHIRef to a depth state with the desired settings
 */
template<	bool				InEnableDepthWrite	= true,
			ECompareFunction	InDepthTest			= CF_LessEqual >
class TStaticDepthStateRHI : public TStaticStateRHI< TStaticDepthStateRHI< InEnableDepthWrite, InDepthTest >, FDepthStateRHIRef, FDepthStateRHIParamRef >
{
public:
	/**
	 * @brief Create RHI object
	 * @return Return created RHI object
	 */
	static FDepthStateRHIRef CreateRHI()
	{
		FDepthStateInitializerRHI		initializerRHI = { InEnableDepthWrite, InDepthTest };
		return GRHI->CreateDepthState( initializerRHI );
	}
};

#endif // !STATICSTATESRHI_H