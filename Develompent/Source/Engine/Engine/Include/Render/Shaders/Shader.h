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

#if WITH_EDITOR
#include "ShaderCompiler.h"
#endif // WITH_EDITOR

/**
 * @ingroup Engine
 * @brief Base class of shader
 */
class FShader
{
public:
	/**
	 * @brief Initialize shader
	 * @param[in] InShaderCacheItem Cache of shader
	 */
	virtual void Init( const FShaderCache::FShaderCacheItem& InShaderCacheItem );

	/**
	 * @brief Set the constant shader parameters
	 * 
	 * @param InDeviceContextRHI Device context
	 * @param InVertexFactory Vertex factory
	 * @param InMaterialResource Material
	 */
	virtual void SetConstantParameters( class FBaseDeviceContextRHI* InDeviceContextRHI, const class FVertexFactory* InVertexFactory, const class FMaterial* InMaterialResource ) const;

	/**
	 * @brief Set the l2w transform shader
	 * 
	 * @param InDeviceContextRHI RHI device context
	 * @param InMesh Mesh data
	 * @param InVertexFactory Vertex factory
	 * @param InView Scene view
	 * @param InNumInstances Number instances
	 * @param InStartInstanceID ID of first instance
	 */
	virtual void SetMesh( class FBaseDeviceContextRHI* InDeviceContextRHI, const struct FMeshBatch& InMesh, const class FVertexFactory* InVertexFactory, const class FSceneView* InView, uint32 InNumInstances = 1, uint32 InStartInstanceID = 0 ) const;

#if WITH_EDITOR
	/**
	 * @brief Is need compile shader for platform
	 *
	 * @param InShaderPlatform Shader platform
	 * @param InVFMetaType Vertex factory meta type. If him is nullptr - return general check
	 * @return Return true if need compile shader, else returning false
	 */
	static bool ShouldCache( EShaderPlatform InShaderPlatform, class FVertexFactoryMetaType* InVFMetaType = nullptr );

	/**
	 * @brief Modify compilation environment
	 *
	 * @param InShaderPlatform Shader platform
	 * @param InEnvironment Shader compiler environment
	 */
	static void ModifyCompilationEnvironment( EShaderPlatform InShaderPlatform, FShaderCompilerEnvironment& InEnvironment );
#endif // WITH_EDITOR

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
	 * Get vertex factory hash
	 * @return Return vertex factory hash if frequency SF_Vertex, else return INVALID_HASH
	 */
	FORCEINLINE uint64 GetVertexFactoryHash() const
	{
		return vertexFactoryHash;
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

	/**
	 * Get meta type of this shader
	 * @return Return meta type shader
	 */
	virtual class FShaderMetaType* GetType() const = 0;

private:
	std::wstring				name;				/**< Name of class shader */
	EShaderFrequency			frequency;			/**< Frequency of shader */
	uint64						vertexFactoryHash;	/**< Vertex factory hash */
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
FArchive& operator<<( FArchive& InArchive, FShader*& InValue );

/**
 * Overload operator << for serialize bool
 */
FORCEINLINE FArchive& operator<<( FArchive& InArchive, const FShader*& InValue )
{
	check( InArchive.IsSaving() );
	if ( InValue )
	{
		InArchive << InValue->GetName();
		InArchive << InValue->GetVertexFactoryHash();
	}
	else
	{
		InArchive << std::wstring();
		InArchive << ( uint64 )INVALID_HASH;
	}

	return InArchive;
}

#endif // !SHADER_H

