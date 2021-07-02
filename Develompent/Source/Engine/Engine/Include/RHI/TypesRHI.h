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
 * @brief Reference to FHullShaderRHI
 */
typedef TRefCountPtr< FBaseShaderRHI >					FHullShaderRHIRef;

/**
 * @ingroup Engine Engine
 * @brief Reference to FDomainShaderRHI
 */
typedef TRefCountPtr< FBaseShaderRHI >					FDomainShaderRHIRef;

/**
 * @ingroup Engine Engine
 * @brief Reference to FPixelShaderRHI
 */
typedef TRefCountPtr< FBaseShaderRHI >					FPixelShaderRHIRef;

/**
 * @ingroup Engine Engine
 * @brief Reference to FGeometryShaderRHI
 */
typedef TRefCountPtr< FBaseShaderRHI >					FGeometryShaderRHIRef;

/**
 * @ingroup Engine Engine
 * @brief Reference to FBoundShaderStateRHI
 */
typedef TRefCountPtr< FBoundShaderStateRHI >			FBoundShaderStateRHIRef;

/**
 * @ingroup Engine Engine
 * @brief Reference to FBaseVertexBufferRHI
 */
typedef TRefCountPtr< FBaseVertexBufferRHI >			FVertexBufferRHIRef;

/**
 * @ingroup Engine Engine
 * @brief Reference to FBaseIndexBufferRHI
 */
typedef TRefCountPtr< FBaseIndexBufferRHI >				FIndexBufferRHIRef;

#endif // !TYPESRHI_H
