#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include "Logger/LoggerMacros.h"
#include "Reflection/ObjectGlobals.h"
#include "Reflection/ObjectHash.h"
#include "Reflection/ObjectGC.h"
#include "Reflection/Object.h"
#include "Reflection/ObjectPackage.h"

/**
 * @ingroup Core
 * @brief Special hash bucket
 * 
 * Contains a pointer to head element and an optional list of items if more than one element exists in the bucket.
 * The item list is only allocated if needed
 */
struct HashBucket
{
	friend struct HashBucketIterator;

	/**
	 * @brief Get set
	 * @return Return a pointer on std::unordered_set if this hash bacuket it have, otherwise retruns NULL
	 */
	FORCEINLINE std::unordered_set<CObject*>* GetSet()
	{
		if ( elementsOrSetPtr[1] && !elementsOrSetPtr[0] )
		{
			return ( std::unordered_set<CObject*>* )elementsOrSetPtr[1];
		}
		return nullptr;
	}

	/**
	 * @brief Get set
	 * @return Return a pointer on std::unordered_set if this hash bacuket it have, otherwise retruns NULL
	 */
	FORCEINLINE const std::unordered_set<CObject*>* GetSet() const
	{
		if ( elementsOrSetPtr[1] && !elementsOrSetPtr[0] )
		{
			return ( std::unordered_set<CObject*>* )elementsOrSetPtr[1];
		}
		return nullptr;
	}

	/**
	 * @brief Constructor
	 */
	FORCEINLINE HashBucket()
	{
		elementsOrSetPtr[0] = nullptr;
		elementsOrSetPtr[1] = nullptr;
	}

	/**
	 * @brief Destructor
	 */
	FORCEINLINE ~HashBucket()
	{
		delete GetSet();
	}

	/**
	 * @brief Add an object to the bucket
	 * @param InObject	Object
	 */
	FORCEINLINE void Add( CObject* InObject )
	{
		std::unordered_set<CObject*>*	items = GetSet();
		if ( items )
		{
			items->insert( InObject );
		}
		else if ( elementsOrSetPtr[0] && elementsOrSetPtr[1] )
		{
			items = new std::unordered_set<CObject*>();
			items->insert( ( CObject* )elementsOrSetPtr[0] );
			items->insert( ( CObject* )elementsOrSetPtr[1] );
			items->insert( InObject );
			elementsOrSetPtr[0] = nullptr;
			elementsOrSetPtr[1] = items;
		}
		else if ( elementsOrSetPtr[0] )
		{
			elementsOrSetPtr[1] = InObject;
		}
		else
		{
			elementsOrSetPtr[0] = InObject;
			Assert( !elementsOrSetPtr[1] );
		}
	}

	/**
	 * @brief Remove an object from the bucket
	 * 
	 * @param InObject	Object to remove from the bucket
	 * @return Return the number of elements removed
	 */
	FORCEINLINE uint32 Remove( CObject* InObject )
	{
		uint32							result = 0;
		std::unordered_set<CObject*>*	items = GetSet();
		if ( items )
		{
			uint32		oldSize = items->size();
			items->erase( InObject );
			result = oldSize - items->size();
			
			if ( items->size() <= 2 )
			{
				auto	it = items->begin();
				elementsOrSetPtr[0] = *it;
				++it;
				elementsOrSetPtr[1] = *it;
				delete items;
			}
		}
		else if ( elementsOrSetPtr[1] == InObject )
		{
			result = 1;
			elementsOrSetPtr[1] = nullptr;
		}
		else if ( elementsOrSetPtr[0] == InObject )
		{
			result = 1;
			elementsOrSetPtr[0] = elementsOrSetPtr[1];
			elementsOrSetPtr[1] = nullptr;
		}

		return result;
	}

	/**
	 * @brief Checks if an object exists in this bucket
	 * 
	 * @param InObject		Object to check
	 * @return Return TRUE if InObject is exists in the bucket, otherwise returns FALSE
	 */
	FORCEINLINE bool Contains( CObject* InObject ) const
	{
		const std::unordered_set<CObject*>*		items = GetSet();
		if ( items )
		{
			return items->find( InObject ) != items->end();
		}

		return elementsOrSetPtr[0] == InObject || elementsOrSetPtr[1] == InObject;
	}

	/**
	 * @brief Get the number of objects in this bucket
	 * @return Return the number of objects in the bucket
	 */
	FORCEINLINE uint32 Num() const
	{
		const std::unordered_set<CObject*>*		items = GetSet();
		if ( items )
		{
			return items->size();
		}
		return !!elementsOrSetPtr[0] + !!elementsOrSetPtr[1];
	}

	
	static std::unordered_set<CObject*> emptyBucket;			/**< This always empty set is used to get an iterator if the bucket doesn't use a std::unoredered_set (has only 1 element) */
	void*								elementsOrSetPtr[2];	/**< If these are both null, this bucket is empty. If the first one is null, but the second one is non-null, then the second one is a std::unordered_set pointer. If the first one is not null, then it is a cobject ptr, and the second ptr is either null or a second element */

private:
	/**
	 * @brief Get an begin interator for the std::unordered_set in this bucket or the EmptyBucket if items is NULL
	 * @return Return an begin interator for the std::unordered_set in this bucket or the EmptyBucket if items is NULL
	 */
	FORCEINLINE std::unordered_set<CObject*>::iterator GetBeginIteratorForSet()
	{
		std::unordered_set<CObject*>*	items = GetSet();
		return items ? items->begin() : emptyBucket.begin();
	}

	/**
	 * @brief Get a end interator for the std::unordered_set in this bucket or the EmptyBucket if items is NULL
	 * @return Return a end interator end for the std::unordered_set in this bucket or the EmptyBucket if items is NULL
	 */
	FORCEINLINE std::unordered_set<CObject*>::iterator GetEndIteratorForSet()
	{
		std::unordered_set<CObject*>* items = GetSet();
		return items ? items->end() : emptyBucket.end();
	}
};
std::unordered_set<CObject*>	HashBucket::emptyBucket;


/**
 * @ingroup Core
 * @brief Hash Bucket Iterator. Iterates over all objects in the bucket
 */
struct HashBucketIterator
{
	/**
	 * @brief Constructor
	 * @param InBucket	Bucket
	 */
	FORCEINLINE HashBucketIterator( HashBucket& InBucket )
		: bucket( InBucket )
		, setIterator( InBucket.GetBeginIteratorForSet() )
		, endIterator( InBucket.GetEndIteratorForSet() )
		, bItems( !!InBucket.GetSet() )
		, bReachedEndNoItems( !InBucket.elementsOrSetPtr[0] && !InBucket.elementsOrSetPtr[1] )
		, bSecondItem( false )
	{}

	/**
	 * @brief Advances the iterator to the next element
	 */
	FORCEINLINE HashBucketIterator& operator++()
	{
		if ( bItems )
		{
			++setIterator;
		}
		else
		{
			bReachedEndNoItems = bSecondItem || !bucket.elementsOrSetPtr[1];
			bSecondItem = true;
		}
		return *this;
	}

	/**
	 * @brief Conversion to "bool" returning true if the iterator is valid
	 */
	FORCEINLINE explicit operator bool() const
	{
		if ( bItems )
		{
			return setIterator != endIterator;
		}
		else
		{
			return !bReachedEndNoItems;
		}
	}

	/**
	 * @brief Inverse of the "bool" operator
	 */
	FORCEINLINE bool operator!() const
	{
		return !( bool )*this;
	}

	/**
	 * @brief Get object from iterator
	 */
	FORCEINLINE CObject* operator*()
	{
		if ( bItems )
		{
			return *setIterator;
		}
		else
		{
			return ( CObject* )bucket.elementsOrSetPtr[!!bSecondItem];
		}
	}

	HashBucket&								bucket;				/**< Bucket */
	std::unordered_set<CObject*>::iterator	setIterator;		/**< Current std::unordered_set iterator */
	std::unordered_set<CObject*>::iterator	endIterator;		/**< End iterator at std::unordered_set */
	bool									bItems;				/**< Is bucket has std::unordered_set */
	bool									bReachedEndNoItems;	/**< Is reached end when no std::unordered_set */
	bool									bSecondItem;		/**< Is current second item when no std::unordered_set */
};


/**
 * @ingroup Core
 * @brief Wrapper around a std::unordered_map with HashBucket values
 */
template<typename TType>
class TBucketMap
{
public:
	/**
	 * @brief Add a bucket into the map
	 * @param InKey		Key
	 */
	FORCEINLINE void Add( const TType& InKey )
	{
		map.insert( std::make_pair( InKey, HashBucket() ) );
	}

	/**
	 * @brief Remove a bucket from the map
	 * @param InKey		Key
	 */
	FORCEINLINE void Remove( const TType& InKey )
	{
		map.erase( InKey );
	}

	/**
	 * @brief Find a bucket in the map
	 * 
	 * @param InKey		Key
	 * @return Return a pointer to found bucket. If isn't exist returns NULL
	 */
	FORCEINLINE HashBucket* Find( const TType& InKey )
	{
		auto	it = map.find( InKey );
		return it != map.end() ? &it->second : nullptr;
	}

	/**
	 * @brief Find or add a bucket in the map
	 * 
	 * @param InKey		Key
	 * @return Return a reference on created/found bucket
	 */
	FORCEINLINE HashBucket& FindOrAdd( const TType& InKey )
	{
		// Find a bucket in the map
		auto	it = map.find( InKey );
		if ( it != map.end() )
		{
			return it->second;
		}

		// Otherwise we add a new bucket
		return map.insert( std::make_pair( InKey, HashBucket() ) ).first->second;
	}

private:
	std::unordered_map<TType, HashBucket>		map;		/**< Map */
};


/**
 * @ingroup Core
 * @brief Hash tables for CObject
 */
class CObjectHashTables
{
public:
	/**
	 * @brief Get singleton instance
	 * @return Return singleton instance
	 */
	static FORCEINLINE CObjectHashTables& Get()
	{
		static CObjectHashTables	hashTables;
		return hashTables;
	}

	/**
	 * @brief Checks if the Hash/Object pair exists in the CName hash table
	 * 
	 * @param InHash	CName hash
	 * @param InObject	Object
	 * @return Return TRUE if the Hash/Object pair exists, otherwise returns FALSE
	 */
	FORCEINLINE bool PairExistsInCNameHash( uint64 InHash, CObject* InObject )
	{
		bool			bResult = false;
		HashBucket*		bucket = hashCName.Find( InHash );
		if ( bucket )
		{
			bResult = bucket->Contains( InObject );
		}
		return bResult;
	}

	/**
	 * @brief Checks if the Hash/Object pair exists in the outer hash table
	 *
	 * @param InHash	Outer hash
	 * @param InObject	Object
	 * @return Return TRUE if the Hash/Object pair exists, otherwise returns FALSE
	 */
	FORCEINLINE bool PairExistsInOuterHash( uint64 InHash, CObject* InObject )
	{
		bool	bResult = false;
		auto	range = hashOuter.equal_range( InHash );
		for ( auto it = range.first; it != range.second; ++it )
		{
			if ( it->second == InObject->GetIndex() )
			{
				bResult = true;
				break;
			}
		}

		return bResult;
	}

	/**
	 * @brief Adds the Hash/Object pair to the CName hash table
	 * 
	 * @param InHash	CName hash
	 * @param InObject	Object
	 */
	FORCEINLINE void AddToCNameHash( uint64 InHash, CObject* InObject )
	{
		HashBucket&		bucket = hashCName.FindOrAdd( InHash );
		bucket.Add( InObject );
	}

	/**
	 * @brief Adds the Hash/Object pair to the outer hash table
	 *
	 * @param InHash	Outer hash
	 * @param InObject	Object
	 */
	FORCEINLINE void AddToOuterHash( uint64 InHash, CObject* InObject )
	{
		hashOuter.insert( std::make_pair( InHash, InObject->GetIndex() ) );
	}

	/**
	 * @brief Removes the Hash/Object pair from the CName hash table
	 * 
	 * @param InHash	CName hash
	 * @param InObject	Object
	 * @return Return the number of elements removed
	 */
	FORCEINLINE uint32 RemoveFromCNameHash( uint64 InHash, CObject* InObject )
	{
		uint32		numRemoved	= 0;
		HashBucket* bucket		= hashCName.Find( InHash );
		if ( bucket )
		{
			numRemoved = bucket->Remove( InObject );
			if ( bucket->Num() == 0 )
			{
				hashCName.Remove( InHash );
			}
		}
		return numRemoved;
	}

	/**
	 * @brief Removes the Hash/Object pair from the outer hash table
	 *
	 * @param InHash	Outer hash
	 * @param InObject	Object
	 * @return Return the number of elements removed
	 */
	FORCEINLINE uint32 RemoveFromOuterHash( uint64 InHash, CObject* InObject )
	{
		uint32		numRemoved = 0;
		auto		range = hashOuter.equal_range( InHash );
		for ( auto it = range.first; it != range.second; ++it )
		{
			if ( it->second == InObject->GetIndex() )
			{
				hashOuter.erase( it );
				++numRemoved;
				break;
			}
		}

		return numRemoved;
	}

	TBucketMap<uint64>							hashCName;	/**< CName hash table */
	std::unordered_multimap<uint64, uint32>		hashOuter;	/**< Outer hash table */
};


/**
 * @ingroup Core
 * @brief Splits an object path into CNames representing an outer chain
 * 
 * Input path examples: "Object", "Package.Object", "Object:Subobject", "Object:Subobject.Nested", "Package.Object:Subobject", "Package.Object:Subobject.NestedSubobject"
 */
struct ObjectSearchPath
{
	/**
	 * @brief Constructor
	 * @param InPath	Path
	 */
	explicit ObjectSearchPath( const CName& InPath )
	{
		std::wstring	path;
		std::wstring	delimiters = TEXT( "." ) SUBOBJECT_DELIMITER;
		InPath.ToString( path );

		std::size_t		idStart = 0;
		std::size_t		idCurrent = 0;
		while ( ( idCurrent = path.find_first_of( delimiters, idStart ) ) != std::wstring::npos )
		{
			outers.push_back( path.substr( idStart, idCurrent - idStart ) );
			idStart = idCurrent + 1;
		}

		if ( outers.empty() )
		{
			inner = path;
		}
		else
		{
			inner = CName( path.substr( idStart ), InPath.GetNumber() );
		}
	}

	/**
	 * @brief Is match outer names
	 * 
	 * @param InOuter	Outer object
	 * @return Return TRUE if matched outer names, otherwise returns FALSE
	 */
	FORCEINLINE bool MatchOuterNames( CObject* InOuter ) const
	{
		if ( outers.empty() )
		{
			return true;
		}

		for ( int32 index = outers.size() - 1; index > 0; --index )
		{
			if ( !InOuter || InOuter->GetCName() != outers[index] )
			{
				return false;
			}

			InOuter = InOuter->GetOuter();
		}

		return InOuter && InOuter->GetCName().EndsWith( outers[0] );
	}

	CName					inner;	/**< Inner name */
	std::vector<CName>		outers;	/**< Array of outer names */
};


/**
 * @ingroup Core
 * @brief Calculates the object's hash just using the object's name index
 * 
 * @param InObjectName	The object's name to use the index of
 * @return Return calculated hash for InObjectName
 */
static FORCEINLINE uint64 GetObjectHash( const CName& InObjectName )
{
	return InObjectName.GetHash();
}

/**
 * @ingroup Core
 * @brief Calculates the object's hash just using the object's name index XORed with the outer
 * 
 * Calculates the object's hash just using the object's name index
 * XORed with the outer. Yields much better spread in the hash
 * buckets, but requires knowledge of the outer, which isn't available
 * in all cases
 * 
 * @param ObjName	The object's name to use the index of
 * @param Outer		The object's outer pointer
 */
static FORCEINLINE uint64 GetObjectOuterHash( const CName& InObjectName, ptrint InOuter )
{
	return InObjectName.GetHash() + ( InOuter >> 6 );
}


/*
==================
FindObjectFast
==================
*/
CObject* FindObjectFast( CClass* InClass, CObject* InOuter, const CName& InName, bool InIsExactClass /* = false */, bool InIsAnyPackage /* = false */, ObjectFlags_t InExclusiveFlags /* = OBJECT_None */ )
{
	CObjectGC&		objectGC = CObjectGC::Get();
	if ( CObjectPackage::IsSavingPackage() || objectGC.IsGarbageCollecting() )
	{
		Sys_Errorf( TEXT( "Illegal call to FindObjectFast() while serializing object data or garbage collecting!" ) );
	}

	// If they specified an outer use that during the hashing
	CObjectHashTables&		hashTables = CObjectHashTables::Get();
	CObject*				result = nullptr;
	if ( InOuter )
	{
		uint64		hash = GetObjectOuterHash( InName, ( uint64 )InOuter );
		auto		range = hashTables.hashOuter.equal_range( hash );
		for ( auto it = range.first; it != range.second; ++it )
		{
			uint32		objectIndex = it->second;
			CObject*	object = objectGC.GetObject( objectIndex );
			if (
				// Check that the name matches the name we're searching for
				object->GetCName() == InName &&

				// Don't return objects that have any of the exclusive flags set
				!object->HasAnyObjectFlags( InExclusiveFlags ) &&

				// Check that the object has the correct Outer
				object->GetOuter() == InOuter &&

				// If a class was specified, check that the object is of the correct class
				( !InClass || ( InIsExactClass ? object->GetClass() == InClass : IsA( object, InClass ) ) ) )
			{
				AssertMsg( !object->IsUnreachable(), TEXT( "%s is unreachable" ), object->GetFullName().c_str() );
				if ( result )
				{
					Warnf( TEXT( "Ambiguous search, could be %s or %s\n" ), result->GetFullName().c_str(), object->GetFullName().c_str() );
				}
				else
				{
					result = object;
				}

#if SHIPPING_BUILD
				break;
#endif // SHIPPING_BUILD
			}
		}
	}
	else
	{
		ObjectSearchPath	searchPath( InName );
		uint64				hash = GetObjectHash( searchPath.inner );
		HashBucket*			bucket = hashTables.hashCName.Find( hash );
		if ( bucket )
		{
			for ( HashBucketIterator it( *bucket ); it; ++it )
			{
				CObject*	object = *it;
				if (
					// Check that the name matches the name we're searching for
					object->GetCName() == searchPath.inner &&

					// Don't return objects that have any of the exclusive flags set
					!object->HasAnyObjectFlags( InExclusiveFlags ) &&

					// If there is no package (no InOuter specified, and InName's package is "")
					// and the caller specified any package, then accept it, regardless of its package.
					// Or, if the object is a top-level package then accept it immediately
					( InIsAnyPackage || !object->GetOuter() ) &&

					// If a class was specified, check that the object is of the correct class
					( !InClass || ( InIsExactClass ? object->GetClass() == InClass : IsA( object, InClass ) ) ) &&

					// Ensure that the partial path provided matches the object found
					searchPath.MatchOuterNames( object->GetOuter() ) )
				{
					AssertMsg( !object->IsUnreachable(), TEXT( "%s is unreachable" ), object->GetFullName().c_str() );
					if ( result )
					{
						Warnf( TEXT( "Ambiguous search, could be %s or %s\n" ), result->GetFullName().c_str(), object->GetFullName().c_str() );
					}
					else
					{
						result = object;
					}

#if SHIPPING_BUILD
					break;
#endif // SHIPPING_BUILD
				}
			}
		}
	}

	return result;
}

/*
==================
HashObject
==================
*/
void HashObject( CObject* InObject )
{
	// We can hash an object only with name
	CName	name = InObject->GetCName();
	if ( name != NAME_None )
	{
		CObjectHashTables&		hashTables = CObjectHashTables::Get();
		uint64					hash = GetObjectHash( name );
		Assert( InObject );
		AssertMsg( !hashTables.PairExistsInCNameHash( hash, InObject ), TEXT( "%s already exists in CObject hash" ), InObject->GetFullName().c_str() );

		// Add hash into CName hash table
		hashTables.AddToCNameHash( hash, InObject );

		// If InObject has outer then we calculate outer hash and add it to the outer hash table
		ptrint	outer = ( ptrint )InObject->GetOuter();
		if ( outer )
		{
			hash = GetObjectOuterHash( name, outer );

			// If it already exists, something is wrong with the external code
			AssertMsg( !hashTables.PairExistsInOuterHash( hash, InObject ), TEXT( "%s already exists in CObject Outer hash" ), InObject->GetFullName().c_str() );
			hashTables.AddToOuterHash( hash, InObject );
		}
	}
}

/*
==================
UnhashObject
==================
*/
void UnhashObject( CObject* InObject )
{
	// We can unhash an object only with name
	CName	name = InObject->GetCName();
	if ( name != NAME_None )
	{
		CObjectHashTables&		hashTables = CObjectHashTables::Get();
		uint64					hash = GetObjectHash( name );
		uint32					numRemoved = hashTables.RemoveFromCNameHash( hash, InObject );

		// Must have existed, else something is wrong with the external code
		AssertMsg( numRemoved == 1, TEXT( "Internal error in remove from CName hash. NumRemoved = %d for %s" ), numRemoved, InObject->GetFullName().c_str() );

		// If InObject has outer then we calculate outer hash and remove it from the outer hash table
		ptrint		outer = ( ptrint )InObject->GetOuter();
		if ( outer )
		{
			hash		= GetObjectOuterHash( name, outer );
			numRemoved	= hashTables.RemoveFromOuterHash( hash, InObject );

			// Must have existed, else something is wrong with the external code
			AssertMsg( numRemoved == 1, TEXT( "Internal error in remove from outer hash. NumRemoved = %d for %s" ), numRemoved, InObject->GetFullName().c_str() );
		}
	}
}