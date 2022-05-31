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
class CMaterial : public CAsset
{
public:
	/**
	 * Constructor
	 */
	CMaterial();

	/**
	 * Destructor
	 */
	~CMaterial();

	/**
	 * Serialize to archive
	 *
	 * @param[in] InArchive Archive
	 */
	virtual void Serialize( class CArchive& InArchive ) override;

	/**
	 * Set scalar parameter value
	 *
	 * @param[in] InParameterName Parameter name
	 * @param[in] InValue Value
	 */
	FORCEINLINE void SetScalarParameterValue( const std::wstring& InParameterName, float InValue )
	{
		auto	itScalarParameter	= scalarParameters.find( InParameterName );
		bool	bNotFound			= itScalarParameter == scalarParameters.end();
		if ( bNotFound || itScalarParameter->second != InValue )
		{
			MarkDirty();
		}

		if ( !bNotFound )
		{
			itScalarParameter->second			= InValue;
		}
		else
		{
			scalarParameters[ InParameterName ] = InValue;
		}
	}

	/**
	 * Set texture parameter value
	 *
	 * @param[in] InParameterName Parameter name
	 * @param[in] InValue Value
	 */
	FORCEINLINE void SetTextureParameterValue( const std::wstring& InParameterName, const TAssetHandle<class CTexture2D>& InValue )
	{
		auto	itTextureParameter	= textureParameters.find( InParameterName );
		bool	bNotFound			= itTextureParameter == textureParameters.end();
		if ( bNotFound || itTextureParameter->second != InValue )
		{
			MarkDirty();
		}

		if ( !bNotFound )
		{
			itTextureParameter->second			= InValue;
		}
		else
		{
			textureParameters[ InParameterName ] = InValue;
		}
	}

	/**
	 * Set vector parameter value
	 *
	 * @param InParameterName	Parameter name
	 * @param InValue			Value
	 */
	FORCEINLINE void SetVectorParameterValue( const std::wstring& InParameterName, const Vector4D& InValue )
	{
		auto	itVectorParameter	= vectorParameters.find( InParameterName );
		bool	bNotFound			= itVectorParameter == vectorParameters.end();
		if ( bNotFound || itVectorParameter->second != InValue )
		{
			MarkDirty();
		}

		if ( !bNotFound )
		{
			itVectorParameter->second			= InValue;
		}
		else
		{
			vectorParameters[ InParameterName ] = InValue;
		}
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
		bool	bUseOnStaticMesh = usage & MU_StaticMesh;
		if ( bUseOnStaticMesh != InIsUseOnStaticMeshes )
		{
			MarkDirty();
		}

		if ( InIsUseOnStaticMeshes )
		{
			usage |= MU_StaticMesh;
		}
		else
		{
			usage &= ~MU_StaticMesh;
		}
		isNeedUpdateShaderMap = true;
	}

	/**
	 * Set use material on sprite meshes
	 *
	 * @param[in] InIsUseOnSpriteMeshes Is use this material on sprite meshes
	 */
	FORCEINLINE void UsageOnSpriteMesh( bool InIsUseOnSpriteMeshes )
	{
		bool	bUseOnSpriteMesh = usage & MU_Sprite;
		if ( bUseOnSpriteMesh != InIsUseOnSpriteMeshes )
		{
			MarkDirty();
		}

		if ( InIsUseOnSpriteMeshes )
		{
			usage |= MU_Sprite;
		}
		else
		{
			usage &= ~MU_Sprite;
		}
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
	CShader* GetShader( uint64 InVertexFactoryHash, EShaderFrequency InShaderFrequency );

	/**
	 * Get shader type
	 * 
	 * @param InShaderFrequency		Shader frequency
	 * @return Return shader meta type, if not exist return NULL
	 */
	FORCEINLINE class CShaderMetaType* GetShaderType( EShaderFrequency InShaderFrequency ) const
	{
		check( InShaderFrequency < SF_NumDrawFrequencies );
		static class CShaderMetaType*		shadersType[ SF_NumDrawFrequencies ] =
		{			
			&CBasePassVertexShader::staticType,		// SF_Vertex
			nullptr,								// SF_Hull
			nullptr,								// SF_Domain
			&CBasePassPixelShader::staticType,		// SF_Pixel
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
	bool GetTextureParameterValue( const std::wstring& InParameterName, TAssetHandle<CTexture2D>& OutValue ) const;

	/**
	 * Get vector parameter value
	 *
	 * @param InParameterName	Parameter name
	 * @param OutValue			Return value
	 * @return Return true if finded, else return false
	 */
	bool GetVectorParameterValue( const std::wstring& InParameterName, Vector4D& OutValue ) const;

	/**
	 * Get dependent assets
	 * @param OutDependentAssets	Output set of dependent assets
	 * @param InFilter				Filter of getting assets by type. If setted AT_Unknown return all types
	 */
	virtual void GetDependentAssets( SetDependentAssets_t& OutDependentAssets, EAssetType InFilter = AT_Unknown ) const override;

	/**
	 * Reload dependent assets
	 * @param InForce	Is need force reload assets
	 */
	virtual void ReloadDependentAssets( bool InForce = false );

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
	std::vector< CShader* > GetMeshShaders( uint64 InVertexFactoryHash ) const;

	/**
	 * Typedef mesh shader map
	 */
	typedef std::unordered_map< uint64, std::vector< CShader* > >				MeshShaderMap_t;

	bool															isNeedUpdateShaderMap;	/**< Is need update shader map */
	bool															isTwoSided;				/**< Is two sided material */
	bool															isWireframe;			/**< Is wireframe material */
	uint32															usage;					/**< Usage flags (see EMaterialUsage) */
	MeshShaderMap_t													shaderMap;				/**< Shader map for material */
	std::unordered_map< std::wstring, float >						scalarParameters;		/**< Array scalar parameters */
	std::unordered_map< std::wstring, Vector4D >					vectorParameters;		/**< Vector parameters */
	std::unordered_map< std::wstring, TAssetHandle<CTexture2D> >	textureParameters;		/**< Array texture parameters */
};

//
// Serialization
//

FORCEINLINE CArchive& operator<<( CArchive& InArchive, TAssetHandle<CMaterial>& InValue )
{
	TAssetHandle<CAsset>		asset = InValue;
	InArchive << asset;

	if ( InArchive.IsLoading() )
	{
		InValue = asset;
	}
	return InArchive;
}

FORCEINLINE CArchive& operator<<( CArchive& InArchive, const TAssetHandle<CMaterial>& InValue )
{
	check( InArchive.IsSaving() );
	InArchive << ( TAssetHandle<CAsset> )InValue;
	return InArchive;
}

#endif // !MATERIAL_H
