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
  * @brief Reference to vertex shader
  */
typedef TRefCountPtr< class FBaseShaderRHI >			FVertexShaderRHIRef;

/**
 * @ingroup Engine Engine
 * @brief Pointer to vertex shader
 */
typedef class FBaseShaderRHI*							FVertexShaderRHIParamRef;

/**
 * @ingroup Engine Engine
 * @brief Reference to hull shader
 */
typedef TRefCountPtr< class FBaseShaderRHI >			FHullShaderRHIRef;

/**
 * @ingroup Engine Engine
 * @brief Pointer to hull shader
 */
typedef class FBaseShaderRHI*							FHullShaderRHIParamRef;

/**
 * @ingroup Engine Engine
 * @brief Reference to domain shader
 */
typedef TRefCountPtr< class FBaseShaderRHI >			FDomainShaderRHIRef;

/**
 * @ingroup Engine Engine
 * @brief Pointer to domain shader
 */
typedef class FBaseShaderRHI*							FDomainShaderRHIParamRef;

/**
 * @ingroup Engine Engine
 * @brief Reference to pixel shader
 */
typedef TRefCountPtr< class FBaseShaderRHI >			FPixelShaderRHIRef;

/**
 * @ingroup Engine Engine
 * @brief Pointer to pixel shader
 */
typedef class FBaseShaderRHI*							FPixelShaderRHIParamRef;

/**
 * @ingroup Engine Engine
 * @brief Reference to geometry shader
 */
typedef TRefCountPtr< class FBaseShaderRHI >			FGeometryShaderRHIRef;

/**
 * @ingroup Engine Engine
 * @brief Pointer to geometry shader
 */
typedef class FBaseShaderRHI*							FGeometryShaderRHIParamRef;

/**
 * @ingroup Engine Engine
 * @brief Reference to FBoundShaderStateRHI
 */
typedef TRefCountPtr< class FBaseBoundShaderStateRHI >	FBoundShaderStateRHIRef;

/**
 * @ingroup Engine Engine
 * @brief Pointer to FBoundShaderStateRHI
 */
typedef class FBaseBoundShaderStateRHI*					FBoundShaderStateRHIParamRef;

/**
 * @ingroup Engine Engine
 * @brief Reference to FBaseVertexBufferRHI
 */
typedef TRefCountPtr< class FBaseVertexBufferRHI >		FVertexBufferRHIRef;

/**
 * @ingroup Engine Engine
 * @brief Pointer to FBaseVertexBufferRHI
 */
typedef class FBaseVertexBufferRHI*						FVertexBufferRHIParamRef;

/**
 * @ingroup Engine Engine
 * @brief Reference to FBaseIndexBufferRHI
 */
typedef TRefCountPtr< class FBaseIndexBufferRHI >		FIndexBufferRHIRef;

/**
 * @ingroup Engine Engine
 * @brief Pointer to FBaseIndexBufferRHI
 */
typedef class FBaseIndexBufferRHI*						FIndexBufferRHIParamRef;

/**
 * @ingroup Engine Engine
 * @brief Reference to FBaseVertexDeclarationRHI
 */
typedef TRefCountPtr< class FBaseVertexDeclarationRHI >	FVertexDeclarationRHIRef;

/**
 * @ingroup Engine Engine
 * @brief Pointer to FBaseVertexDeclarationRHI
 */
typedef class FBaseVertexDeclarationRHI*				FVertexDeclarationRHIParamRef;

/**
 * @ingroup Engine Engine
 * @brief Reference to FBaseRasterizerStateRHI
 */
typedef TRefCountPtr< class FBaseRasterizerStateRHI >	FRasterizerStateRHIRef;

/**
 * @ingroup Engine Engine
 * @brief Pointer to FBaseRasterizerStateRHI
 */
typedef class FBaseRasterizerStateRHI*					FRasterizerStateRHIParamRef;

/**
 * @ingroup Engine Engine
 * Reference to FBaseSurfaceRHI
 */
typedef TRefCountPtr< class FBaseSurfaceRHI >			FSurfaceRHIRef;

/**
 * @ingroup Engine Engine
 * Pointer to FBaseSurfaceRHI
 */
typedef class FBaseSurfaceRHI*							FSurfaceRHIParamRef;

/**
 * @ingroup Engine Engine
 * Reference to base texture
 */
typedef TRefCountPtr< class FBaseTextureRHI >			FTextureRHIRef;

/**
 * @ingroup Engine Engine
 * Pointer to base texture
 */
typedef class FBaseTextureRHI*							FTextureRHIParamRef;

/**
 * @ingroup Engine Engine
 * Reference to texture 2D
 */
typedef TRefCountPtr< class FBaseTextureRHI >			FTexture2DRHIRef;

/**
 * @ingroup Engine Engine
 * Pointer to texture 2D
 */
typedef class FBaseTextureRHI*							FTexture2DRHIParamRef;

/**
 * @ingroup Engine Engine
 * Reference to RHI resource
 */
typedef TRefCountPtr< class FBaseResourceRHI >			FResourceRHIRef;

/**
 * @ingroup Engine Engine
 * Pointer to RHI resource
 */
typedef class FBaseResourceRHI*							FResourceRHIParamRef;

/**
 * @ingroup Engine Engine
 * Reference to sampler state
 */
typedef TRefCountPtr< class FBaseSamplerStateRHI >		FSamplerStateRHIRef;

/**
 * @ingroup Engine Engine
 * Pointer to sampler state
 */
typedef class FBaseSamplerStateRHI*						FSamplerStateRHIParamRef;

#endif // !TYPESRHI_H
