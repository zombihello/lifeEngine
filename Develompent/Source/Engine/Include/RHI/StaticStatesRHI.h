/**
 * @file
 * @addtogroup Engine Engine
 *
 * ************************************************************
 *                  This file is part of:
 *                      LIFEENGINE
 *          https://github.com/zombihello/lifeEngine
 * ************************************************************
 * Copyright (C) 2024 Yehor Pohuliaka.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef STATICSTATESRHI_H
#define STATICSTATESRHI_H

#include "Engine/Misc/EngineGlobals.h"
#include "Engine/Render/RenderResource.h"
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
		static CStaticStateResource		staticStateResource;
		return staticStateResource.GetRHI();
	};

private:
	/**
	 * @brief A resource which manages the RHI resource
	 */
	class CStaticStateResource : public CRenderResource
	{
	public:
		/**
		 * @brief Constructor
		 */
		CStaticStateResource()
		{
			InitResource();
		}

		/**
		 * @brief Destructor
		 */
		~CStaticStateResource()
		{
			ReleaseResource();
		}

		/**
		 * @brief Get RHI object
		 * @return Return RHI object
		 */
		FORCEINLINE TRHIRefType GetRHI() const
		{
			return stateRHI;
		}

	protected:
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

	private:
		TRHIRefType			stateRHI;		/**< RHI state reference */
	};
};

/**
 * @ingroup Engine
 * @brief A static RHI sampler state resource
 * @warning Should only be used from the rendering thread
 * 
 * TStaticSamplerStateRHI<...>::GetRHI() will return a SamplerStateRHIRef_t to a sampler state with the desired settings
 */
template<	ESamplerFilter			InFilter						= SF_Point, 
			ESamplerAddressMode		InAddressU						= SAM_Clamp, 
			ESamplerAddressMode		InAddressV						= SAM_Clamp, 
			ESamplerAddressMode		InAddressW						= SAM_Clamp, 
			ESamplerMipMapLODBias	InMipBias						= SMMLODBias_None,
			uint32					InMaxAnisotropy					= 0,
			uint32					InBorderColor					= 0,
			ESamplerCompareFunction InSamplerComparisonFunction		= SCF_Never >
class TStaticSamplerStateRHI : public TStaticStateRHI< TStaticSamplerStateRHI< InFilter, InAddressU, InAddressV, InAddressW, InMipBias, InMaxAnisotropy, InBorderColor, InSamplerComparisonFunction >, SamplerStateRHIRef_t, SamplerStateRHIParamRef_t >
{
public:
	/**
	 * @brief Create RHI object
	 * @return Return created RHI object
	 */
	static SamplerStateRHIRef_t CreateRHI()
	{
		SSamplerStateInitializerRHI		initializerRHI = 
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
		return g_RHI->CreateSamplerState( initializerRHI );
	}
};

/**
 * @ingroup Engine
 * @brief Typedef of linear sampler state RHI
 */
typedef TStaticSamplerStateRHI< SF_Bilinear, SAM_Wrap, SAM_Wrap, SAM_Wrap>		LinearSamplerStateRHI_t;

/**
 * @ingroup Engine
 * @brief A static RHI rasterizer state resource
 * @warning Should only be used from the rendering thread
 * 
 * TStaticRasterizerStateRHI<...>::GetRHI() will return a RasterizerStateRHIRef_t to a rasterizer state with the desired settings
 */
template<	ERasterizerFillMode		InFillMode		= FM_Solid,
			ERasterizerCullMode		InCullMode		= CM_None >
class TStaticRasterizerStateRHI : public TStaticStateRHI< TStaticRasterizerStateRHI< InFillMode, InCullMode >, RasterizerStateRHIRef_t, RasterizerStateRHIParamRef_t >
{
public:
	/**
	 * @brief Create RHI object
	 * @return Return created RHI object
	 */
	static RasterizerStateRHIRef_t CreateRHI()
	{
		RasterizerStateInitializerRHI initializerRHI = { InFillMode, InCullMode, 0.f, 0.f, true };
		return g_RHI->CreateRasterizerState( initializerRHI );
	}
};

/**
 * @ingroup Engine
 * @brief A static RHI depth state resource
 * @warning Should only be used from the rendering thread
 * 
 * TStaticDepthStateRHI<...>::GetRHI() will return a DepthStateRHIRef_t to a depth state with the desired settings
 */
template<	bool				InEnableDepthWrite	= true,
			ECompareFunction	InDepthTest			= CF_LessEqual >
class TStaticDepthStateRHI : public TStaticStateRHI< TStaticDepthStateRHI< InEnableDepthWrite, InDepthTest >, DepthStateRHIRef_t, DepthStateRHIParamRef_t >
{
public:
	/**
	 * @brief Create RHI object
	 * @return Return created RHI object
	 */
	static DepthStateRHIRef_t CreateRHI()
	{
		DepthStateInitializerRHI		initializerRHI = { InEnableDepthWrite, InDepthTest };
		return g_RHI->CreateDepthState( initializerRHI );
	}
};

/**
 * @ingroup Engine
 * @brief A static RHI blend state resource
 * @warning Should only be used from the rendering thread
 * 
 * TStaticBlendStateRHI<...>::GetRHI() will return a BlendStateRHIRef_t to a blend state with the desired settings
 */
template<	EBlendOperation		InColorBlendOp		= BO_Add,
			EBlendFactor		InColorSrcBlend		= BF_One,
			EBlendFactor		InColorDestBlend	= BF_Zero,
			EBlendOperation		InAlphaBlendOp		= BO_Add,
			EBlendFactor		InAlphaSrcBlend		= BF_One,
			EBlendFactor		InAlphaDestBlend	= BF_Zero,
			ECompareFunction	InAlphaTest			= CF_Always,
			byte				InAlphaRef			= 255 >
class TStaticBlendStateRHI : public TStaticStateRHI<TStaticBlendStateRHI<InColorBlendOp, InColorSrcBlend, InColorDestBlend, InAlphaBlendOp, InAlphaSrcBlend, InAlphaDestBlend, InAlphaTest, InAlphaRef>, BlendStateRHIRef_t, BlendStateRHIParamRef_t>
{
public:
	/**
	 * @brief Create RHI object
	 * @return Return created RHI object
	 */
	static BlendStateRHIRef_t CreateRHI()
	{
		BlendStateInitializerRHI		initializerRHI = { InColorBlendOp, InColorSrcBlend, InColorDestBlend, InAlphaBlendOp, InAlphaSrcBlend, InAlphaDestBlend, InAlphaTest, InAlphaRef };
		return g_RHI->CreateBlendState( initializerRHI );
	}
};

/**
 * @ingroup Engine
 * @brief A static RHI stencil state resource
 * @warning Should only be used from the rendering thread
 *
 * TStaticStencilStateRHI<...>::GetRHI() will return a StencilStateRHIRef_t to a stencil state with the desired settings
 */
template<	bool				InEnableFrontFaceStencil		= false,
			ECompareFunction	InFrontFaceStencilTest			= CF_Always,
			EStencilOp			InFrontFaceStencilFailStencilOp	= SO_Keep,
			EStencilOp			InFrontFaceDepthFailStencilOp	= SO_Keep,
			EStencilOp			InFrontFacePassStencilOp		= SO_Keep,
			bool				InEnableBackFaceStencil			= false,
			ECompareFunction	InBackFaceStencilTest			= CF_Always,
			EStencilOp			InBackFaceStencilFailStencilOp	= SO_Keep,
			EStencilOp			InBackFaceDepthFailStencilOp	= SO_Keep,
			EStencilOp			InBackFacePassStencilOp			= SO_Keep,
			uint32				InStencilReadMask				= 0xFFFFFFFF,
			uint32				InStencilWriteMask				= 0xFFFFFFFF,
			uint32				InStencilRef					= 0 >
class TStaticStencilStateRHI : public TStaticStateRHI<TStaticStencilStateRHI<InEnableFrontFaceStencil, InFrontFaceStencilTest, InFrontFaceStencilFailStencilOp, InFrontFaceDepthFailStencilOp, InFrontFacePassStencilOp, InEnableBackFaceStencil, InBackFaceStencilTest, InBackFaceStencilFailStencilOp, InBackFaceDepthFailStencilOp, InBackFacePassStencilOp, InStencilReadMask, InStencilWriteMask, InStencilRef>, StencilStateRHIRef_t, StencilStateRHIParamRef_t>
{
public:
	/**
	 * @brief Create RHI object
	 * @return Return created RHI object
	 */
	static StencilStateRHIRef_t CreateRHI()
	{
		StencilStateInitializerRHI		initializerRHI( InEnableFrontFaceStencil, InFrontFaceStencilTest, InFrontFaceStencilFailStencilOp, InFrontFaceDepthFailStencilOp, InFrontFacePassStencilOp, InEnableBackFaceStencil, InBackFaceStencilTest, InBackFaceStencilFailStencilOp, InBackFaceDepthFailStencilOp, InBackFacePassStencilOp, InStencilReadMask, InStencilWriteMask, InStencilRef );
		return g_RHI->CreateStencilState( initializerRHI );
	}
};

#endif // !STATICSTATESRHI_H