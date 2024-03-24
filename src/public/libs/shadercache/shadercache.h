/**
 * @file
 * @addtogroup shadercache shadercache
 *
 * ************************************************************
 *                  This file is part of:
 *                      LIFEENGINE
 *          https://github.com/zombihello/lifeEngine
 * ************************************************************
 * Copyright (C) 2024 Yehor Pohuliaka.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef SHADERCACHE_H
#define SHADERCACHE_H

#include <vector>
#include <unordered_map>

#include "core/types.h"
#include "core/platform.h"
#include "studiorender/studioapi/istudioshader.h"
#include "materialsystem/ishader.h"
#include "materialsystem/ivertexfactory.h"

// Forward declarations
class IFileReader;
class IFileWriter;

/**
 * @ingroup shadercache
 * @brief A map of shader parameter names to registers allocated to that parameter
 */
class CShaderParameterMap : public IShaderParameterMap
{
public:
	/**
	 * @brief Parameter allocation
	 */
	struct ParameterAllocation
	{
		/**
		 * @brief Construct a new ParameterAllocation object
		 */
		ParameterAllocation() :
			bBound( false )
		{}

		uint32			bufferIndex;		/**< Buffer index */
		uint32			baseIndex;			/**< Base index */
		uint32			size;				/**< Size parameter */
		uint32			samplerIndex;		/**< Sampler index */
		mutable bool 	bBound;				/**< Is bound */
	};

	/**
	 * @brief Find parameter allocation
	 *
	 * @param pParameterName	Parameter name
	 * @param bufferIndex		Output buffer index
	 * @param baseIndex			Output base index
	 * @param size				Output size
	 * @param samplerIndex		Output sampler index
	 * @return Return TRUE if parameter is found, otherwise FALSE
	 */
	virtual bool FindParameterAllocation( const achar* pParameterName, uint32& bufferIndex, uint32& baseIndex, uint32& size, uint32& samplerIndex ) const override;

	/**
	 * @brief Add parameter allocation
	 *
	 * @param pParameterName	Parameter name
	 * @param bufferIndex		Buffer index
	 * @param baseIndex			Base index
	 * @param size				Size
	 * @param samplerIndex		Sampler index
	 */
	void AddParameterAllocation( const achar* pParameterName, uint32 bufferIndex, uint32 baseIndex, uint32 size, uint32 samplerIndex );

	/**
	 * @brief Write data to file
	 * @param pFile		File
	 */
	void Write( IFileWriter* pFile ) const;

	/**
	 * @brief Read data from file
	 * @param pFile		File
	 */
	void Read( IFileReader* pFile );

	/**
	 * @brief Get parameter map
	 * @return Return parameter map
	 */
	FORCEINLINE const std::unordered_map<std::string, ParameterAllocation>& GetParameterMap() const
	{
		return parameterMap;
	}

private:
	std::unordered_map<std::string, ParameterAllocation>		parameterMap;		/**< Parameter map */
};

/**
 * @ingroup shadercache
 * @brief Shader cache element
 */
struct ShaderCacheElement
{
	/**
	 * @brief Constructor
	 */
	ShaderCacheElement()
		: numInstructions( 0 )
	{}

	/**
	 * @brief Write data to file
	 * @param pFile		File
	 */
	void Write( IFileWriter* pFile ) const;

	/**
	 * @brief Read data from file
	 * @param pFile		File
	 */
	void Read( IFileReader* pFile );

	std::vector<byte>		bytecode;			/**< Byte code of shader */
	uint32					numInstructions;	/**< Number instructions in shader */
	CShaderParameterMap		parameterMap;		/**< Parameter map */
};

/**
 * @ingroup shadercache
 * @brief Helper for work with shader cache files
 */
class CShaderCacheDoc
{
public:
	/**
	 * @brief Constructor
	 */
	CShaderCacheDoc();

	/**
	 * @brief Save shader cache document
	 * @warning g_pFileSystem must be valid
	 *
	 * @param pPath		Path to save
	 * @return Return TRUE if shader cache was successes saved, otherwise return FALSE
	 */
	bool Save( const achar* pPath );

	/**
	 * @brief Load shader cache document
	 * @warning g_pFileSystem must be valid
	 *
	 * @param pPath		Path to file
	 * @return Return TRUE if shader cache was successes loaded, otherwise return FALSE
	 */
	bool Load( const achar* pPath );

	/**
	 * @brief Clear shader cache
	 */
	FORCEINLINE void Clear()
	{
		for ( uint32 index = 0; index < VERTEXFACTORY_NUM_TYPES; ++index )
		{
			elements[index].clear();
		}
	}

	/**
	 * @brief Add shader cache
	 * 
	 * @param cache		Shader cache element
	 * @param vfType	Vertex factory type. If IsUseVertexFactory is FALSE must be -1
	 */
	FORCEINLINE void AddShaderCache( const ShaderCacheElement& cache, EVertexFactoryType vfType = ( EVertexFactoryType )-1 )
	{
		Assert( ( IsUseVertexFactory() && vfType != ( EVertexFactoryType )-1 ) || ( !IsUseVertexFactory() && vfType == ( EVertexFactoryType )-1 ) );
		std::vector<ShaderCacheElement>&		vfElements = elements[IsUseVertexFactory() ? ( uint32 )vfType : 0];
		vfElements.push_back( cache );
	}

	/**
	 * @brief Set shader type
	 * @param shaderType	Shader type
	 */
	FORCEINLINE void SetShaderType( EStudioShaderType shaderType )
	{
		CShaderCacheDoc::shaderType = shaderType;
	}

	/**
	 * @brief Get shader type
	 * @return Return shader type
	 */
	FORCEINLINE EStudioShaderType GetShaderType() const
	{
		return shaderType;
	}

	/**
	 * @brief Set using any vertex factory
	 * @param bUse	Is this shader cache using any vertex factory
	 */
	FORCEINLINE void SetUseVertexFactory( bool bUse )
	{
		bUseVertexFactory = bUse;
	}

	/**
	 * @brief Is using any vertex factory
	 * @return Return TRUE if this cache is using any vertex factory
	 */
	FORCEINLINE bool IsUseVertexFactory() const
	{
		return bUseVertexFactory;
	}

	/**
	 * @brief Get number of shader caches for vertex factory type
	 * 
	 * @param vfType	Vertex factory type. If IsUseVertexFactory is FALSE must be -1
	 * @return Return number of shader cache for vfType
	 */
	FORCEINLINE uint32 GetNumShaderCaches( EVertexFactoryType vfType = ( EVertexFactoryType )-1 ) const
	{
		Assert( ( IsUseVertexFactory() && vfType != ( EVertexFactoryType )-1 ) || ( !IsUseVertexFactory() && vfType == ( EVertexFactoryType )-1 ) );
		return ( uint32 )elements[IsUseVertexFactory() ? ( uint32 )vfType : 0].size();
	}

	/**
	 * @brief Get shader cache
	 * 
	 * @param index		Shader cache index
	 * @param vfType	Vertex factory type. If IsUseVertexFactory is FALSE must be -1
	 * @return Return shader cache at index
	 */
	FORCEINLINE const ShaderCacheElement& GetShaderCache( uint32 index, EVertexFactoryType vfType = ( EVertexFactoryType )-1 ) const
	{
		Assert( ( IsUseVertexFactory() && vfType != ( EVertexFactoryType )-1 ) || ( !IsUseVertexFactory() && vfType == ( EVertexFactoryType )-1 ) );
		const std::vector<ShaderCacheElement>&		vfElements = elements[IsUseVertexFactory() ? ( uint32 )vfType : 0];
		Assert( index >= 0 && index < vfElements.size() );
		return vfElements[index];
	}

	/**
	 * @brief Is empty shader cache
	 * @return Return TRUE if shader cache is empty, otherwise returns FALSE
	 */
	FORCEINLINE bool IsEmpty() const
	{
		for ( uint32 index = 0; index < VERTEXFACTORY_NUM_TYPES; ++index )
		{
			if ( !elements[index].empty() )
			{
				return false;
			}
		}

		return true;
	}

private:
	bool								bUseVertexFactory;					/**< Is using any vertex factory */
	EStudioShaderType					shaderType;							/**< Shader type */
	std::vector<ShaderCacheElement>		elements[VERTEXFACTORY_NUM_TYPES];	/**< Variations of shader array */
};

#endif // !SHADERCACHE_H