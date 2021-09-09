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
#include "BaseShaderRHI.h"
#include "BaseBufferRHI.h"
#include "BaseSurfaceRHI.h"

 /**
  * @ingroup Engine Engine
  * @brief Reference to vertex shader
  */
typedef TRefCountPtr< FBaseShaderRHI >					FVertexShaderRHIRef;

/**
 * @ingroup Engine Engine
 * @brief Pointer to vertex shader
 */
typedef FBaseShaderRHI*									FVertexShaderRHIParamRef;

/**
 * @ingroup Engine Engine
 * @brief Reference to hull shader
 */
typedef TRefCountPtr< FBaseShaderRHI >					FHullShaderRHIRef;

/**
 * @ingroup Engine Engine
 * @brief Pointer to hull shader
 */
typedef FBaseShaderRHI*									FHullShaderRHIParamRef;

/**
 * @ingroup Engine Engine
 * @brief Reference to domain shader
 */
typedef TRefCountPtr< FBaseShaderRHI >					FDomainShaderRHIRef;

/**
 * @ingroup Engine Engine
 * @brief Pointer to domain shader
 */
typedef FBaseShaderRHI*									FDomainShaderRHIParamRef;

/**
 * @ingroup Engine Engine
 * @brief Reference to pixel shader
 */
typedef TRefCountPtr< FBaseShaderRHI >					FPixelShaderRHIRef;

/**
 * @ingroup Engine Engine
 * @brief Pointer to pixel shader
 */
typedef FBaseShaderRHI*									FPixelShaderRHIParamRef;

/**
 * @ingroup Engine Engine
 * @brief Reference to geometry shader
 */
typedef TRefCountPtr< FBaseShaderRHI >					FGeometryShaderRHIRef;

/**
 * @ingroup Engine Engine
 * @brief Pointer to geometry shader
 */
typedef FBaseShaderRHI*									FGeometryShaderRHIParamRef;

/**
 * @ingroup Engine Engine
 * @brief Reference to FBoundShaderStateRHI
 */
typedef TRefCountPtr< FBaseBoundShaderStateRHI >		FBoundShaderStateRHIRef;

/**
 * @ingroup Engine Engine
 * @brief Pointer to FBoundShaderStateRHI
 */
typedef FBaseBoundShaderStateRHI*						FBoundShaderStateRHIParamRef;

/**
 * @ingroup Engine Engine
 * @brief Reference to FBaseVertexBufferRHI
 */
typedef TRefCountPtr< FBaseVertexBufferRHI >			FVertexBufferRHIRef;

/**
 * @ingroup Engine Engine
 * @brief Pointer to FBaseVertexBufferRHI
 */
typedef FBaseVertexBufferRHI*							FVertexBufferRHIParamRef;

/**
 * @ingroup Engine Engine
 * @brief Reference to FBaseIndexBufferRHI
 */
typedef TRefCountPtr< FBaseIndexBufferRHI >				FIndexBufferRHIRef;

/**
 * @ingroup Engine Engine
 * @brief Pointer to FBaseIndexBufferRHI
 */
typedef FBaseIndexBufferRHI*							FIndexBufferRHIParamRef;

/**
 * @ingroup Engine Engine
 * @brief Reference to FBaseVertexDeclarationRHI
 */
typedef TRefCountPtr< FBaseVertexDeclarationRHI >		FVertexDeclarationRHIRef;

/**
 * @ingroup Engine Engine
 * @brief Pointer to FBaseVertexDeclarationRHI
 */
typedef FBaseVertexDeclarationRHI*						FVertexDeclarationRHIParamRef;

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
typedef TRefCountPtr< FBaseSurfaceRHI >					FSurfaceRHIRef;

/**
 * @ingroup Engine Engine
 * Pointer to FBaseSurfaceRHI
 */
typedef FBaseSurfaceRHI*								FSurfaceRHIParamRef;

/**
 * @ingroup Engine Engine
 * Reference to base texture
 */
typedef TRefCountPtr< FBaseTextureRHI >					FTextureRHIRef;

/**
 * @ingroup Engine Engine
 * Pointer to base texture
 */
typedef FBaseTextureRHI*								FTextureRHIParamRef;

/**
 * @ingroup Engine Engine
 * Reference to texture 2D
 */
typedef TRefCountPtr< FBaseTextureRHI >					FTexture2DRHIRef;

/**
 * @ingroup Engine Engine
 * Pointer to texture 2D
 */
typedef FBaseTextureRHI*								FTexture2DRHIParamRef;

#endif // !TYPESRHI_H
