/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright BSOD-Games, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef BOUNDSHADERSTATECACHE_H
#define BOUNDSHADERSTATECACHE_H

#include <unordered_map>
#include "RHI/TypesRHI.h"

/**
 * @ingroup Engine
 * @brief Key used to map a set of unique decl/vs/ps combinations to a vertex shader resource
 */
class CBoundShaderStateKey
{
public:
	/**
	 * @brief Constructor
	 * 
	 * @param[in] InVertexDeclaration Vertex declaration
	 * @param[in] InVertexShader Vertex shader
	 * @param[in] InPixelShader Pixel shader
	 * @param[in] InHullShader Hull shader
	 * @param[in] InDomainShader Domain shader
	 * @param[in] InGeometryShader Geometry shader
	 */
	CBoundShaderStateKey( VertexDeclarationRHIParamRef_t InVertexDeclaration, VertexShaderRHIParamRef_t InVertexShader, PixelShaderRHIParamRef_t InPixelShader, HullShaderRHIParamRef_t InHullShader = nullptr, DomainShaderRHIParamRef_t InDomainShader = nullptr, GeometryShaderRHIParamRef_t InGeometryShader = nullptr );

	/**
	 * @brief Get hash of key
	 * @return Return hash of this key
	 */
	FORCEINLINE uint64 GetHash() const
	{
		return hash;
	}

	/**
	 * @brief Overrload operator '<'
	 * @return Return true if key less second key
	 */
	FORCEINLINE bool operator<( const CBoundShaderStateKey& InRight ) const
	{
		return hash < InRight.hash;
	}

	/**
	 * @brief Overrload operator '=='
	 * @return Return true if key equal second key
	 */
	FORCEINLINE bool operator==( const CBoundShaderStateKey& InRight ) const
	{
		return hash == InRight.hash && vertexDeclaration == InRight.vertexDeclaration && vertexShader == InRight.vertexShader && pixelShader == InRight.pixelShader && hullShader == InRight.hullShader && domainShader == InRight.domainShader && geometryShader == InRight.geometryShader;
	}

private:
	uint64								hash;					/**< Hash of key */
	VertexDeclarationRHIParamRef_t		vertexDeclaration;		/**< Vertex decl for this combination */
	VertexShaderRHIParamRef_t			vertexShader;			/**< VS for this combination */
	PixelShaderRHIParamRef_t			pixelShader;			/**< PS for this combination */
	HullShaderRHIParamRef_t				hullShader;				/**< HS for this combination */
	DomainShaderRHIParamRef_t			domainShader;			/**< DS for this combination */
	GeometryShaderRHIParamRef_t			geometryShader;			/**< GS for this combination */
};

/**
 * @ingroup Engine
 * @brief A list of the most recently used bound shader states.
 */
class CBoundShaderStateHistory
{
public:
	/**
	 * @brief Adds a bound shader state to the history
	 * 
	 * @param[in] InKey Key of bound shader state
	 * @param[in] InBoundShaderState Bound shader state
	 */
	FORCEINLINE void Add( const CBoundShaderStateKey& InKey, BoundShaderStateRHIParamRef_t InBoundShaderState )
	{
		boundShaderStateMap[ InKey ] = InBoundShaderState;
	}

	/**
	 * @brief Find bound shadet state by key
	 * 
	 * @param[in] InKey Key of bound shader state
	 * @return Return pointer to bound shader state. If not found return nullptr
	 */
	FORCEINLINE BoundShaderStateRHIRef_t Find( const CBoundShaderStateKey& InKey ) const
	{
		auto		itBoundShaderState = boundShaderStateMap.find( InKey );
		if ( itBoundShaderState == boundShaderStateMap.end() )
		{
			return nullptr;
		}

		return itBoundShaderState->second;
	}

	/**
	 * @brief Remove bound shader state from history
	 * 
	 * @param[in] InKey Key of bound shader state
	 */
	FORCEINLINE void Remove( const CBoundShaderStateKey& InKey )
	{
		boundShaderStateMap.erase( InKey );
	}

	/**
	 * @brief Remove all bound shader states from history
	 */
	FORCEINLINE void RemoveAll()
	{
		boundShaderStateMap.clear();
	}

private:
	/**
	 * @brief key hasher for using CBoundShaderStateKey in std::unordered_map
	 */
	struct BoundShaderStateKeyHasher
	{
		/**
		 * @brief Get hash from CBoundShaderStateKey
		 */
		FORCEINLINE std::size_t operator()( const CBoundShaderStateKey& InKey ) const
		{
			return InKey.GetHash();
		}
	};

	std::unordered_map< CBoundShaderStateKey, BoundShaderStateRHIParamRef_t, BoundShaderStateKeyHasher >			boundShaderStateMap;
};

#endif // !BOUNDSHADERSTATECACHE_H