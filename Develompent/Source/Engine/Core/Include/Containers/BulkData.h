/**
 * @file
 * @addtogroup Core Core
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef BULKDATA_H
#define BULKDATA_H

#include <vector>

#include "System/Archive.h"
#include "Misc/Misc.h"
#include "Core.h"

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
	check( InArchive.IsSaving() );
	InValue.Serialize( InArchive );
	return InArchive;
}

#endif // !BULKDATA_H