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
#include "Misc/Compression.h"
#include "System/BaseTargetPlatform.h"

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
	 * @brief Tells the archive to attempt to preload the specified object so data can be loaded out of it
	 * @param InObject	The object to load data for
	 */
	virtual void Preload( class CObject* InObject ) {}

	/**
	 * @brief Precache the region that to be read soon
	 * This function will not change the current archive position
	 * 
	 * @param InPrecacheOffset		Offset at which to begin precaching
	 * @param InPrecacheSize		Number of bytes to precache
	 */
	virtual void Precache( uint32 InPrecacheOffset, uint32 InPrecacheSize ) {}

	/**
	 * @brief Flushes cache and frees internal data
	 */
	virtual void FlushCache() {}

	/**
	 * @brief Set mapping from offsets/size that are going to be used for seeking and serialization to what
	 * is actually stored on disk
	 * 
	 * @param InCompressedChunks	Pointer to array containing information about (un)compressed chunks
	 * @param InCompressionFlags	Flags determining compression format associated with mapping
	 * @return Return TRUE if archive supports translating offsets and uncompressing on read, otherwise FALSE 
	 */
	virtual bool SetCompressionMap( std::vector<struct CompressedChunk>* InCompressedChunks, ECompressionFlags InCompressionFlags ) { return false; }

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
	 * @brief Indicates whether this archive is filtering editor-only on save or contains data that had editor-only content stripped
	 * @return Return TRUE if the archive filters editor-only content, otherwise FALSE 
	 */
	FORCEINLINE bool IsFilterEditorOnly() const
	{
		return arIsFilterEditorOnly;
	}

	/**
	 * @brief Sets a flag indicating that this archive needs to filter editor-only content
	 * @param InFilterEditorOnly	Whether to filter editor-only content
	 */
	virtual void SetFilterEditorOnly( bool InFilterEditorOnly )
	{
		arIsFilterEditorOnly = InFilterEditorOnly;
	}

	/**
	 * @brief Indicates whether this archive is saving or loading game state
	 * @note This is intended for game-specific archives and is not true for any of the build in save methods
	 * 
	 * @return Return TRUE if the archive is dealing with save games, otherwise FALSE
	 */
	FORCEINLINE bool IsSaveGame() const
	{
		return arIsSaveGame;
	}

	/**
	 * @brief Sets a flag indicating that this archive is saving or loading game state
	 * @param InIsSaveGame		Whether this archive is saving/loading game state
	 */
	FORCEINLINE void SetSaveGame( bool InIsSaveGame )
	{
		arIsSaveGame = InIsSaveGame;
	}

	/**
	 * @brief Checks whether the archive wants to skip the property independent of the other flags
	 * @return Return TRUE if this property must be skiped, otherwise returns FALSE
	 */
	virtual bool ShouldSkipProperty( const class CProperty* InProperty ) const
	{
		return false;
	}

	/**
	 * @brief Checks is this archive wants properties to be serialized in binary form instead of safer but slower tagged form
	 * @return Return TRUE if this archive wants properties to be serialized in binary form instead of safer but slower tagged form, otherwise returns FALSE
	 */
	FORCEINLINE bool WantBinaryPropertySerialization() const
	{
		return arWantBinaryPropertySerialization;
	}

	/**
	 * @brief Set is this archive wants properties to be serialized in binary form instead of safer but slower tagged form
	 * @param InWantBinaryPropertySerialization		TRUE is means what this archive want properties to be serialized in binary form instead of safer but slower tagged form
	 */
	virtual void SetWantBinaryPropertySerialization( bool InWantBinaryPropertySerialization )
	{
		arWantBinaryPropertySerialization = InWantBinaryPropertySerialization;
	}

	/**
	 * @brief Is the archive used for cooking
	 * @return Return TRUE if the archive is used from cooking, otherwise FALSE. In build without editor always returns FASLE
	 */
	FORCEINLINE bool IsCooking() const
	{
#if WITH_EDITOR
		Assert( !cookingTargetPlatform || ( IsSaving() && !IsLoading() ) );
		return cookingTargetPlatform;
#else
		return false;
#endif // WITH_EDITOR
	}

	/**
	 * @brief Get the cooking target platform
	 * @note In build without editor always returns NULL
	 * 
	 * @return Return the cooking target platform. If not set returns NULL
	 */
	FORCEINLINE CBaseTargetPlatform* GetCookingTarget() const
	{
#if WITH_EDITOR
		return cookingTargetPlatform;
#else
		return nullptr;
#endif // WITH_EDITOR
	}

	/**
	 * @brief Set the cooking target platform
	 * @note In build without editor do nothing
	 * 
	 * @param InCookingTarget	The target platform to set
	 */
	virtual void SetCookingTarget( CBaseTargetPlatform* InCookingTarget )
	{
#if WITH_EDITOR
		cookingTargetPlatform = InCookingTarget;
#endif // WITH_EDITOR
	}

	/**
	 * @brief Is this archive only looking for CObject references
	 * @return Return TRUE if this archive is only looking for CObject references, otherwise returns FALSE
	 */
	FORCEINLINE bool IsObjectReferenceCollector() const
	{
		return arIsObjectReferenceCollector;
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

	/**
	 * @brief Override operator << for serialize CNames
	 * @return Return reference to self
	 */
	virtual CArchive& operator<<( class CName& InValue );

	/**
	 * @brief Override operator << for serialize CNames
	 * @return Return reference to self
	 */
	virtual CArchive& operator<<( const class CName& InValue );

protected:
	uint32					arVer;								/**< Archive version (look ELifeEnginePackageVersion) */
	EArchiveType			arType;								/**< Archive type */
	std::wstring			arPath;								/**< Path to archive */
	bool					arIsObjectReferenceCollector;		/**< Whether this archive only cares about serializing object references */
	bool					arIsFilterEditorOnly;				/**< Whether editor only properties are being filtered from the archive (or has been filtered) */
	bool					arIsSaveGame;						/**< Whether this archive is saving/loading game state */
	bool					arWantBinaryPropertySerialization;	/**< Whether this archive wants properties to be serialized in binary form instead of tagged */

#if WITH_EDITOR
	CBaseTargetPlatform*	cookingTargetPlatform;				/**< Holds the cooking target platform */
#endif // WITH_EDITOR
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
	{
		arIsObjectReferenceCollector = true;
	}

	/**
	 * @brief Is saving archive
	 * @return True if archive saving, false if archive loading
	 */
	virtual bool IsSaving() const 
	{ 
		return true; 
	}

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
