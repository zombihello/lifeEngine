/**
 * @file
 * @addtogroup Core Core
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef OBJECTGC_H
#define OBJECTGC_H

#include <vector>
#include <list>

#include "Misc/Misc.h"
#include "Reflection/ObjectMacros.h"
#include "Core.h"

/**
 * @ingroup Core
 * @brief Context sensitive keep flags for garbage collection
 */
#define GARBAGE_COLLECTION_KEEPFLAGS		OBJECT_Native

/**
 * @ingroup Core
 * @brief Enum of different supported reference type tokens
 */
enum EGCReferenceType
{
	GCRT_None,			/**< None */
	GCRT_Object,		/**< CObject */
	GCRT_ArrayObject,	/**< Array of CObject */
	GCRT_ArrayStruct,	/**< Array of struct with CObject */
	GCRT_FixedArray,	/**< Fixed array */
	GCRT_EndOfStream	/**< End of stream */
};

/**
 * @ingroup Core
 * @brief Convenience struct containing all necessary information for a reference
 */
struct GCReferenceInfo
{
	/**
	 * @brief Constructor
	 * 
	 * @param InType	Type of reference
	 * @param InOffset	Offset into object/struct
	 */
	FORCEINLINE GCReferenceInfo( EGCReferenceType InType, uint32 InOffset )
		: returnCount( 0 )
		, type( InType )
		, offset( InOffset )
	{
		AssertMsg( InType != GCRT_None && InType <= GCRT_EndOfStream, TEXT( "Invalid GC Token Reference Type (%d)" ), ( uint32 )InType );
		AssertMsg( ( InOffset & ~0x7FFFF ) == 0, TEXT( "Invalid GC Token Offset (%d), max is %d" ), InOffset, 0x7FFFF );
	}

	/**
	 * @brief Constructor
	 * @param InValue	Value to set union mapping to a uint32 to
	 */
	FORCEINLINE GCReferenceInfo( uint32 InValue )
		: value( InValue )
	{}

	/**
	 * @brief uint32 conversion operator
	 * @return Return uint32 value of struct
	 */
	FORCEINLINE operator uint32() const
	{
		return value;
	}

	// Mapping to exactly one uint32
	union
	{
		// Mapping to exactly one uint32
		struct
		{		
			uint32	returnCount : 8;	/**< Return depth, e.g. 1 for last entry in an array, 2 for last entry in an array of structs of arrays, ... */		
			uint32	type		: 5;	/**< Type of reference */		
			uint32	offset		: 19;	/**< Offset into struct/object */
		};

		uint32		value;				/**< uint32 value of reference info, used for easy conversion to/from uint32 for token array */
	};

	static const GCReferenceInfo	endOfStreamToken;	/**< End of token stream token */
};

/**
 * @ingroup Core
 * @brief Convenience structure containing all necessary information for skipping a dynamic array
 */
struct GCSkipInfo
{
	/**
	 * @brief Constructor
	 */
	FORCEINLINE GCSkipInfo()
	{}

	/**
	 * @brief Constructor
	 * @param InValue	Value to set union mapping to a uint32 to
	 */
	FORCEINLINE GCSkipInfo( uint32 InValue )
		: value( InValue )
	{}

	/**
	 * @brief uint32 conversion operator
	 * @return Return uint32 value of struct
	 */
	FORCEINLINE operator uint32() const
	{
		return value;
	}

	// Mapping to exactly one uint32
	union
	{
		// Mapping to exactly one uint32
		struct
		{
			uint32	innerReturnCount	: 8;	/**< Return depth not taking into account embedded arrays. This is needed to return appropriately when skipping empty dynamic arrays as we never step into them */		
			uint32	skipIndex			: 24;	/**< Skip index */
		};
		
		uint32		value;						/**< uint32 value of skip info, used for easy conversion to/ from uint32 for token array */
	};
};

/**
 * @ingroup Core
 * @brief Reference token stream class 
 * Used for creating and parsing stream of object references
 */
class CGCReferenceTokenStream
{
public:
	static_assert( sizeof( GCReferenceInfo ) == sizeof( uint32 ) );

	/**
	 * @brief Initialization value to ensure that we have the right skip index index
	 */
	enum EGCArrayInfoPlaceholder 
	{ 
		GCSkipIndexPlaceholder = 0xDEDEDEDE 
	};

	/**
	 * @brief Shrinks the token stream, removing array slack
	 */
	FORCEINLINE void Shrink()
	{
		tokens.shrink_to_fit();
	}

	/**
	 * @brief Prepends passed in stream to existing one
	 * @param InOther	Stream to concatenate
	 */
	void PrependStream( const CGCReferenceTokenStream& InOther );

	/**
	 * @brief Emit reference info
	 * @param InReferenceInfo	Reference info to emit
	 */
	FORCEINLINE void EmitReferenceInfo( GCReferenceInfo InReferenceInfo )
	{
		tokens.push_back( InReferenceInfo );
	}

	/**
	 * @brief Emit placeholder for array skip index, updated in UpdateSkipIndexPlaceholder
	 * @return Return the index of the skip index, used later in UpdateSkipIndexPlaceholder
	 */
	FORCEINLINE uint32 EmitSkipIndexPlaceholder()
	{
		tokens.push_back( GCSkipIndexPlaceholder );
		return tokens.size() - 1;
	}

	/**
	 * @brief Updates skip index place holder stored and passed in skip index index with passed
	 * in skip index. 
	 * The skip index is used to skip over tokens in the case of an empty
	 * dynamic array
	 *
	 * @param InSkipIndexIndex	Index where skip index is stored at.
	 * @param InSkipIndex		Index to store at skip index index
	 */
	void UpdateSkipIndexPlaceholder( uint32 InSkipIndexIndex, uint32 InSkipIndex );

	/**
	 * @brief Emit count
	 * @param InCount	Count to emit
	 */
	FORCEINLINE void EmitCount( uint32 InCount )
	{
		tokens.push_back( InCount );
	}

	/**
	 * @brief Emit stride
	 * @param InStride	Stride to emit
	 */
	FORCEINLINE void EmitStride( uint32 InStride )
	{
		tokens.push_back( InStride );
	}

	/**
	 * @brief Increase return count on last token.
	 * @return Return index of next token
	 */
	FORCEINLINE uint32 EmitReturn()
	{
		GCReferenceInfo		referenceInfo = tokens.back();
		Assert( referenceInfo.type != GCRT_None );
		++referenceInfo.returnCount;
		tokens[tokens.size() - 1] = referenceInfo;
		return tokens.size();
	}

	/**
	 * @brief Reads count and advances stream
	 *
	 * @param InOutCurrentIndex		Current index. After calling, increase by one
	 * @return Return read in count
	 */
	FORCEINLINE uint32 ReadCount( uint32& InOutCurrentIndex ) const
	{
		return tokens[InOutCurrentIndex++];
	}

	/**
	 * @brief Reads stride and advances stream
	 *
	 * @param InOutCurrentIndex		Current index. After calling, increase by one
	 * @return Return read in stride
	 */
	FORCEINLINE uint32 ReadStride( uint32& InOutCurrentIndex ) const
	{
		return tokens[InOutCurrentIndex++];
	}

	/**
	 * @brief Reads in reference info and advances stream
	 *
	 * @param InOutCurrentIndex		Current index. After calling, increase by one
	 * @return Return read in reference info
	 */
	FORCEINLINE GCReferenceInfo ReadReferenceInfo( uint32& InOutCurrentIndex ) const
	{
		return tokens[InOutCurrentIndex++];
	}

	/**
	 * @brief Access reference info at passed in index. Used as helper to eliminate LHS
	 *
	 * @param InCurrentIndex	Current index
	 * @return Return reference info at passed in index
	 */
	FORCEINLINE GCReferenceInfo AccessReferenceInfo( uint32 InCurrentIndex ) const
	{
		return tokens[InCurrentIndex];
	}

	/**
	 * @brief Read in skip index and advances stream
	 *
	 * @param InOutCurrentIndex		Current index. After calling, increase by one
	 * @return Return read in skip index
	 */
	FORCEINLINE GCSkipInfo ReadSkipInfo( uint32& InOutCurrentIndex ) const
	{
		GCSkipInfo			skipInfo = tokens[InOutCurrentIndex];
		skipInfo.skipIndex += InOutCurrentIndex;
		InOutCurrentIndex++;
		return skipInfo;
	}

	/**
	 * @brief Read return count stored at the index before the skip index 
	 * This is required to correctly return the right amount of levels when skipping over an empty array
	 *
	 * @param InSkipInfo	Index of first token after array
	 * @return Return skip return count
	 */
	FORCEINLINE uint32 GetSkipReturnCount( GCSkipInfo InSkipInfo ) const
	{
		Assert( InSkipInfo.skipIndex > 0 && InSkipInfo.skipIndex <= tokens.size() );
		GCReferenceInfo			referenceInfo = tokens[InSkipInfo.skipIndex - 1];
		Assert( referenceInfo.type != GCRT_None );
		return referenceInfo.returnCount - InSkipInfo.innerReturnCount;
	}

	/**
	 * @brief Queries the stream for an end of stream condition
	 *
	 * @param InCurrentIndex	Current index
	 * @return Return TRUE if the end of the stream has been reached, FALSE otherwise
	 */
	FORCEINLINE bool IsEndOfStream( uint32 InCurrentIndex ) const
	{
		return InCurrentIndex >= tokens.size();
	}

	/**
	 * @brief Get token count
	 * @return Return token count
	 */
	FORCEINLINE uint32 GetTokenCount() const
	{ 
		return tokens.size();
	}

private:
	std::vector<uint32>		tokens;		/**< Token array */
};

/**
 * @ingroup Core
 * @brief Scope helper structure to emit tokens for fixed arrays in the case of array size (passed in count) being > 1
 */
struct GCReferenceFixedArrayTokenHelper
{
	/**
	 * @brief Constructor 
	 * Emitting necessary tokens for fixed arrays if count > 1 and also keeping track of count so
	 * destructor can do the same
	 *
	 * @param InReferenceTokenStream	Token stream to emit tokens to
	 * @param InOffset					Offset into object/struct
	 * @param InCount					Array count
	 * @param InStride					Array type stride (e.g. sizeof(struct) or sizeof(CObject*))
	 */
	GCReferenceFixedArrayTokenHelper( CGCReferenceTokenStream* InReferenceTokenStream, uint32 InOffset, uint32 InCount, uint32 InStride )
		: referenceTokenStream( InReferenceTokenStream )
		, count( InCount )
	{
		if ( InCount > 1 )
		{
			referenceTokenStream->EmitReferenceInfo( GCReferenceInfo( GCRT_FixedArray, InOffset ) );
			referenceTokenStream->EmitStride( InStride );
			referenceTokenStream->EmitCount( InCount );
		}
	}

	/**
	 * @brief Destructor
	 * Emitting return if array size > 1
	 */
	~GCReferenceFixedArrayTokenHelper()
	{
		if ( count > 1 )
		{
			referenceTokenStream->EmitReturn();
		}
	}

private:
	CGCReferenceTokenStream*	referenceTokenStream;	/**< Reference token stream used to emit to */
	uint32						count;					/**< Size of fixed array */
};

/**
 * @ingroup Core
 * @brief Garbage collector of all CObject
 */
class CObjectGC
{
public:
	friend class CObject;
	friend class CObjectIterator;

	/**
	 * @brief Constructor
	 */
	CObjectGC();

	/**
	 * @brief Get singleton instance
	 * @return Return singleton instance
	 */
	static FORCEINLINE CObjectGC& Get()
	{
		static CObjectGC	objectGC;
		return objectGC;
	}

	/**
	 * @brief Add the object into GC for it to keep an eye on the one
	 * After added object into GC, in case when InObject is unreachable garbage collector will delete it
	 * 
	 * @param InObject	Object
	 */
	void AddObject( class CObject* InObject );

	/**
	 * @brief Remove object from GC
	 * @param InObject	Object
	 */
	void RemoveObject( class CObject* InObject );

	/**
	 * @brief Deletes all unreferenced objects, keeping objects that have any of the passed in InKeepFlags set
	 * 
	 * @param	InKeepFlags				Objects with those flags will be kept regardless of being referenced or not
	 * @param	InIsPerformFullPurge	If TRUE, perform a full purge after the mark pass
	 */
	void CollectGarbage( ObjectFlags_t InKeepFlags, bool InIsPerformFullPurge = true );

	/**
	 * @brief Incrementally purge garbage by deleting all unreferenced objects after routing destroy
	 *
	 * @param InIsUseTimeLimit	Whether the time limit parameter should be used
	 * @param InTimeLimit		Soft time limit for this function call
	 */
	void IncrementalPurgeGarbage( bool InIsUseTimeLimit, float InTimeLimit = 0.005f );

	/**
	 * @brief Is valid object index
	 * 
	 * @param InIndex	Object index
	 * @return Return TRUE if InIndex is valid, otherwise returns FALSE
	 */
	FORCEINLINE bool IsValidIndex( uint32 InIndex ) const
	{
		return InIndex >= 0 && InIndex < allocatedObjects.size();
	}

	/**
	 * @brief Get object by index
	 * 
	 * @param InIndex	Object index
	 * @param Return object by index
	 */
	FORCEINLINE class CObject* GetObject( uint32 InIndex ) const
	{
		return allocatedObjects[InIndex];
	}

	/**
	 * @brief Get number of allocated objects
	 * @return Return number of allocated objects
	 */
	FORCEINLINE uint32 GetNumAllocatedObjects() const
	{
		uint32	numNonGCObjects = lastNonGCIndex != INDEX_NONE ? lastNonGCIndex : 0;
		uint32	numGCObjects	= allocatedObjects.size() - firstGCIndex;
		return numNonGCObjects + numGCObjects;
	}

	/**
	 * brief Get reserved size of allocated objects array
	 * @return Return reserved size of allocated objects array
	 */
	FORCEINLINE uint32 GetReservedSizeAllocatedObjects() const
	{
		return allocatedObjects.size();
	}

private:
	/**
	 * @brief Helper struct for stack based approach
	 */
	struct StackEntry
	{	
		byte*		data;			/**< Current data pointer, incremented by stride */	
		int32		stride;			/**< Current stride */	
		int32		count;			/**< Current loop count, decremented each iteration */
		int32		loopStartIndex; /**< First token index in loop */
	};

	/**
	* @brief Performs reachability analysis
	* @param InKeepFlags	Objects with these flags will be kept regardless of being referenced or not
	*/
	void PerformReachabilityAnalysis( ObjectFlags_t InKeepFlags );

	/**
	 * @brief Mark objects as unreachable
	 * 
	 * @param InOutReachableObjects		Output array with reachable objects
	 * @param InKeepFlags				Objects with these flags will be kept regardless of being referenced or not
	 */
	void MarkObjectsUnreachable( std::vector<class CObject*>& InOutReachableObjects, ObjectFlags_t InKeepFlags );

	/**
	 * @brief Traverse object references
	 * @param InOutReachableObjects		Output array with reachable objects
	 */
	void TraverseObjectReferences( std::vector<class CObject*>& InOutReachableObjects );

	/**
	 * @brief Process object for references
	 * 
	 * @param InCurrentObject			Current object
	 * @param InOutReachableObjects		Output array with reachable objects
	 * @param InOutStack				Stack
	 */
	void ProcessObjectForReferences( class CObject* InCurrentObject, std::vector<class CObject*>& InOutNewReachableObjects, std::vector<StackEntry>& InOutStack );

	/**
	 * @brief Is time limit exceeded
	 * 
	 * @param InStartTime	Start time
	 * @param InTimeLimit	Time limit
	 * @return Return TRUE if time limit is exceeded, otherwsire returns FALSE
	 */
	FORCEINLINE bool IsTimeLimitExceeded( double InStartTime, double InTimeLimit ) const
	{
		// Only check time limit every so often to avoid calling Sys_Seconds too often
		return Sys_Seconds() - InStartTime > InTimeLimit;
	}

	bool						bPurgeIsRequired;								/**< Whether we need to purge objects or not */
	bool						bDelayedBeginDestroyHasBeenRoutedToAllObjects;	/**< Whether delayed BeginDestroy has already been routed to all unreachable objects */
	bool						bFinishDestroyHasBeenRoutedToAllObjects;		/**< Whether FinishDestroy has already been routed to all unreachable objects */
	uint32						currentPurgeObjectIndex;						/**< Current object index for incremental purge */
	uint32						objectsPendingDestructionCount;					/**< Number of objects actually still pending destruction */
	uint32						firstGCIndex;									/**< First index into objects array taken into account for GC */
	uint32						lastNonGCIndex;									/**< Index pointing to last object created in range disregarded for GC */
	std::vector<class CObject*>	allocatedObjects;								/**< List of all allocated objects */
	std::vector<uint32>			objectsPendingDestruction;						/**< Array that we'll fill with indices to objects that are still pending destruction after the first GC sweep */
	std::list<uint32>			availableObjectIndeces;							/**< Available object indices */
	std::vector<uint32>			unreachableObjectsIndices;						/**< Index of objects with the OBJECT_Unreachable flag during garbage collection */
};

#endif // !OBJECTGC_H