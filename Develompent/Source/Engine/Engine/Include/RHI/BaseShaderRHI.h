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
#include "Render/BoundShaderStateCache.h"
#include "RHI/TypesRHI.h"
#include "RHI/BaseResourceRHI.h"
#include "System/Archive.h"

/**
 * @ingroup Engine
 * @brief Enumeration of types shader
 */
enum EShaderFrequency
{
	SF_Vertex,											/**< Vertex shader */
	SF_Hull,											/**< Hull shader */
	SF_Domain,											/**< Domain shader */
	SF_Pixel,											/**< Pixel shader */
	SF_Geometry,										/**< Geometry shader */
	SF_Compute,											/**< Compute shader */
	SF_NumFrequencies,									/**< Count frequencies */
	SF_NumDrawFrequencies	= SF_NumFrequencies - 1		/**< Count frequencies part of pipeline (without compute shader) */
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
struct SVertexElement
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
	SVertexElement() :
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
	SVertexElement( byte InStreamIndex, uint32 InStride, byte InOffset, byte InType, byte InUsage, byte InUsageIndex, bool InIsUseInstanceIndex = false, uint32 InNumVerticesPerInstance = 0 ) :
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
class CBaseShaderRHI : public CBaseResourceRHI
{
public:
	/**
	 * @brief Constructor
	 * @param[in] InFrequency Frequency of shader
	 * @param[in] InShaderName Shader name
	 */
	explicit CBaseShaderRHI( EShaderFrequency InFrequency, const tchar* InShaderName ) 
		: frequency( InFrequency )
#if !SHIPPING_BUILD
		, shaderName( InShaderName )
#endif // !SHIPPING_BUILD
	{}

	/**
	 * @brief Get shader name
	 * @return Return shader name
	 */
	FORCEINLINE const tchar* GetShaderName() const
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
	FORCEINLINE EShaderFrequency GetFrequency() const
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
typedef std::vector< SVertexElement >		VertexDeclarationElementList_t;

/**
 * @ingroup Engine
 * @breif Base class of vertex declaration
 */
class CBaseVertexDeclarationRHI : public CBaseResourceRHI
{
public:
	/**
	 * @brief Constructor
	 */
	CBaseVertexDeclarationRHI( const VertexDeclarationElementList_t& InElementList )					
	{}

	/**
	 * @brief Destructor 
	 */
	virtual ~CBaseVertexDeclarationRHI()
	{}

	/**
	 * @brief Get hash
	 * 
	 * @param[in] InHash Start hash
	 * @return Return calculated hash
	 */
	virtual uint64 GetHash( uint64 InHash = 0 ) const { return 0; }
};

/**
 * @ingroup Engine
 * @brief Base class of bound shader state
 */
class CBaseBoundShaderStateRHI : public CBaseResourceRHI
{
public:
	/**
	 * Constructor
	 * 
	 * @param[in] InKey Key of bound shader state
	 * @param[in] InVertexDeclaration Vertex declaration
	 * @param[in] InVertexShader Vertex shader
	 * @param[in] InPixelShader Pixel shader
	 * @param[in] InHullShader Hull shader
	 * @param[in] InDomainShader Domain shader
	 * @param[in] InGeometryShader Geometry shader
	 */
	CBaseBoundShaderStateRHI( const CBoundShaderStateKey& InKey, VertexDeclarationRHIRef_t InVertexDeclaration, VertexShaderRHIRef_t InVertexShader, PixelShaderRHIRef_t InPixelShader, HullShaderRHIRef_t InHullShader = nullptr, DomainShaderRHIRef_t InDomainShader = nullptr, GeometryShaderRHIRef_t InGeometryShader = nullptr ) :
		key( InKey ),
		vertexDeclaration( InVertexDeclaration ),
		vertexShader( InVertexShader ),
		pixelShader( InPixelShader ),
		hullShader( InHullShader ),
		domainShader( InDomainShader ),
		geometryShader( InGeometryShader )
	{}

	/**
	 * @brief Destructor
	 */
	virtual ~CBaseBoundShaderStateRHI()
	{}

	/**
	 * @brief Get vertex declaration
	 * @return Pointer to vertex declaration
	 */
	FORCEINLINE VertexDeclarationRHIRef_t GetVertexDeclaration() const
	{
		return vertexDeclaration;
	}

	/**
	 * @brief Get vertex shader
	 * @return Pointer to vertex shader
	 */
	FORCEINLINE VertexShaderRHIRef_t GetVertexShader() const
	{
		return vertexShader;
	}

	/**
	 * @brief Get pixel shader
	 * @return Pointer to pixel shader
	 */
	FORCEINLINE PixelShaderRHIRef_t GetPixelShader() const
	{
		return pixelShader;
	}

	/**
	 * @brief Get hull shader
	 * @return Pointer to hull shader
	 */
	FORCEINLINE HullShaderRHIRef_t GetHullShader() const
	{
		return hullShader;
	}

	/**
	 * @brief Get domain shader
	 * @return Pointer to domain shader
	 */
	FORCEINLINE DomainShaderRHIRef_t GetDomainShader() const
	{
		return domainShader;
	}

	/**
	 * @brief Get geometry shader
	 * @return Pointer to geometry shader
	 */
	FORCEINLINE GeometryShaderRHIRef_t GetGeometryShader() const
	{
		return geometryShader;
	}

	/**
	 * @brief Get hash
	 * @return Return calculated hash
	 */
	FORCEINLINE uint64 GetHash() const			
	{ 
		return key.GetHash(); 
	}

protected:
	CBoundShaderStateKey			key;					/**< Bound shader state key */
	VertexDeclarationRHIRef_t		vertexDeclaration;		/**< Vertex declaration */
	VertexShaderRHIRef_t			vertexShader;			/**< Vertex shader */
	PixelShaderRHIRef_t				pixelShader;			/**< Pixel shader */
	HullShaderRHIRef_t				hullShader;				/**< Hull shader */
	DomainShaderRHIRef_t			domainShader;			/**< Domain shader */
	GeometryShaderRHIRef_t			geometryShader;			/**< Geometry shader */
};

//
// Overloaded operators for serialize in archive
//

FORCEINLINE CArchive& operator<<( CArchive& InArchive, EShaderFrequency& InValue )
{
	InArchive.Serialize( &InValue, sizeof( InValue ) );
	return InArchive;
}

FORCEINLINE CArchive& operator<<( CArchive& InArchive, const EShaderFrequency& InValue )
{
	Assert( InArchive.IsSaving() );
	InArchive.Serialize( ( void* ) &InValue, sizeof( InValue ) );
	return InArchive;
}

#endif // !BASESHADERRHI_H