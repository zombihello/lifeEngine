/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright BSOD-Games, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef TYPESRHI_H
#define TYPESRHI_H

#include "Misc/RefCountPtr.h"

/**
 * @ingroup Engine Engine
 * @brief Reference to viewport
 */
typedef TRefCountPtr< class CBaseViewportRHI >			ViewportRHIRef_t;

/**
 * @ingroup Engine Engine
 * @brief Pointer to viewport
 */
typedef class CBaseViewportRHI*							ViewportRHIParamRef_t;

 /**
  * @ingroup Engine Engine
  * @brief Reference to vertex shader
  */
typedef TRefCountPtr< class CBaseShaderRHI >			VertexShaderRHIRef_t;

/**
 * @ingroup Engine Engine
 * @brief Pointer to vertex shader
 */
typedef class CBaseShaderRHI*							VertexShaderRHIParamRef_t;

/**
 * @ingroup Engine Engine
 * @brief Reference to hull shader
 */
typedef TRefCountPtr< class CBaseShaderRHI >			HullShaderRHIRef_t;

/**
 * @ingroup Engine Engine
 * @brief Pointer to hull shader
 */
typedef class CBaseShaderRHI*							HullShaderRHIParamRef_t;

/**
 * @ingroup Engine Engine
 * @brief Reference to domain shader
 */
typedef TRefCountPtr< class CBaseShaderRHI >			DomainShaderRHIRef_t;

/**
 * @ingroup Engine Engine
 * @brief Pointer to domain shader
 */
typedef class CBaseShaderRHI*							DomainShaderRHIParamRef_t;

/**
 * @ingroup Engine Engine
 * @brief Reference to pixel shader
 */
typedef TRefCountPtr< class CBaseShaderRHI >			PixelShaderRHIRef_t;

/**
 * @ingroup Engine Engine
 * @brief Pointer to pixel shader
 */
typedef class CBaseShaderRHI*							PixelShaderRHIParamRef_t;

/**
 * @ingroup Engine Engine
 * @brief Reference to geometry shader
 */
typedef TRefCountPtr< class CBaseShaderRHI >			GeometryShaderRHIRef_t;

/**
 * @ingroup Engine Engine
 * @brief Pointer to geometry shader
 */
typedef class CBaseShaderRHI*							GeometryShaderRHIParamRef_t;

/**
 * @ingroup Engine Engine
 * @brief Reference to FBoundShaderStateRHI
 */
typedef TRefCountPtr< class CBaseBoundShaderStateRHI >	BoundShaderStateRHIRef_t;

/**
 * @ingroup Engine Engine
 * @brief Pointer to FBoundShaderStateRHI
 */
typedef class CBaseBoundShaderStateRHI*					BoundShaderStateRHIParamRef_t;

/**
 * @ingroup Engine Engine
 * @brief Reference to CBaseVertexBufferRHI
 */
typedef TRefCountPtr< class CBaseVertexBufferRHI >		VertexBufferRHIRef_t;

/**
 * @ingroup Engine Engine
 * @brief Pointer to CBaseVertexBufferRHI
 */
typedef class CBaseVertexBufferRHI*						VertexBufferRHIParamRef_t;

/**
 * @ingroup Engine Engine
 * @brief Reference to CBaseIndexBufferRHI
 */
typedef TRefCountPtr< class CBaseIndexBufferRHI >		IndexBufferRHIRef_t;

/**
 * @ingroup Engine Engine
 * @brief Pointer to CBaseIndexBufferRHI
 */
typedef class CBaseIndexBufferRHI*						IndexBufferRHIParamRef_t;

/**
 * @ingroup Engine Engine
 * @brief Reference to CBaseVertexDeclarationRHI
 */
typedef TRefCountPtr< class CBaseVertexDeclarationRHI >	VertexDeclarationRHIRef_t;

/**
 * @ingroup Engine Engine
 * @brief Pointer to CBaseVertexDeclarationRHI
 */
typedef class CBaseVertexDeclarationRHI*				VertexDeclarationRHIParamRef_t;

/**
 * @ingroup Engine Engine
 * @brief Reference to CBaseRasterizerStateRHI
 */
typedef TRefCountPtr< class CBaseRasterizerStateRHI >	RasterizerStateRHIRef_t;

/**
 * @ingroup Engine Engine
 * @brief Pointer to CBaseRasterizerStateRHI
 */
typedef class CBaseRasterizerStateRHI*					RasterizerStateRHIParamRef_t;

/**
 * @ingroup Engine Engine
 * Reference to CBaseSurfaceRHI
 */
typedef TRefCountPtr< class CBaseSurfaceRHI >			SurfaceRHIRef_t;

/**
 * @ingroup Engine Engine
 * Pointer to CBaseSurfaceRHI
 */
typedef class CBaseSurfaceRHI*							SurfaceRHIParamRef_t;

/**
 * @ingroup Engine Engine
 * Reference to base texture
 */
typedef TRefCountPtr< class CBaseTextureRHI >			TextureRHIRef_t;

/**
 * @ingroup Engine Engine
 * Pointer to base texture
 */
typedef class CBaseTextureRHI*							TextureRHIParamRef_t;

/**
 * @ingroup Engine Engine
 * Reference to texture 2D
 */
typedef TRefCountPtr< class CBaseTextureRHI >			Texture2DRHIRef_t;

/**
 * @ingroup Engine Engine
 * Pointer to texture 2D
 */
typedef class CBaseTextureRHI*							Texture2DRHIParamRef_t;

/**
 * @ingroup Engine Engine
 * Reference to RHI resource
 */
typedef TRefCountPtr< class CBaseResourceRHI >			ResourceRHIRef_t;

/**
 * @ingroup Engine Engine
 * Pointer to RHI resource
 */
typedef class CBaseResourceRHI*							ResourceRHIParamRef_t;

/**
 * @ingroup Engine Engine
 * Reference to sampler state
 */
typedef TRefCountPtr< class CBaseSamplerStateRHI >		SamplerStateRHIRef_t;

/**
 * @ingroup Engine Engine
 * Pointer to sampler state
 */
typedef class CBaseSamplerStateRHI*						SamplerStateRHIParamRef_t;

/**
 * @ingroup Engine Engine
 * Reference to depth state
 */
typedef TRefCountPtr< class CBaseDepthStateRHI >		DepthStateRHIRef_t;

/**
 * @ingroup Engine Engine
 * Pointer to depth state
 */
typedef class CBaseDepthStateRHI*						DepthStateRHIParamRef_t;

/**
 * @ingroup Engine Engine
 * Reference to blend state
 */
typedef TRefCountPtr< class CBaseBlendStateRHI >		BlendStateRHIRef_t;

/**
 * @ingroup Engine Engine
 * Pointer to blend state
 */
typedef class CBaseBlendStateRHI*						BlendStateRHIParamRef_t;

/**
 * @ingroup Engine Engine
 * Reference to stencil state
 */
typedef TRefCountPtr< class CBaseStencilStateRHI >		StencilStateRHIRef_t;

/**
 * @ingroup Engine Engine
 * Pointer to stencil state
 */
typedef class CBaseStencilStateRHI*						StencilStateRHIParamRef_t;

#endif // !TYPESRHI_H
