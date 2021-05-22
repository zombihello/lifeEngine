/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright BSOD-Games, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef BASESHADERRHI_H
#define BASESHADERRHI_H

#include <string>

#include "Core.h"
#include "Misc/RefCounted.h"

/**
 * @ingroup Engine
 * @brief Enumeration of types shader
 */
enum EShaderFrequency
{
	SF_Vertex,				/**< Vertex shader */
	SF_Hull,				/**< Hull shader */
	SF_Domain,				/**< Domain shader */
	SF_Pixel,				/**< Pixel shader */
	SF_Geometry,			/**< Geometry shader */
	SF_Compute,				/**< Compute shader */
	SF_NumFrequencies		/**< Count frequencies */
};

/**
 * @ingroup Engine
 * @brief Base class for work with shader
 */
class FBaseShaderRHI : public FRefCounted
{
public:
	/**
	 * @brief Constructor
	 * @param[in] InFrequency Frequency of shader
	 */
	explicit									FBaseShaderRHI( EShaderFrequency InFrequency ) :
		frequency( InFrequency )
	{}

	/**
	 * @brief Set shader name
	 * @param[in] InShaderName Shader name
	 */
	FORCEINLINE void							SetShaderName( const tchar* InShaderName )
	{
#if !SHIPPING
		shaderName = InShaderName;
#endif // !SHIPPING
	}

	/**
	 * @brief Get shader name
	 * @return Return shader name
	 */
	FORCEINLINE const tchar*					GetShaderName() const
	{
#if !SHIPPING
		return shaderName.c_str();
#else
		return TEXT( "" );
#endif // !SHIPPING
	}

	/**
	 * @brief Get frequency
	 * @return Return frequency of shader
	 */
	FORCEINLINE EShaderFrequency				GetFrequency() const
	{
		return frequency;
	}

private:
#if !SHIPPING
	std::wstring				shaderName;		/**< Shader name */
#endif // !SHIPPING

	EShaderFrequency			frequency;		/**< Frequency of shader */
};

/**
 * @ingroup Engine
 * @brief Base class of bound shader state
 */
class FBoundShaderStateRHI : public FRefCounted
{};

#endif // !BASESHADERRHI_H