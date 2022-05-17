/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef MATERIAL_H
#define MATERIAL_H

#include <unordered_map>
#include <string>

#include "Math/Math.h"
#include "Misc/EngineGlobals.h"
#include "System/Package.h"
#include "RHI/BaseShaderRHI.h"
#include "Render/Shaders/ShaderManager.h"
#include "Render/Shaders/Shader.h"
#include "Render/Texture.h"
#include "Render/Shaders/BasePassShader.h"
#include "Core.h"

/**
 * @ingroup Engine Engine
 * @brief Reference to FMaterial
 */
typedef TRefCountPtr< class FMaterial >				FMaterialRef;

/**
 * @ingroup Engine
 * Flags usage material
 */
enum EMaterialUsage
{
	MU_None				= 0,							/**< Material not used on the all meshes */
	MU_StaticMesh		= 1 << 0,						/**< Material used on the static meshes */
	MU_Sprite			= 1 << 1,						/**< Material used on the sprite meshes */
	MU_AllMeshes		= MU_StaticMesh | MU_Sprite		/**< Material used on all type meshes */
};

/**
 * @ingroup Engine
 * Material
 */
class FMaterial : public FAsset
{
public:
	/**
	 * Constructor
	 */
	FMaterial();

	/**
	 * Destructor
	 */
	~FMaterial();

	/**
	 * Serialize to archive
	 *
	 * @param[in] InArchive Archive
	 */
	virtual void Serialize( class FArchive& InArchive ) override;

	/**
	 * Set scalar parameter value
	 *
	 * @param[in] InParameterName Parameter name
	 * @param[in] InValue Value
	 */
	FORCEINLINE void SetScalarParameterValue( const std::wstring& InParameterName, float InValue )
	{
		scalarParameters[ InParameterName ] = InValue;
		MarkDirty();
	}

	/**
	 * Set texture parameter value
	 *
	 * @param[in] InParameterName Parameter name
	 * @param[in] InValue Value
	 */
	FORCEINLINE void SetTextureParameterValue( const std::wstring& InParameterName, class FTexture2D* InValue )
	{
		textureParameters[ InParameterName ] = InValue;
		MarkDirty();
	}

	/**
	 * Set vector parameter value
	 *
	 * @param InParameterName	Parameter name
	 * @param InValue			Value
	 */
	FORCEINLINE void SetVectorParameterValue( const std::wstring& InParameterName, const FVector4D& InValue )
	{
		vectorParameters[ InParameterName ] = InValue;
		MarkDirty();
	}

	/**
	 * Set two sided mode
	 * 
	 * @param[in] InIsTwoSided Enable two sided mode
	 */
	FORCEINLINE void SetTwoSided( bool InIsTwoSided )
	{
		if ( isTwoSided != InIsTwoSided )
		{
			MarkDirty();
		}
		isTwoSided = InIsTwoSided;
	}

	/**
	 * Set wireframe mode
	 * 
	 * @param[in] InIsWireframe Enable wireframe mode
	 */
	FORCEINLINE void SetWireframe( bool InIsWireframe )
	{
		if ( isWireframe != InIsWireframe )
		{
			MarkDirty();
		}

		isWireframe = InIsWireframe;
	}

	/**
	 * Set use material on static meshes
	 * 
	 * @param[in] InIsUseOnStaticMeshes Is use this material on static meshes
	 */
	FORCEINLINE void UsageOnStaticMesh( bool InIsUseOnStaticMeshes )
	{
		if ( InIsUseOnStaticMeshes )
		{
			usage |= MU_StaticMesh;
		}
		else
		{
			usage &= ~MU_StaticMesh;
		}

		MarkDirty();
		isNeedUpdateShaderMap = true;
	}

	/**
	 * Set use material on sprite meshes
	 *
	 * @param[in] InIsUseOnSpriteMeshes Is use this material on sprite meshes
	 */
	FORCEINLINE void UsageOnSpriteMesh( bool InIsUseOnSpriteMeshes )
	{
		if ( InIsUseOnSpriteMeshes )
		{
			usage |= MU_Sprite;
		}
		else
		{
			usage &= ~MU_Sprite;
		}

		MarkDirty();
		isNeedUpdateShaderMap = true;
	}

	/**
	 * Set usage flags
	 * 
	 * @param[in] InUsageFlags Usage flags of material (see EMaterialUsage)
	 */
	FORCEINLINE void SetUsageFlags( uint32 InUsageFlags )
	{
		if ( usage != InUsageFlags )
		{
			MarkDirty();
		}

		usage = InUsageFlags;
		isNeedUpdateShaderMap = true;
	}

	/**
	 * Get usage flags
	 * @return Return usage flags
	 */
	FORCEINLINE uint32 GetUsageFlags() const
	{
		return usage;
	}

	/**
	 * Get shader
	 *
	 * @param[in] InVertexFactoryHash Vertex factory hash
	 * @param[in] InShaderFrequency Shader frequency
	 * @return Return pointer to shader
	 */
	FShader* GetShader( uint64 InVertexFactoryHash, EShaderFrequency InShaderFrequency );

	/**
	 * Get shader type
	 * 
	 * @param InShaderFrequency		Shader frequency
	 * @return Return shader meta type, if not exist return NULL
	 */
	FORCEINLINE class FShaderMetaType* GetShaderType( EShaderFrequency InShaderFrequency ) const
	{
		check( InShaderFrequency < SF_NumDrawFrequencies );
		static class FShaderMetaType*		shadersType[ SF_NumDrawFrequencies ] =
		{			
			&FBasePassVertexShader::staticType,		// SF_Vertex
			nullptr,								// SF_Hull
			nullptr,								// SF_Domain
			&FBasePassPixelShader::staticType,		// SF_Pixel
			nullptr									// SF_Geometry
		};

		return shadersType[ InShaderFrequency ];
	}

	/**
	 * Get scalar parameter value
	 *
	 * @param[in] InParameterName Parameter name
	 * @param[out] OutValue Return value
	 * @return Return true if finded, else return false
	 */
	bool GetScalarParameterValue( const std::wstring& InParameterName, float& OutValue ) const;

	/**
	 * Get texture parameter value
	 *
	 * @param[in] InParameterName Parameter name
	 * @param[out] OutValue Return value
	 * @return Return true if finded, else return false
	 */
	bool GetTextureParameterValue( const std::wstring& InParameterName, FTexture2DRef& OutValue ) const;

	/**
	 * Get vector parameter value
	 *
	 * @param InParameterName	Parameter name
	 * @param OutValue			Return value
	 * @return Return true if finded, else return false
	 */
	bool GetVectorParameterValue( const std::wstring& InParameterName, FVector4D& OutValue ) const;

#if WITH_EDITOR
	/**
	 * Get dependent assets
	 * @param OutDependentAssets	Output set of dependent assets
	 * @param InFilter				Filter of getting assets by type. If setted AT_Unknown return all types
	 */
	virtual void GetDependentAssets( FSetDependentAssets& OutDependentAssets, EAssetType InFilter = AT_Unknown ) const override;
#endif // WITH_EDITOR

	/**
	 * Is enabled two sided mode
	 * @return Return true if two sided mode enabled, else return false
	 */
	FORCEINLINE bool IsTwoSided() const
	{
		return isTwoSided;
	}

	/**
	 * Is enabled wireframe mode
	 * @return Return true if wireframe mode enabled, else return false
	 */
	FORCEINLINE bool IsWireframe() const
	{
		return isWireframe;
	}

private:
	/**
	 * Cache of shader map
	 */
	void CacheShaderMap();

	/**
	 * Get array shader instances for mesh type
	 * 
	 * @param[in] InVertexFactoryHash Vertex factory hash
	 * @return Return array shader instances for mesh type
	 */
	std::vector< FShader* > GetMeshShaders( uint64 InVertexFactoryHash ) const;

	/**
	 * Typedef mesh shader map
	 */
	typedef std::unordered_map< uint64, std::vector< FShader* > >				FMeshShaderMap;

	bool												isNeedUpdateShaderMap;	/**< Is need update shader map */
	bool												isTwoSided;				/**< Is two sided material */
	bool												isWireframe;			/**< Is wireframe material */
	uint32												usage;					/**< Usage flags (see EMaterialUsage) */
	FMeshShaderMap										shaderMap;				/**< Shader map for material */
	std::unordered_map< std::wstring, float >			scalarParameters;		/**< Array scalar parameters */
	std::unordered_map< std::wstring, FVector4D >		vectorParameters;		/**< Vector parameters */
	std::unordered_map< std::wstring, FTexture2DRef >	textureParameters;		/**< Array texture parameters */
};

//
// Serialization
//

FORCEINLINE FArchive& operator<<( FArchive& InArchive, FMaterialRef& InValue )
{
	FAssetRef			asset = InValue;
	InArchive << asset;

	if ( InArchive.IsLoading() )
	{
		InValue = asset;
	}
	return InArchive;
}

FORCEINLINE FArchive& operator<<( FArchive& InArchive, const FMaterialRef& InValue )
{
	check( InArchive.IsSaving() );
	InArchive << ( FAssetRef )InValue;
	return InArchive;
}

#endif // !MATERIAL_H
