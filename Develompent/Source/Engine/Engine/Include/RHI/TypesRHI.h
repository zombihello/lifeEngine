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

 /**
  * @ingroup Engine Engine
  * @brief Reference to FVertexShaderRHI
  */
typedef TRefCountPtr< FBaseShaderRHI >					FVertexShaderRHIRef;

/**
 * @ingroup Engine Engine
 * @brief Pointer to FVertexShaderRHI
 */
typedef FBaseShaderRHI*									FVertexShaderRHIParamRef;

/**
 * @ingroup Engine Engine
 * @brief Reference to FHullShaderRHI
 */
typedef TRefCountPtr< FBaseShaderRHI >					FHullShaderRHIRef;

/**
 * @ingroup Engine Engine
 * @brief Pointer to FHullShaderRHI
 */
typedef FBaseShaderRHI*									FHullShaderRHIParamRef;

/**
 * @ingroup Engine Engine
 * @brief Reference to FDomainShaderRHI
 */
typedef TRefCountPtr< FBaseShaderRHI >					FDomainShaderRHIRef;

/**
 * @ingroup Engine Engine
 * @brief Pointer to FDomainShaderRHI
 */
typedef FBaseShaderRHI*									FDomainShaderRHIParamRef;

/**
 * @ingroup Engine Engine
 * @brief Reference to FPixelShaderRHI
 */
typedef TRefCountPtr< FBaseShaderRHI >					FPixelShaderRHIRef;

/**
 * @ingroup Engine Engine
 * @brief Pointer to FPixelShaderRHI
 */
typedef FBaseShaderRHI*									FPixelShaderRHIParamRef;

/**
 * @ingroup Engine Engine
 * @brief Reference to FGeometryShaderRHI
 */
typedef TRefCountPtr< FBaseShaderRHI >					FGeometryShaderRHIRef;

/**
 * @ingroup Engine Engine
 * @brief Pointer to FGeometryShaderRHI
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

#endif // !TYPESRHI_H
