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
#include "Misc/RefCounted.h"
#include "Misc/EngineGlobals.h"
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
class FMaterial : public FRefCounted
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
	void Serialize( class FBaseArchive& InArchive );

	/**
	 * Set shader
	 *
	 * @param[in] InShaderMetaType Meta info about shader
	 * @param[in] InShaderFrequency Frequency
	 */
	FORCEINLINE void SetShader( const class FShaderMetaType& InShaderMetaType, EShaderFrequency InShaderFrequency )
	{
		check( InShaderFrequency < SF_NumFrequencies );
		shaders[ InShaderFrequency ] = GShaderManager->FindInstance( InShaderMetaType.GetName() );
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
	 * Get shader
	 *
	 * @param[in] InShaderFrequency Frequency
	 * @return Return pointer to shader
	 */
	FORCEINLINE FShaderRef GetShader( EShaderFrequency InShaderFrequency ) const
	{
		check( InShaderFrequency < SF_NumFrequencies );
		return shaders[ InShaderFrequency ];
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
	FShaderRef												shaders[ SF_NumFrequencies ];		/**< Shader for each stage */
	std::unordered_map< std::wstring, float >				scalarParameters;					/**< Array scalar parameters */
	std::unordered_map< std::wstring, FTexture2DRef >		textureParameters;					/**< Array texture parameters */
};

#endif // !MATERIAL_H