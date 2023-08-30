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
#include "Misc/SharedPointer.h"

#if WITH_EDITOR
#include "ShaderCompiler.h"
#endif // WITH_EDITOR

/**
 * @ingroup Engine
 * @brief Base class of shader
 */
class CShader
{
public:
	/**
	 * @brief Initialize shader
	 * @param[in] InShaderCacheItem Cache of shader
	 */
	virtual void Init( const CShaderCache::ShaderCacheItem& InShaderCacheItem );

	/**
	 * @brief Set the constant shader parameters
	 * 
	 * @param InDeviceContextRHI Device context
	 * @param InVertexFactory Vertex factory
	 * @param InMaterialResource Material
	 */
	virtual void SetConstantParameters( class CBaseDeviceContextRHI* InDeviceContextRHI, const class CVertexFactory* InVertexFactory, const TSharedPtr<class CMaterial>& InMaterialResource ) const;

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
	virtual void SetMesh( class CBaseDeviceContextRHI* InDeviceContextRHI, const struct MeshBatch& InMesh, const class CVertexFactory* InVertexFactory, const class CSceneView* InView, uint32 InNumInstances = 1, uint32 InStartInstanceID = 0 ) const;

#if WITH_EDITOR
	/**
	 * @brief Is need compile shader for platform
	 *
	 * @param InShaderPlatform Shader platform
	 * @param InVFMetaType Vertex factory meta type. If him is nullptr - return general Assert
	 * @return Return true if need compile shader, else returning false
	 */
	static bool ShouldCache( EShaderPlatform InShaderPlatform, class CVertexFactoryMetaType* InVFMetaType = nullptr );

	/**
	 * @brief Modify compilation environment
	 *
	 * @param InShaderPlatform Shader platform
	 * @param InEnvironment Shader compiler environment
	 */
	static void ModifyCompilationEnvironment( EShaderPlatform InShaderPlatform, ShaderCompilerEnvironment& InEnvironment );
#endif // WITH_EDITOR

	/**
	 * @brief Get name of shader
	 * @return Return shader name
	 */
	FORCEINLINE const std::wstring& GetName() const
	{
		return name;
	}

	/**
	 * @brief Get frequency of shader
	 * @return Return Frequency of shader
	 */
	FORCEINLINE EShaderFrequency GetFrequency() const
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
	FORCEINLINE uint32 GetNumInstructions() const
	{
		return numInstructions;
	}

	/**
	 * @brief Get vertex shader
	 * @return Return pointer to RHI vertex shader. If this shader not SF_Vertex return nullptr
	 */
	FORCEINLINE VertexShaderRHIRef_t GetVertexShader() const
	{
		return vertexShader;
	}

	/**
	 * @brief Get hull shader
	 * @return Return pointer to RHI hull shader. If this shader not SF_Hull return nullptr
	 */
	FORCEINLINE HullShaderRHIRef_t GetHullShader() const
	{
		return hullShader;
	}

	/**
	 * @brief Get domain shader
	 * @return Return pointer to RHI domain shader. If this shader not SF_Domain return nullptr
	 */
	FORCEINLINE DomainShaderRHIRef_t GetDomainShader() const
	{
		return domainShader;
	}

	/**
	 * @brief Get geometry shader
	 * @return Return pointer to RHI geometry shader. If this shader not SF_Geometry return nullptr
	 */
	FORCEINLINE GeometryShaderRHIRef_t GetGeometryShader() const
	{
		return geometryShader;
	}

	/**
	 * @brief Get pixel shader
	 * @return Return pointer to RHI pixel shader. If this shader not SF_Pixel return nullptr
	 */
	FORCEINLINE PixelShaderRHIRef_t GetPixelShader() const
	{
		return pixelShader;
	}

	/**
	 * Get meta type of this shader
	 * @return Return meta type shader
	 */
	virtual class CShaderMetaType* GetType() const = 0;

private:
	std::wstring				name;				/**< Name of class shader */
	EShaderFrequency			frequency;			/**< Frequency of shader */
	uint64						vertexFactoryHash;	/**< Vertex factory hash */
	uint32						numInstructions;	/**< Number instructions in shader */
	VertexShaderRHIRef_t		vertexShader;		/**< Pointer to RHI vertex shader */
	HullShaderRHIRef_t			hullShader;			/**< Pointer to RHI hull shader */
	DomainShaderRHIRef_t		domainShader;		/**< Pointer to RHI domain shader */
	GeometryShaderRHIRef_t		geometryShader;		/**< Pointer to RHI geometry shader */
	PixelShaderRHIRef_t			pixelShader;		/**< Pointer to RHI pixel shader */
};

//
// Serializations
//

/**
 * Overload operator << for serialize bool
 */
CArchive& operator<<( CArchive& InArchive, CShader*& InValue );

/**
 * Overload operator << for serialize bool
 */
FORCEINLINE CArchive& operator<<( CArchive& InArchive, const CShader*& InValue )
{
	Assert( InArchive.IsSaving() );
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

