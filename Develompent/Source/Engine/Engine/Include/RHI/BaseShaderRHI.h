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
 * @brief Enumeration of types vertex element
 */
enum EVertexElementType
{
	VET_None,			/**< Unknown type */
	VET_Float1,			/**< Float type */
	VET_Float2,			/**< Vector of 2 floats */
	VET_Float3,			/**< Vector of 3 floats */
	VET_Float4,			/**< Vector of 4 floats */
	VET_UByte4,			/**< Vector of 4 unsigned bytes */
	VET_UByte4N,		/**< Vector of 4 unsigned bytes normalized */
	VET_Color,			/**< Color type */
	VET_Max
};

/**
 * @ingroup Engine
 * @brief Enumeration of vertex element usage
 */
enum EVertexElementUsage
{
	VEU_Position,					/**< Position usage */
	VEU_TextureCoordinate,			/**< Texture coordinate usage */
	VEU_BlendWeight,				/**< Blend wight usage */
	VEU_BlendIndices,				/**< Blend indices usage */
	VEU_Normal,						/**< Normal usage */
	VEU_Tangent,					/**< Tangent usage */
	VEU_Binormal,					/**< Binormal usage */
	VEU_Color,						/**< Color usage */
	VEU_SplitPos					/**< Split position usage */
};

/**
 * @ingroup Engine
 * @brief Struct of vertex element
 */
struct FVertexElement
{
	uint32			stride;						/**< Stride element */
	byte			streamIndex;				/**< Index of buffer */
	byte			offset;						/**< Offset in buffer */
	byte			type;						/**< Type element */
	byte			usage;						/**< Usage type */
	byte			usageIndex;					/**< Usage index */
	bool			isUseInstanceIndex;			/**< Is use instance index */
	uint32			numVerticesPerInstance;		/**< Number vertices per one instance */

	/**
	 * @brief Constructor
	 */
	FVertexElement() :
		streamIndex( 0 ),
		stride( 0 ),
		offset( 0 ),
		type( 0 ),
		usage( 0 ),
		usageIndex( 0 ),
		isUseInstanceIndex( false ),
		numVerticesPerInstance( 0 )
	{}

	/**
	 * @brief Constructor
	 * 
	 * @param[in] InStreamIndex Index of buffer
	 * @param[in] InStride Stride element
	 * @param[in] InOffset Offset in buffer
	 * @param[in] InType Type element
	 * @param[in] InUsage Usage type
	 * @param[in] InUsageIndex Usage index
	 * @param[in] InIsUseInstanceIndex Is use instance index
	 * @param[in] InNumVerticesPerInstance Number vertices per one instance
	 */
	FVertexElement( byte InStreamIndex, uint32 InStride, byte InOffset, byte InType, byte InUsage, byte InUsageIndex, bool InIsUseInstanceIndex = false, uint32 InNumVerticesPerInstance = 0 ) :
		streamIndex( InStreamIndex ),
		stride( InStride ),
		offset( InOffset ),
		type( InType ),
		usage( InUsage ),
		usageIndex( InUsageIndex ),
		isUseInstanceIndex( InIsUseInstanceIndex ),
		numVerticesPerInstance( InNumVerticesPerInstance )
	{}
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
	 * @param[in] InShaderName Shader name
	 */
	explicit									FBaseShaderRHI( EShaderFrequency InFrequency, const tchar* InShaderName ) :
		frequency( InFrequency )
#if !SHIPPING_BUILD
		, shaderName( InShaderName )
#endif // !SHIPPING_BUILD
	{}

	/**
	 * @brief Get shader name
	 * @return Return shader name
	 */
	FORCEINLINE const tchar*					GetShaderName() const
	{
#if !SHIPPING_BUILD
		return shaderName.c_str();
#else
		return TEXT( "" );
#endif // !SHIPPING_BUILD
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
#if !SHIPPING_BUILD
	std::wstring				shaderName;		/**< Shader name */
#endif // !SHIPPING_BUILD

	EShaderFrequency			frequency;		/**< Frequency of shader */
};

/**
 * @ingroup Engine Engine
 * @brief Element list for vertex declaration
 */
typedef std::vector< FVertexElement >		FVertexDeclarationElementList;

/**
 * @ingroup Engine
 * @breif Base class of vertex declaration
 */
class FBaseVertexDeclarationRHI : public FRefCounted
{
public:
	/**
	 * @brief Constructor
	 */
	FBaseVertexDeclarationRHI( const FVertexDeclarationElementList& InElementList )					
	{}
};

/**
 * @ingroup Engine
 * @brief Base class of bound shader state
 */
class FBoundShaderStateRHI : public FRefCounted
{};

#endif // !BASESHADERRHI_H