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
class FBoundShaderStateKey
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
	FBoundShaderStateKey( FVertexDeclarationRHIParamRef InVertexDeclaration, FVertexShaderRHIParamRef InVertexShader, FPixelShaderRHIParamRef InPixelShader, FHullShaderRHIParamRef InHullShader = nullptr, FDomainShaderRHIParamRef InDomainShader = nullptr, FGeometryShaderRHIParamRef InGeometryShader = nullptr );

	/**
	 * @brief Get hash of key
	 * @return Return hash of this key
	 */
	FORCEINLINE uint32 GetHash() const
	{
		return hash;
	}

	/**
	 * @brief Overrload operator '<'
	 * @return Return true if key less second key
	 */
	FORCEINLINE bool operator<( const FBoundShaderStateKey& InRight ) const
	{
		return hash < InRight.hash;
	}

	/**
	 * @brief Overrload operator '=='
	 * @return Return true if key equal second key
	 */
	FORCEINLINE bool operator==( const FBoundShaderStateKey& InRight ) const
	{
		return hash == InRight.hash && vertexDeclaration == InRight.vertexDeclaration && vertexShader == InRight.vertexShader && pixelShader == InRight.pixelShader && hullShader == InRight.hullShader && domainShader == InRight.domainShader && geometryShader == InRight.geometryShader;
	}

private:
	uint32								hash;					/**< Hash of key */
	FVertexDeclarationRHIParamRef		vertexDeclaration;		/**< Vertex decl for this combination */
	FVertexShaderRHIParamRef			vertexShader;			/**< VS for this combination */
	FPixelShaderRHIParamRef				pixelShader;			/**< PS for this combination */
	FHullShaderRHIParamRef				hullShader;				/**< HS for this combination */
	FDomainShaderRHIParamRef			domainShader;			/**< DS for this combination */
	FGeometryShaderRHIParamRef			geometryShader;			/**< GS for this combination */
};

/**
 * @ingroup Engine
 * @brief A list of the most recently used bound shader states.
 */
class FBoundShaderStateHistory
{
public:
	/**
	 * @brief Adds a bound shader state to the history
	 * 
	 * @param[in] InKey Key of bound shader state
	 * @param[in] InBoundShaderState Bound shader state
	 */
	FORCEINLINE void Add( const FBoundShaderStateKey& InKey, FBoundShaderStateRHIParamRef InBoundShaderState )
	{
		boundShaderStateMap[ InKey ] = InBoundShaderState;
	}

	/**
	 * @brief Find bound shadet state by key
	 * 
	 * @param[in] InKey Key of bound shader state
	 * @return Return pointer to bound shader state. If not found return nullptr
	 */
	FORCEINLINE FBoundShaderStateRHIRef Find( const FBoundShaderStateKey& InKey ) const
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
	FORCEINLINE void Remove( const FBoundShaderStateKey& InKey )
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
	 * @brief key hasher for using FBoundShaderStateKey in std::unordered_map
	 */
	struct FBoundShaderStateKeyHasher
	{
		/**
		 * @brief Get hash from FBoundShaderStateKey
		 */
		FORCEINLINE std::size_t operator()( const FBoundShaderStateKey& InKey ) const
		{
			return InKey.GetHash();
		}
	};

	std::unordered_map< FBoundShaderStateKey, FBoundShaderStateRHIParamRef, FBoundShaderStateKeyHasher >			boundShaderStateMap;
};

#endif // !BOUNDSHADERSTATECACHE_H