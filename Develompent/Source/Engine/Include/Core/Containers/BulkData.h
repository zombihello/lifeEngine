/**
 * @file
 * @addtogroup Core Core
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

#ifndef BULKDATA_H
#define BULKDATA_H

#include <vector>

#include "Core/System/Archive.h"
#include "Core/Misc/Misc.h"
#include "Core/Core.h"

/**
 * @ingroup Core
 * Container for store bulk data in archive
 */
template< typename TType >
class CBulkData
{
public:
	/**
	 * Constructor
	 * 
	 * @param[in] InFlags Compression flags (see ECompressionFlags)
	 */
	FORCEINLINE CBulkData( ECompressionFlags InFlags = CF_ZLIB ) : compressionFlags( InFlags )
	{}

	/**
	 * Add element
	 * 
	 * @param[in] InElement New element
	 */
	FORCEINLINE void AddElement( const TType& InElement )
	{
		data.push_back( InElement );
	}

	/**
	 * Remove element
	 * 
	 * @param[in] InIndex Index element to remove
	 */
	FORCEINLINE void RemoveElement( uint32 InIndex )
	{
		data.erase( data.begin() + InIndex );
	}

	/**
	 * Remove all elements
	 */
	FORCEINLINE void RemoveAllElements()
	{
		data.clear();
	}

	/**
	 * Serialize to archive
	 * 
	 * @param[in] InArchive Archive
	 */
	void Serialize( CArchive& InArchive )
	{
		if ( InArchive.Ver() < VER_CompressedZlib )
		{
			return;
		}

		uint32			sizeData = data.size();
		InArchive << sizeData;

		if ( InArchive.IsLoading() )
		{
			data.resize( sizeData );
		}
		InArchive.SerializeCompressed( data.data(), sizeof( TType ) * sizeData, compressionFlags );
	}

	/**
	 * Resize array of bulk data
	 * 
	 * @param[in] InNewSize New size of array
	 */
	FORCEINLINE void Resize( uint32 InNewSize )
	{
		data.resize( InNewSize );
	}

	/**
	 * Set elements in array
	 * 
	 * @param[in] InData Pointer to array of data
	 * @param[in] InSize Size array of data
	 */
	FORCEINLINE void SetElements( const TType* InData, uint32 InSize )
	{
		data.resize( InSize );
		memcpy( data.data(), InData, sizeof( TType ) * InSize );
	}

	/**
	 * Set compression flags
	 * 
	 * @param[in] InFlags Flags of compression (see ECompressionFlags)
	 */
	FORCEINLINE void SetCompressionFlags( ECompressionFlags InFlags )
	{
		compressionFlags = InFlags;
	}

	/**
	 * Get compression flags
	 * @return Return compression flags this bulk data
	 */
	FORCEINLINE ECompressionFlags GetCompressionFlags() const
	{
		return compressionFlags;
	}

	/**
	 * Get pointer to begin array
	 * 
	 * @return Return pointer to begin array, if array is empty return nullptr
	 */
	FORCEINLINE TType* GetData()
	{
		return Num() > 0 ? data.data() : nullptr;
	}

	/**
	 * Get pointer to begin array
	 *
	 * @return Return pointer to begin array, if array is empty return nullptr
	 */
	FORCEINLINE const TType* GetData() const
	{
		return Num() > 0 ? data.data() : nullptr;
	}

	/**
	 * Get element
	 * 
	 * @param[in] InIndex Index element
	 * @return Return element containing in data array
	 */
	FORCEINLINE const TType& GetElement( uint32 InIndex ) const
	{
		return data[ InIndex ];
	}

	/**
	 * Get STD container
	 * @return Return STD container containing data
	 */
	FORCEINLINE const std::vector<TType>& GetStdContainer() const
	{
		return data;
	}

	/**
	 * Get element
	 *
	 * @param[in] InIndex Index element
	 * @return Return element containing in data array
	 */
	FORCEINLINE TType& GetElement( uint32 InIndex )
	{
		return data[ InIndex ];
	}

	/**
	 * Get number of data
	 * @return Return number of data
	 */
	FORCEINLINE uint32 Num() const
	{
		return data.size();
	}

	/**
	 * Operator of copy
	 */
	FORCEINLINE CBulkData<TType>& operator=( const std::vector<TType>& InOther )
	{
		data = InOther;
		return *this;
	}

private:
	ECompressionFlags				compressionFlags;		/**< Compression flags (see ECompressionFlags) */
	std::vector< TType >			data;					/**< Array data */
};

//
// Overloaded operators for serialize in archive
//

template< typename TType >
FORCEINLINE CArchive& operator<<( CArchive& InArchive, CBulkData< TType >& InValue )
{
	InValue.Serialize( InArchive );
	return InArchive;
}

template< typename TType >
FORCEINLINE CArchive& operator<<( CArchive& InArchive, const CBulkData< TType >& InValue )
{
	Assert( InArchive.IsSaving() );
	InValue.Serialize( InArchive );
	return InArchive;
}

#endif // !BULKDATA_H