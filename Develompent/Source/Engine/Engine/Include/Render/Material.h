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
#include "System/Name.h"
#include "RHI/BaseShaderRHI.h"
#include "Render/Shaders/ShaderManager.h"
#include "Render/Shaders/Shader.h"
#include "Render/Texture.h"
#include "Render/Shaders/BasePassShader.h"
#include "Core.h"

/**
 * @ingroup Engine
 * @brief Flags usage material
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
 * @brief Material
 */
class CMaterial : public CAsset
{
public:
	/**
	 * @brief Constructor
	 */
	CMaterial();

	/**
	 * @brief Destructor
	 */
	~CMaterial();

	/**
	 * @brief Serialize to archive
	 *
	 * @param InArchive		Archive
	 */
	virtual void Serialize( class CArchive& InArchive ) override;

	/**
	 * @brief Set scalar parameter value
	 *
	 * @param InParameterName	Parameter name
	 * @param InValue			Value
	 */
	FORCEINLINE void SetScalarParameterValue( const CName& InParameterName, float InValue )
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
	 * @brief Set texture parameter value
	 *
	 * @param InParameterName	Parameter name
	 * @param InValue			Value
	 */
	FORCEINLINE void SetTextureParameterValue( const CName& InParameterName, const TAssetHandle<class CTexture2D>& InValue )
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
	 * @brief Set vector parameter value
	 *
	 * @param InParameterName	Parameter name
	 * @param InValue			Value
	 */
	FORCEINLINE void SetVectorParameterValue( const CName& InParameterName, const Vector4D& InValue )
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
	 * @brief Set two sided mode
	 * 
	 * @param InIsTwoSided		Enable two sided mode
	 */
	FORCEINLINE void SetTwoSided( bool InIsTwoSided )
	{
		if ( bTwoSided != InIsTwoSided )
		{
			MarkDirty();
		}
		bTwoSided = InIsTwoSided;
	}

	/**
	 * @brief Set wireframe mode
	 * 
	 * @param InIsWireframe		Enable wireframe mode
	 */
	FORCEINLINE void SetWireframe( bool InIsWireframe )
	{
		if ( bWireframe != InIsWireframe )
		{
			MarkDirty();
		}

		bWireframe = InIsWireframe;
	}

	/**
	 * @brief Set use material on static meshes
	 * 
	 * @param InIsUseOnStaticMeshes		Is use this material on static meshes
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
		bNeedUpdateShaderMap = true;
	}

	/**
	 * @brief Set use material on sprite meshes
	 *
	 * @param InIsUseOnSpriteMeshes			Is use this material on sprite meshes
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
		bNeedUpdateShaderMap = true;
	}

	/**
	 * @brief Set usage flags
	 * 
	 * @param InUsageFlags		Usage flags of material (see EMaterialUsage)
	 */
	FORCEINLINE void SetUsageFlags( uint32 InUsageFlags )
	{
		if ( usage != InUsageFlags )
		{
			MarkDirty();
		}

		usage = InUsageFlags;
		bNeedUpdateShaderMap = true;
	}

	/**
	 * @brief Get usage flags
	 * @return Return usage flags
	 */
	FORCEINLINE uint32 GetUsageFlags() const
	{
		return usage;
	}

	/**
	 * @brief Get shader
	 *
	 * @param InVertexFactoryHash	Vertex factory hash
	 * @param InShaderFrequency		Shader frequency
	 * @return Return pointer to shader
	 */
	CShader* GetShader( uint64 InVertexFactoryHash, EShaderFrequency InShaderFrequency );

	/**
	 * @brief Get shader type
	 * 
	 * @param InShaderFrequency		Shader frequency
	 * @return Return shader meta type, if not exist return NULL
	 */
	FORCEINLINE class CShaderMetaType* GetShaderType( EShaderFrequency InShaderFrequency ) const
	{
		Assert( InShaderFrequency < SF_NumDrawFrequencies );
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
	 * @brief Get scalar parameter value
	 *
	 * @param InParameterName	Parameter name
	 * @param OutValue			Return value
	 * @return Return true if found, otherwise false
	 */
	bool GetScalarParameterValue( const CName& InParameterName, float& OutValue ) const;

	/**
	 * @brief Get texture parameter value
	 *
	 * @param InParameterName	Parameter name
	 * @param OutValue			Return value
	 * @return Return true if found, otherwise false
	 */
	bool GetTextureParameterValue( const CName& InParameterName, TAssetHandle<CTexture2D>& OutValue ) const;

	/**
	 * @brief Get vector parameter value
	 *
	 * @param InParameterName	Parameter name
	 * @param OutValue			Return value
	 * @return Return true if found, otherwise false
	 */
	bool GetVectorParameterValue( const CName& InParameterName, Vector4D& OutValue ) const;

	/**
	 * @brief Get dependent assets
	 * 
	 * @param OutDependentAssets	Output set of dependent assets
	 * @param InFilter				Filter of getting assets by type. If setted AT_Unknown return all types
	 */
	virtual void GetDependentAssets( SetDependentAssets_t& OutDependentAssets, EAssetType InFilter = AT_Unknown ) const override;

	/**
	 * @brief Reload dependent assets
	 * @param InForce	Is need force reload assets
	 */
	virtual void ReloadDependentAssets( bool InForce = false );

	/**
	 * @brief Is enabled two sided mode
	 * @return Return true if two sided mode enabled, else return false
	 */
	FORCEINLINE bool IsTwoSided() const
	{
		return bTwoSided;
	}

	/**
	 * @brief Is enabled wireframe mode
	 * @return Return true if wireframe mode enabled, else return false
	 */
	FORCEINLINE bool IsWireframe() const
	{
		return bWireframe;
	}

	/**
	 * @brief Set translucency material
	 * @param InIsTranslucency	Is translucency material
	 */
	FORCEINLINE void SetTranslucency( bool InIsTranslucency )
	{
		if ( bTranslucency != InIsTranslucency )
		{
			MarkDirty();
		}

		bTranslucency = InIsTranslucency;
	}

	/**
	 * @brief Is translucency material
	 * @return Return TRUE if material is translucency, otherwise FALSE
	 */
	FORCEINLINE bool IsTranslucency() const
	{
		return bTranslucency;
	}

	const static CName		albedoTextureParamName;				/**< Name of Albedo texture parameter */
	const static CName		normalTextureParamName;				/**< Name of Normal texture parameter */
	const static CName		metallicTextureParamName;			/**< Name of Metallic texture parameter */
	const static CName		roughnessTextureParamName;			/**< Name of Roughness texture parameter */
	const static CName		emissionTextureParamName;			/**< Name of Emission texture parameter */
	const static CName		aoTextureParamName;					/**< Name of AO texture parameter */

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

	bool																			bNeedUpdateShaderMap;	/**< Is need update shader map */
	bool																			bTwoSided;				/**< Is two sided material */
	bool																			bWireframe;				/**< Is wireframe material */
	bool																			bTranslucency;			/**< Is translucency material */
	uint32																			usage;					/**< Usage flags (see EMaterialUsage) */
	MeshShaderMap_t																	shaderMap;				/**< Shader map for material */
	std::unordered_map<CName, float, CName::SHashFunction>							scalarParameters;		/**< Array scalar parameters */
	std::unordered_map<CName, Vector4D, CName::SHashFunction>						vectorParameters;		/**< Vector parameters */
	std::unordered_map<CName, TAssetHandle<CTexture2D>, CName::SHashFunction>		textureParameters;		/**< Array texture parameters */
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
	Assert( InArchive.IsSaving() );
	InArchive << ( TAssetHandle<CAsset> )InValue;
	return InArchive;
}

#endif // !MATERIAL_H
