/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright BSOD-Games, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef SHADER_H
#define SHADER_H

#include <vector>

#include "Misc/RefCounted.h"
#include "RHI/TypesRHI.h"
#include "ShaderCache.h"

/**
 * @ingroup Engine Engine
 * @brief Reference to FShader
*/
typedef TRefCountPtr< class FShader >				FShaderRef;

/**
 * @ingroup Engine
 * @brief Base class of shader
 */
class FShader : public FRefCounted
{
public:
	/**
	 * @brief Initialize shader
	 * @param[in] InShaderCacheItem Cache of shader
	 */
	void										Init( const FShaderCache::FShaderCacheItem& InShaderCacheItem );

	/**
	 * @brief Get name of shader
	 * @return Return shader name
	 */
	FORCEINLINE const std::wstring&				GetName() const
	{
		return name;
	}

	/**
	 * @brief Get frequency of shader
	 * @return Return Frequency of shader
	 */
	FORCEINLINE EShaderFrequency				GetFrequency() const
	{
		return frequency;
	}

	/**
	 * @brief Get count instructions in shader
	 * @return Return count instruction in shader
	 */
	FORCEINLINE uint32							GetNumInstructions() const
	{
		return numInstructions;
	}

	/**
	 * @brief Get vertex shader
	 * @return Return pointer to RHI vertex shader. If this shader not SF_Vertex return nullptr
	 */
	FORCEINLINE FVertexShaderRHIRef				GetVertexShader() const
	{
		return vertexShader;
	}

	/**
	 * @brief Get hull shader
	 * @return Return pointer to RHI hull shader. If this shader not SF_Hull return nullptr
	 */
	FORCEINLINE FHullShaderRHIRef				GetHullShader() const
	{
		return hullShader;
	}

	/**
	 * @brief Get domain shader
	 * @return Return pointer to RHI domain shader. If this shader not SF_Domain return nullptr
	 */
	FORCEINLINE FDomainShaderRHIRef				GetDomainShader() const
	{
		return domainShader;
	}

	/**
	 * @brief Get geometry shader
	 * @return Return pointer to RHI geometry shader. If this shader not SF_Geometry return nullptr
	 */
	FORCEINLINE FGeometryShaderRHIRef			GetGeometryShader() const
	{
		return geometryShader;
	}

	/**
	 * @brief Get pixel shader
	 * @return Return pointer to RHI pixel shader. If this shader not SF_Pixel return nullptr
	 */
	FORCEINLINE FPixelShaderRHIRef				GetPixelShader() const
	{
		return pixelShader;
	}

private:
	std::wstring				name;				/**< Name of class shader */
	EShaderFrequency			frequency;			/**< Frequency of shader */
	uint32						numInstructions;	/**< Number instructions in shader */
	FVertexShaderRHIRef			vertexShader;		/**< Pointer to RHI vertex shader */
	FHullShaderRHIRef			hullShader;			/**< Pointer to RHI hull shader */
	FDomainShaderRHIRef			domainShader;		/**< Pointer to RHI domain shader */
	FGeometryShaderRHIRef		geometryShader;		/**< Pointer to RHI geometry shader */
	FPixelShaderRHIRef			pixelShader;		/**< Pointer to RHI pixel shader */
};

//
// Serializations
//

/**
 * Overload operator << for serialize bool
 */
FArchive& operator<<( FArchive& InArchive, FShaderRef& InValue );

/**
 * Overload operator << for serialize bool
 */
FORCEINLINE FArchive& operator<<( FArchive& InArchive, const FShaderRef& InValue )
{
	check( InArchive.IsSaving() );
	InArchive << InValue->GetName();
	return InArchive;
}

#endif // !SHADER_H

