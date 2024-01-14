/**
 * @file
 * @addtogroup Core Core
 *
 * Copyright BSOD-Games, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef Archive_H
#define Archive_H

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>

#include "Core.h"
#include "Misc/Types.h"
#include "Misc/Misc.h"

/**
 * @ingroup Core
 * Enumeration of type archive
 */
enum EArchiveType
{
	AT_TextFile,		/**< Archive is text file */
	AT_ShaderCache,		/**< Archive contains shader cache */
	AT_TextureCache,	/**< DEPRECATED. Archive contains texture cache */
	AT_World,			/**< Archive contains world */
	AT_Package,			/**< Archive contains assets */
	AT_BinaryFile,		/**< Archive is unknown binary file */
	AT_Scripts			/**< Archive contains scripts */
};

/**
 * @ingroup Core
 * @brief The base class for work with archive
 */
class CArchive
{
public:
	/**
	 * Constructor
	 * 
	 * @param InPath Path to archive
	 */
	CArchive( const std::wstring& InPath );

	/**
	 * @brief Destructor
	 */
	virtual ~CArchive() {}

	/**
	 * @brief Serialize data
	 * 
	 * @param[in] InBuffer Pointer to buffer for serialize
	 * @param[in] InSize Size of buffer
	 */
	virtual void Serialize( void* InBuffer, uint32 InSize ) {}

	/**
	 * @brief Serialize compression data
	 * 
	 * @param[in] InBuffer Pointer to buffer for serialize
	 * @param[in] InSize Size of buffer
	 * @param[in] InFlags Compression flags (see ECompressionFlags)
	 */
	void SerializeCompressed( void* InBuffer, uint32 InSize, ECompressionFlags InFlags );

	/**
	 * @brief Serialize archive header
	 */
	void SerializeHeader();

	/**
	 * @brief Get current position in archive
	 * @return Current position in archive
	 */
	virtual uint32 Tell() { return 0; };

	/**
	 * @brief Set current position in archive
	 * 
	 * @param[in] InPosition New position in archive
	 */
	virtual void Seek( uint32 InPosition ) {}

	/**
	 * @brief Flush data
	 */
	virtual void Flush() {}

	/**
	 * Set archive type
	 * 
	 * @param[in] InType Archive type
	 */
	FORCEINLINE void SetType( EArchiveType InType )
	{
		arType = InType;
	}

	/**
	 * @brief Is saving archive
	 * @return True if archive saving, false if archive loading
	 */
	virtual bool IsSaving() const { return false; }

	/**
	 * @breif Is loading archive
	 * @return True if archive loading, false if archive saving
	 */
	virtual bool IsLoading() const { return false; }

	/**
	 * Is end of file
	 * @return Return true if end of file, else return false
	 */
	virtual bool IsEndOfFile() { return false; }

	/**
	 * @brief Get size of archive
	 * @return Size of archive
	 */
	virtual uint32 GetSize() { return 0; }

	/**
	 * @brief Set archive version
	 * @param InVer		New archive version
	 */
	FORCEINLINE void SetVer( uint32 InVer )
	{
		arVer = InVer;
	}

	/**
	 * Get archive version
	 * @return Return archive version
	 */
	FORCEINLINE uint32 Ver() const
	{
		return arVer;
	}

	/**
	 * Get archive type
	 * @return Return type archive
	 */
	FORCEINLINE EArchiveType Type() const
	{
		return arType;
	}

	/**
	 * Get path to archive
	 * @return Return path to archive
	 */
	FORCEINLINE const std::wstring& GetPath() const
	{
		return arPath;
	}

	/**
	 * @brief Override operator << for serialize CObjects
	 * @return Return reference to self
	 */
	virtual CArchive& operator<<( class CObject*& InValue )
	{
		AssertMsg( false, TEXT( "Default CArchive not support serialize CObject" ) );
		return *this;
	}

protected:
	uint32					arVer;			/**< Archive version (look ELifeEnginePackageVersion) */
	EArchiveType			arType;			/**< Archive type */
	std::wstring			arPath;			/**< Path to archive */
};

/**
 * @ingroup Core
 * @brief Archive for tagging objects that must be exported to the file
 * It tags the objects passed to it, and recursively tags all of the objects this object references
 */
class CArchiveSaveTagExports : public CArchive
{
public:
	/**
	 * @brief Constructor
	 * @param InOuter	The package to save
	 */
	CArchiveSaveTagExports( class CObject* InOuter );

	/**
	 * @brief Is saving archive
	 * @return True if archive saving, false if archive loading
	 */
	virtual bool IsSaving() const override;

	/**
	 * @brief Override operator << for serialize CObjects
	 * @return Return reference to self
	 */
	virtual CArchive& operator<<( class CObject*& InValue ) override;

	/**
	 * @brief Serializes the specified object, tagging all objects it references
	 * @param InObject		The object that should be serialized, usually the package root
	 */
	void ProcessObject( class CObject* InObject );

private:
	/**
	 * @brief Process tagged objects
	 * 
	 * Iterates over all objects which were encountered during serialization of the root object, serializing each one in turn.
	 * Objects encountered during that serialization are then added to the array and iteration continues until no new objects are
	 * added to the array
	 */
	void ProcessTaggedObjects();

	std::vector<class CObject*>		taggedObjects;		/**< Tagged objects */
	class CObject*					outer;				/**< Package we're currently saving. Only objects contained within this package will be tagged for serialization */
};

/**
 * @ingroup Core
 * @brief Archive for tagging objects and names that must be listed in the file's imports table
 */
class CArchiveSaveTagImports : public CArchive
{
public:
	/**
	 * @brief Constructor
	 * @param InLinker	The package linker to save
	 */
	CArchiveSaveTagImports( class CLinkerSave* InLinker );

	/**
	 * @brief Is saving archive
	 * @return True if archive saving, false if archive loading
	 */
	virtual bool IsSaving() const override;

	/**
	 * @brief Override operator << for serialize CObjects
	 * @return Return reference to self
	 */
	virtual CArchive& operator<<( class CObject*& InValue ) override;

private:
	class CLinkerSave*		linker;		/**< The package linker to save */
};

/**
 * @ingroup Core
 * @brief Archive for collect all object references
 */
template<class TClass>
class TArchiveObjectReferenceCollector : public CArchive
{
public:
	/**
	 * @brief Constructor
	 * 
	 * @param InObjectArray				Array to add object references to
	 * @param InOuter					Limit outer
	 * @param InIsRequireDirectOuter	TRUE to skip objects whose GetOuter() isn't InOuter
	 * @param InIsSerializeRecursively	Only applicable when LimitOuter is valid and bRequireDirectOuter is set.
	 *									Serializes each object encountered looking for subobjects of referenced
	 *									objects that have LimitOuter for their Outer (i.e. nested subobjects/components)
	 * @param InIsShouldIgnoreTransient	TRUE to skip serialization of transient properties
	 */
	TArchiveObjectReferenceCollector( std::vector<TClass*>* InObjectArray, CObject* InOuter = nullptr, bool InIsRequireDirectOuter = true, bool InIsSerializeRecursively = false, bool InIsShouldIgnoreTransient = false )
		: CArchive( TEXT( "TArchiveObjectReferenceCollector" ) )
		, objectArray( InObjectArray )
		, limitOuter( InOuter )
		, bRequireDirectOuter( InIsRequireDirectOuter )
		, bSerializeRecursively( InIsSerializeRecursively && InOuter )
		, bShouldIgnoreTransient( InIsShouldIgnoreTransient )
	{}

protected:
	/**
	 * @brief Override operator << for serialize CObjects
	 * @return Return reference to self
	 */
	virtual CArchive& operator<<( class CObject*& InValue ) override
	{
		// Avoid duplicate entries and skip serialization of transient properties if it need
		if ( InValue && ( !bShouldIgnoreTransient || !InValue->HasAnyObjectFlags( OBJECT_Transient ) ) )
		{
			if ( !limitOuter || InValue->GetOuter() == limitOuter || ( !bRequireDirectOuter && IsIn( InValue, limitOuter ) ) )
			{
				// Do not attempt to serialize objects that have already been
				if ( IsA( InValue, TClass::StaticClass() ) && std::find( objectArray->begin(), objectArray->end(), ( TClass* )InValue ) == objectArray->end() )
				{
					objectArray->push_back( ( TClass* )InValue );
				}

				// Check this object for any potential object references
				if ( bSerializeRecursively && serializedObjects.find( InValue ) == serializedObjects.end() )
				{
					serializedObjects.insert( InValue );
					InValue->Serialize( *this );
				}
			}
		}

		return *this;
	}

	std::vector<TClass*>*			objectArray;			/**< Stored pointer to array of objects we add object references to */
	std::unordered_set<CObject*>	serializedObjects;		/**< List of objects that have been recursively serialized */
	CObject*						limitOuter;				/**< Only objects within this outer will be considered, NULL value indicates that outers are disregarded */
	bool							bRequireDirectOuter;	/**< Determines whether nested objects contained within LimitOuter are considered */
	bool							bSerializeRecursively;	/**< Determines whether we serialize objects that are encounterd by this archive */
	bool							bShouldIgnoreTransient;	/**< TRUE to skip serialization of transient properties */
};

/**
 * @ingroup Core
 * Helper structure for compression support, containing information on compressed
 * and uncompressed size of a chunk of data.
 */
struct CompressedChunkInfo
{
	uint32		compressedSize;			/**< Compressed size of data */
	uint32		uncompressedSize;		/**< Uncompresses size of data */
};

//
// Overloaded operators for serialize in archive
//

FORCEINLINE CArchive& operator<<( CArchive& InArchive, int8& InValue )
{
	InArchive.Serialize( &InValue, sizeof( InValue ) );
	return InArchive;
}

FORCEINLINE CArchive& operator<<( CArchive& InArchive, const int8& InValue )
{
	Assert( InArchive.IsSaving() );
	InArchive.Serialize( ( void* ) &InValue, sizeof( InValue ) );
	return InArchive;
}

FORCEINLINE CArchive& operator<<( CArchive& InArchive, uint8& InValue )
{
	InArchive.Serialize( &InValue, sizeof( InValue ) );
	return InArchive;
}

FORCEINLINE CArchive& operator<<( CArchive& InArchive, const uint8& InValue )
{
	Assert( InArchive.IsSaving() );
	InArchive.Serialize( ( void* ) &InValue, sizeof( InValue ) );
	return InArchive;
}

FORCEINLINE CArchive& operator<<( CArchive& InArchive, int16& InValue )
{
	InArchive.Serialize( &InValue, sizeof( InValue ) );
	return InArchive;
}

FORCEINLINE CArchive& operator<<( CArchive& InArchive, const int16& InValue )
{
	Assert( InArchive.IsSaving() );
	InArchive.Serialize( ( void* ) &InValue, sizeof( InValue ) );
	return InArchive;
}

FORCEINLINE CArchive& operator<<( CArchive& InArchive, uint16& InValue )
{
	InArchive.Serialize( &InValue, sizeof( InValue ) );
	return InArchive;
}

FORCEINLINE CArchive& operator<<( CArchive& InArchive, const uint16& InValue )
{
	Assert( InArchive.IsSaving() );
	InArchive.Serialize( ( void* ) &InValue, sizeof( InValue ) );
	return InArchive;
}

FORCEINLINE CArchive& operator<<( CArchive& InArchive, int32& InValue )
{
	InArchive.Serialize( &InValue, sizeof( InValue ) );
	return InArchive;
}

FORCEINLINE CArchive& operator<<( CArchive& InArchive, const int32& InValue )
{
	Assert( InArchive.IsSaving() );
	InArchive.Serialize( ( void* ) &InValue, sizeof( InValue ) );
	return InArchive;
}

FORCEINLINE CArchive& operator<<( CArchive& InArchive, uint32& InValue )
{
	InArchive.Serialize( &InValue, sizeof( InValue ) );
	return InArchive;
}

FORCEINLINE CArchive& operator<<( CArchive& InArchive, const uint32& InValue )
{
	Assert( InArchive.IsSaving() );
	InArchive.Serialize( ( void* ) &InValue, sizeof( InValue ) );
	return InArchive;
}

FORCEINLINE CArchive& operator<<( CArchive& InArchive, int64& InValue )
{
	InArchive.Serialize( &InValue, sizeof( InValue ) );
	return InArchive;
}

FORCEINLINE CArchive& operator<<( CArchive& InArchive, const int64& InValue )
{
	Assert( InArchive.IsSaving() );
	InArchive.Serialize( ( void* ) &InValue, sizeof( InValue ) );
	return InArchive;
}

FORCEINLINE CArchive& operator<<( CArchive& InArchive, uint64& InValue )
{
	InArchive.Serialize( &InValue, sizeof( InValue ) );
	return InArchive;
}

FORCEINLINE CArchive& operator<<( CArchive& InArchive, const uint64& InValue )
{
	Assert( InArchive.IsSaving() );
	InArchive.Serialize( ( void* ) &InValue, sizeof( InValue ) );
	return InArchive;
}

FORCEINLINE CArchive& operator<<( CArchive& InArchive, bool& InValue )
{
	InArchive.Serialize( &InValue, sizeof( InValue ) );
	return InArchive;
}

FORCEINLINE CArchive& operator<<( CArchive& InArchive, const bool& InValue )
{
	Assert( InArchive.IsSaving() );
	InArchive.Serialize( ( void* ) &InValue, sizeof( InValue ) );
	return InArchive;
}

FORCEINLINE CArchive& operator<<( CArchive& InArchive, float& InValue )
{
	InArchive.Serialize( &InValue, sizeof( InValue ) );
	return InArchive;
}

FORCEINLINE CArchive& operator<<( CArchive& InArchive, const float& InValue )
{
	Assert( InArchive.IsSaving() );
	InArchive.Serialize( ( void* ) &InValue, sizeof( InValue ) );
	return InArchive;
}

FORCEINLINE CArchive& operator<<( CArchive& InArchive, double& InValue )
{
	InArchive.Serialize( &InValue, sizeof( InValue ) );
	return InArchive;
}

FORCEINLINE CArchive& operator<<( CArchive& InArchive, const double& InValue )
{
	Assert( InArchive.IsSaving() );
	InArchive.Serialize( ( void* )&InValue, sizeof( InValue ) );
	return InArchive;
}

FORCEINLINE CArchive& operator<<( CArchive& InArchive, EArchiveType& InValue )
{
	InArchive.Serialize( &InValue, sizeof( InValue ) );
	return InArchive;
}

FORCEINLINE CArchive& operator<<( CArchive& InArchive, const EArchiveType& InValue )
{
	Assert( InArchive.IsSaving() );
	InArchive.Serialize( ( void* ) &InValue, sizeof( InValue ) );
	return InArchive;
}

FORCEINLINE CArchive& operator<<( CArchive& InArchive, CompressedChunkInfo& InValue )
{
	InArchive.Serialize( &InValue, sizeof( InValue ) );
	return InArchive;
}

FORCEINLINE CArchive& operator<<( CArchive& InArchive, const CompressedChunkInfo& InValue )
{
	Assert( InArchive.IsSaving() );
	InArchive.Serialize( ( void* ) &InValue, sizeof( InValue ) );
	return InArchive;
}

FORCEINLINE CArchive& operator<<( CArchive& InArchive, achar& InValue )
{
	InArchive.Serialize( &InValue, sizeof( InValue ) );
	return InArchive;
}

FORCEINLINE CArchive& operator<<( CArchive& InArchive, const achar& InValue )
{
	Assert( InArchive.IsSaving() );
	InArchive.Serialize( ( void* ) &InValue, sizeof( InValue ) );
	return InArchive;
}

FORCEINLINE CArchive& operator<<( CArchive& InArchive, tchar& InValue )
{
	InArchive.Serialize( &InValue, sizeof( InValue ) );
	return InArchive;
}

FORCEINLINE CArchive& operator<<( CArchive& InArchive, const tchar& InValue )
{
	Assert( InArchive.IsSaving() );
	InArchive.Serialize( ( void* ) &InValue, sizeof( InValue ) );
	return InArchive;
}

FORCEINLINE CArchive& operator<<( CArchive& InArchive, const tchar* InStringC )
{
	Assert( InArchive.IsSaving() );
	InArchive.Serialize( ( void* )InStringC, ( uint32 )wcslen( InStringC ) * 2 );
	return InArchive;
}

FORCEINLINE CArchive& operator<<( CArchive& InArchive, const achar* InStringC )
{
	Assert( InArchive.IsSaving() );
	InArchive.Serialize( ( void* )InStringC, ( uint32 )strlen( InStringC ) );
	return InArchive;
}

FORCEINLINE CArchive& operator<<( CArchive& InArchive, std::string& InValue )
{
	// If we serialize text file
	if ( InArchive.Type() == AT_TextFile )
	{
		InArchive.Serialize( InValue.data(), InValue.size() );
	}
	// Else we serialize binary archive
	else
	{
		uint32		stringSize = InValue.size();
		InArchive << stringSize;

		if ( stringSize > 0 )
		{
			if ( InArchive.IsLoading() )
			{
				InValue.resize( stringSize );
			}

			InArchive.Serialize( InValue.data(), stringSize );
		}
	}

	return InArchive;
}

FORCEINLINE CArchive& operator<<( CArchive& InArchive, const std::string& InValue )
{
	Assert( InArchive.IsSaving() );

	// If we serialize text file
	if ( InArchive.Type() == AT_TextFile )
	{
		InArchive.Serialize( ( void* )InValue.data(), InValue.size() );
	}
	// Else we serialize binary archive
	else
	{
		uint32		stringSize = InValue.size();
		InArchive << stringSize;

		if ( stringSize > 0 )
		{
			InArchive.Serialize( ( void* )InValue.data(), stringSize );
		}
	}

	return InArchive;
}

FORCEINLINE CArchive& operator<<( CArchive& InArchive, std::wstring& InValue )
{	
	// If we serialize text file
	if ( InArchive.Type() == AT_TextFile )
	{
		InArchive.Serialize( InValue.data(), InValue.size() * sizeof( std::wstring::value_type ) );
	}
	// Else we serialize binary archive
	else
	{
		uint32		stringSize = InValue.size() * sizeof( std::wstring::value_type );
		InArchive << stringSize;

		if ( stringSize > 0 )
		{
			if ( InArchive.IsLoading() )
			{
				InValue.resize( stringSize / sizeof( std::wstring::value_type ) );
			}

			InArchive.Serialize( InValue.data(), stringSize );
		}
	}

	return InArchive;
}

FORCEINLINE CArchive& operator<<( CArchive& InArchive, const std::wstring& InValue )
{
	Assert( InArchive.IsSaving() );

	// If we serialize text file
	if ( InArchive.Type() == AT_TextFile )
	{
		InArchive.Serialize( ( void* )InValue.data(), InValue.size() * sizeof( std::wstring::value_type ) );
	}
	// Else we serialize binary archive
	else
	{
		uint32		stringSize = InValue.size() * sizeof( std::wstring::value_type );
		InArchive << stringSize;

		if ( stringSize > 0 )
		{
			InArchive.Serialize( ( void* )InValue.data(), stringSize );
		}
	}

	return InArchive;
}

template< typename TType >
FORCEINLINE CArchive& operator<<( CArchive& InArchive, std::vector< TType >& InValue )
{
	if ( InArchive.IsLoading() && InArchive.Ver() < VER_StaticMesh )
	{
		return InArchive;
	}

	uint32		arraySize = InValue.size();
	InArchive << arraySize;

	if ( arraySize > 0 )
	{
		if ( InArchive.IsLoading() )
		{
			InValue.resize( arraySize );
		}

		for ( uint32 index = 0; index < arraySize; ++index )
		{
			InArchive << InValue[ index ];
		}
	}

	return InArchive;
}

template< typename TType >
FORCEINLINE CArchive& operator<<( CArchive& InArchive, const std::vector< TType >& InValue )
{
	Assert( InArchive.IsSaving() );

	uint32		arraySize = InValue.size();
	InArchive << arraySize;

	if ( arraySize > 0 )
	{
		for ( uint32 index = 0; index < arraySize; ++index )
		{
			InArchive << InValue[ index ];
		}
	}

	return InArchive;
}

template< typename TKey, typename TValue, typename THasher = std::hash<TKey> >
FORCEINLINE CArchive& operator<<( CArchive& InArchive, std::unordered_map< TKey, TValue, THasher >& InValue )
{
	if ( InArchive.IsLoading() && InArchive.Ver() < VER_ShaderMap )
	{
		return InArchive;
	}

	uint32		arraySize = InValue.size();
	InArchive << arraySize;

	if ( arraySize > 0 )
	{
		if ( InArchive.IsLoading() )
		{
			for ( uint32 index = 0; index < arraySize; ++index )
			{
				TKey		key;
				TValue		value;

				InArchive << key;
				InArchive << value;
				InValue[ key ] = value;
			}
		}
		else
		{
			for ( auto itValue = InValue.begin(), itValueEnd = InValue.end(); itValue != itValueEnd; ++itValue )
			{
				InArchive << itValue->first;
				InArchive << itValue->second;
			}
		}		
	}

	return InArchive;
}

template< typename TKey, typename TValue, typename THasher = std::hash<TKey> >
FORCEINLINE CArchive& operator<<( CArchive& InArchive, const std::unordered_map< TKey, TValue, THasher >& InValue )
{
	Assert( InArchive.IsSaving() );

	uint32		arraySize = InValue.size();
	InArchive << arraySize;

	if ( arraySize > 0 )
	{
		for ( auto itValue = InValue.begin(), itValueEnd = InValue.end(); itValue != itValueEnd; ++itValue )
		{
			InArchive << itValue->first;
			InArchive << itValue->second;
		}
	}

	return InArchive;
}

#endif // !Archive_H
