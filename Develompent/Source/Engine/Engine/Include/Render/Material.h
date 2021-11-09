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
#include "Core.h"

/**
 * @ingroup Engine Engine
 * @brief Reference to FMaterial
 */
typedef TRefCountPtr< class FMaterial >				FMaterialRef;

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
	 * Set pixel shader
	 *
	 * @param[in] InShaderMetaType Meta info about shader
	 */
	FORCEINLINE void SetShader( const class FShaderMetaType& InShaderMetaType )
	{
		shader = GShaderManager->FindInstance( InShaderMetaType.GetName() );
	}

	/**
	 * Set scalar parameter value
	 *
	 * @param[in] InParameterName Parameter name
	 * @param[in] InValue Value
	 */
	FORCEINLINE void SetScalarParameterValue( const std::wstring& InParameterName, float InValue )
	{
		scalarParameters[ InParameterName ] = InValue;
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
	}

	/**
	 * Set two sided mode
	 * 
	 * @param[in] InIsTwoSided Enable two sided mode
	 */
	FORCEINLINE void SetTwoSided( bool InIsTwoSided )
	{
		isTwoSided = InIsTwoSided;
	}

	/**
	 * Set wireframe mode
	 * 
	 * @param[in] InIsWireframe Enable wireframe mode
	 */
	FORCEINLINE void SetWireframe( bool InIsWireframe )
	{
		isWireframe = InIsWireframe;
	}

	/**
	 * Get pixel shader
	 *
	 * @return Return pointer to pixel shader
	 */
	FORCEINLINE FShaderRef GetShader() const
	{
		return shader;
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
	bool													isTwoSided;							/**< Is two sided material */
	bool													isWireframe;						/**< Is wireframe material */
	FShaderRef												shader;								/**< Pixel shader */
	std::unordered_map< std::wstring, float >				scalarParameters;					/**< Array scalar parameters */
	std::unordered_map< std::wstring, FTexture2DRef >		textureParameters;					/**< Array texture parameters */
};

//
// Serialization
//

FORCEINLINE FArchive& operator<<( FArchive& InArchive, FMaterialRef& InValue )
{
	if ( InArchive.IsSaving() )
	{
		InArchive << ( InValue ? InValue->GetAssetReference() : FAssetReference() );
	}
	else
	{
		FAssetReference			assetReference;
		InArchive << assetReference;
		if ( assetReference.IsValid() )
		{
			InValue = GPackageManager->FindAsset( assetReference.pathPackage, assetReference.hash );
		}
	}

	return InArchive;
}

FORCEINLINE FArchive& operator<<( FArchive& InArchive, const FMaterialRef& InValue )
{
	check( InArchive.IsSaving() );
	InArchive << ( InValue ? InValue->GetAssetReference() : FAssetReference() );
	return InArchive;
}

#endif // !MATERIAL_H