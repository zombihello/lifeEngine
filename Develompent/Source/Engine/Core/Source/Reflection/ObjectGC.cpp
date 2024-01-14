#include "Logger/LoggerMacros.h"
#include "System/Config.h"
#include "Reflection/Object.h"
#include "Reflection/ObjectGC.h"
#include "Reflection/Class.h"
#include "Reflection/LinkerManager.h"

// Maximum count of objects not considered for GC
#define MAX_OBJECTS_NOT_CONSIDERED_FOR_GC			30000

// Maximum number of objects on level
#define MAX_NUMBER_OF_LEVEL_OBJECTS					400000

// Maximum all allocated objects in the system
#define MAX_OBJECTS									( MAX_OBJECTS_NOT_CONSIDERED_FOR_GC + MAX_NUMBER_OF_LEVEL_OBJECTS )

// End of token stream token
const GCReferenceInfo		GCReferenceInfo::endOfStreamToken( GCRT_EndOfStream, 0 );

/*
==================
CGCReferenceTokenStream::PrependStream
==================
*/
void CGCReferenceTokenStream::PrependStream( const CGCReferenceTokenStream& InOther )
{
	// Remove embedded EOS token if needed
	uint32					numTokensToPrepend = ( !InOther.tokens.empty() && InOther.tokens.back() == GCReferenceInfo::endOfStreamToken ) ? InOther.tokens.size() - 1 : InOther.tokens.size();
	std::vector<uint32>		newTokens;
	newTokens.reserve( numTokensToPrepend + tokens.size() );

	// Add tokens of InOther to the new array
	for ( uint32 index = 0; index < numTokensToPrepend; ++index )
	{
		newTokens.push_back( InOther.tokens[index] );
	}

	// Add own tokens to the new array
	for ( uint32 index = 0, count = tokens.size(); index < count; ++index )
	{
		newTokens.push_back( tokens[index] );
	}

	// Replace the new array and the old
	tokens = std::move( newTokens );
}

/*
==================
CGCReferenceTokenStream::UpdateSkipIndexPlaceholder
==================
*/
void CGCReferenceTokenStream::UpdateSkipIndexPlaceholder( uint32 InSkipIndexIndex, uint32 InSkipIndex )
{
	Assert( InSkipIndex > 0 && InSkipIndex <= tokens.size() );
	const GCReferenceInfo&		referenceInfo = tokens[InSkipIndex - 1];
	Assert( referenceInfo.type != GCRT_None );
	Assert( tokens[InSkipIndexIndex] == GCSkipIndexPlaceholder );
	Assert( InSkipIndexIndex < InSkipIndex );
	Assert( referenceInfo.returnCount >= 1 );

	GCSkipInfo					skipInfo;
	skipInfo.skipIndex			= InSkipIndex - InSkipIndexIndex;	
	skipInfo.innerReturnCount	= referenceInfo.returnCount - 1;	// We need to subtract 1 as 'returnCount' includes return from this array
	tokens[InSkipIndexIndex]	= skipInfo;
}

/*
==================
CObjectGC::CObjectGC
==================
*/
CObjectGC::CObjectGC()
	: bIsGarbageCollecting( false )
	, bPurgeIsRequired( false )
	, bDelayedBeginDestroyHasBeenRoutedToAllObjects( false )
	, bFinishDestroyHasBeenRoutedToAllObjects( false )
	, currentPurgeObjectIndex( 0 )
	, objectsPendingDestructionCount( 0 )
	, firstGCIndex( MAX_OBJECTS_NOT_CONSIDERED_FOR_GC )
	, lastNonGCIndex( INDEX_NONE )
{
	// Reserve space in the allocated objects array for objects which disregard for GC
	allocatedObjects.reserve( firstGCIndex + MAX_NUMBER_OF_LEVEL_OBJECTS );
	allocatedObjects.resize( MAX_OBJECTS_NOT_CONSIDERED_FOR_GC );
}

/*
==================
CObjectGC::AddObject
==================
*/
void CObjectGC::AddObject( CObject* InObject )
{
	// If the object already was added to the GC its error
	Assert( InObject->index == INDEX_NONE );
	uint32	index = INDEX_NONE;

	// Special non-garbage collectable range
	if ( InObject->HasAnyObjectFlags( OBJECT_DisregardForGC ) && ( !availableNonGCObjectIndeces.empty() || ( lastNonGCIndex+1 < firstGCIndex ) ) )
	{
		// If we have free available indeces then use it
		if ( !availableNonGCObjectIndeces.empty() )
		{
			index = availableNonGCObjectIndeces.front();
			availableNonGCObjectIndeces.pop_front();
			Assert( !allocatedObjects[index] );
		}
		else
		{
			index = ++lastNonGCIndex;
		}
	}
	// Regular pool/range
	else
	{
		// If we have free available indeces then use it
		if ( !availableGCObjectIndeces.empty() )
		{
			index = availableGCObjectIndeces.front();
			availableGCObjectIndeces.pop_front();
			Assert( !allocatedObjects[index] );
		}
		else
		{
			index = allocatedObjects.size();
			allocatedObjects.push_back( nullptr );
		}
	}

	// Clear object flag signaling disregard for GC if object was allocated in garbage collectible range
	if ( index >= firstGCIndex )
	{
		// Object is allocated in regular pool so we need to clear OBJECT_DisregardForGC if it was set
		InObject->RemoveObjectFlag( OBJECT_DisregardForGC );
	}

	// Make sure only objects in disregarded index range have the object flag set
	Assert( !InObject->HasAnyObjectFlags( OBJECT_DisregardForGC ) || ( index < firstGCIndex ) );

	// Make sure that objects disregarded for GC are part of root set
	Assert( !InObject->HasAnyObjectFlags( OBJECT_DisregardForGC ) || InObject->HasAnyObjectFlags( OBJECT_RootSet ) );

	// Add the object to the global table
	allocatedObjects[index] = InObject;
	InObject->index = index;
}

/*
==================
CObjectGC::RemoveObject
==================
*/
void CObjectGC::RemoveObject( CObject* InObject )
{
	// If the object isn't in the GC its error
	Assert( InObject->index != INDEX_NONE );
	
	// Special non-garbage collectable range
	if ( InObject->IsDisregardedForGC() )
	{
		availableNonGCObjectIndeces.push_back( InObject->index );
	}
	// Regular pool/range
	else
	{		
		availableGCObjectIndeces.push_back( InObject->index );		
	}

	allocatedObjects[InObject->index] = nullptr;
	InObject->index = INDEX_NONE;
}

/*
==================
CObjectGC::CollectGarbage
==================
*/
void CObjectGC::CollectGarbage( ObjectFlags_t InKeepFlags, bool InIsPerformFullPurge /* = true */ )
{
	bIsGarbageCollecting = true;
	Logf( TEXT( "Collecting garbage\n" ) );

	// Make sure previous incremental purge has finished
	if ( bPurgeIsRequired )
	{
		IncrementalPurgeGarbage( false );
	}

	// Do perform reachability analysis
	{
		const double	startTime = Sys_Seconds();
		PerformReachabilityAnalysis( InKeepFlags );
		Logf( TEXT( "%f ms for realtime GC\n" ), ( Sys_Seconds() - startTime ) * 1000.f );
	}

	// Call BeginDestroy in all objects with OBJECT_Unreachable flag
	{
		const double	startTime = Sys_Seconds();
		uint32		numObjects		= allocatedObjects.size();
		CObject**	currentObject	= allocatedObjects.data() + firstGCIndex;
		CObject**	lastObject		= allocatedObjects.data() + numObjects;
		uint32		objectIndex		= firstGCIndex;
		for ( ; currentObject < lastObject; ++currentObject, ++objectIndex )
		{
			CObject*	object = *currentObject;
			if ( object && object->HasAnyObjectFlags( OBJECT_Unreachable ) )
			{
				// Begin the object's asynchronous destruction
				object->ConditionalBeginDestroy();
				unreachableObjectsIndices.push_back( objectIndex );
			}
		}

		Logf( TEXT( "%f ms for unhashing unreachable objects\n" ), ( Sys_Seconds() - startTime ) * 1000.f );
	}

	// Set flag to indicate that we are relying on a purge to be performed
	bPurgeIsRequired = true;

	// Perform a full purge by not using a time limit for the incremental purge. The Editor always does a full purge
	if ( InIsPerformFullPurge || g_IsEditor )
	{
		IncrementalPurgeGarbage( false );
	}

	// Destroy all pending delete object loaders
	CLinkerManager::Get().DeleteLoaders();

	// We're done collecting garbage. Note that IncrementalPurgeGarbage above might already clear it internally
	bIsGarbageCollecting = false;
}

/*
==================
CObjectGC::IncrementalPurgeGarbage
==================
*/
void CObjectGC::IncrementalPurgeGarbage( bool InIsUseTimeLimit, float InTimeLimit /* = 0.005f */ )
{
	// Early out if there is nothing to do
	if ( !bPurgeIsRequired )
	{
		return;
	}

	// Keep track of start time to enforce time limit unless InIsUseTimeLimit is TRUE
	const double		startTime = Sys_Seconds();
	bool				bTimeLimitReached = false;

	if ( !bDelayedBeginDestroyHasBeenRoutedToAllObjects && !bTimeLimitReached )
	{
		bDelayedBeginDestroyHasBeenRoutedToAllObjects = true;
		currentPurgeObjectIndex = 0;
	}

	if ( bDelayedBeginDestroyHasBeenRoutedToAllObjects && !bFinishDestroyHasBeenRoutedToAllObjects && !bTimeLimitReached )
	{
		// Try to dispatch all FinishDestroy messages to unreachable objects.  We'll iterate over every
		// single object and destroy any that are ready to be destroyed.  The objects that aren't yet
		// ready will be added to a list to be processed afterwards
		while ( currentPurgeObjectIndex < unreachableObjectsIndices.size() && !bTimeLimitReached )
		{
			// Object should always be unreachable, have had BeginDestroy called on it and never already be destroyed
			CObject*	object = allocatedObjects[unreachableObjectsIndices[currentPurgeObjectIndex] ];
			Assert( object && object->HasAnyObjectFlags( OBJECT_Unreachable ) && object->HasAnyObjectFlags( OBJECT_BeginDestroyed ) && !object->HasAnyObjectFlags( OBJECT_FinishDestroyed ) );

			// Only proceed with destroying the object if the asynchronous cleanup started by BeginDestroy has finished
			if ( object->IsReadyForFinishDestroy() )
			{
				object->ConditionalFinishDestroy();
			}
			else
			{
				// The object isn't ready for FinishDestroy to be called yet.  This is common in the
				// case of a graphics resource that is waiting for the render thread "release fence"
				// to complete.  Just calling IsReadyForFinishDestroy may begin the process of releasing
				// a resource, so we don't want to block iteration while waiting on the render thread.

				// Add the object index to our list of objects to revisit after we process everything else
				objectsPendingDestruction.push_back( currentPurgeObjectIndex );
				++objectsPendingDestructionCount;
			}

			// We've processed the object so increment our global iterator.  It's important to do this before
			// we test for the time limit so that we don't process the same object again next tick!
			currentPurgeObjectIndex++;

			// Check time limit
			if ( InIsUseTimeLimit )
			{
				bTimeLimitReached = IsTimeLimitExceeded( startTime, InTimeLimit );
				if ( bTimeLimitReached )
				{
					break;
				}
			}
		}

		// Have we finished the first round of attempting to call FinishDestroy on unreachable objects?
		if ( currentPurgeObjectIndex == unreachableObjectsIndices.size() )
		{
			// We've finished iterating over all unreachable objects, but we need still need to handle
			// objects that were deferred
			while ( objectsPendingDestructionCount > 0 )
			{
				uint32				currentPendingObjIndex = 0;
				while ( currentPendingObjIndex < objectsPendingDestructionCount )
				{
					// Grab the actual object for the current pending object list iteration
					const uint32	currentObjectIndex = objectsPendingDestruction[currentPendingObjIndex];
					Assert( IsValidIndex( unreachableObjectsIndices[currentObjectIndex] ) );
					CObject*		object = allocatedObjects[ unreachableObjectsIndices[currentObjectIndex] ];

					// Object should never have been added to the list if it failed this criteria
					Assert( object && object->HasAnyObjectFlags( OBJECT_Unreachable ) );

					// Object should always have had BeginDestroy called on it and never already be destroyed
					Assert( object->HasAnyObjectFlags( OBJECT_BeginDestroyed ) && !object->HasAnyObjectFlags( OBJECT_FinishDestroyed ) );

					// Only proceed with destroying the object if the asynchronous cleanup started by BeginDestroy has finished
					if ( object->IsReadyForFinishDestroy() )
					{
						// Send FinishDestroy message
						object->ConditionalFinishDestroy();

						// Remove the object index from our list quickly (by swapping with the last object index)
						// Swap the last index into the current index
						objectsPendingDestruction[currentPendingObjIndex] = objectsPendingDestruction[objectsPendingDestructionCount - 1];

						// Decrement the object count
						--objectsPendingDestructionCount;
					}
					else
					{
						// We'll revisit this object the next time around. Move on to the next
						++currentPendingObjIndex;
					}

					// Check time limit
					if ( InIsUseTimeLimit )
					{
						bTimeLimitReached = IsTimeLimitExceeded( startTime, InTimeLimit );
						if ( bTimeLimitReached )
						{
							break;
						}
					}
				}

				if ( objectsPendingDestructionCount > 0 )
				{
					if ( InIsUseTimeLimit )
					{
						// A time limit is set and we've completed a full iteration over all leftover objects, so
						// go ahead and bail out even if we have more time left or objects left to process. It's
						// likely in this case that we're waiting for the render thread
						break;
					}
					else
					{
						// Sleep before the next pass to give the render thread some time to release fences
						Sys_Sleep( 0.f );
					}
				}
			}

			// Have all objects been destroyed now?
			if ( objectsPendingDestructionCount == 0 )
			{
				// Release memory we used for objects pending destruction, leaving some slack space
				objectsPendingDestruction.clear();
				objectsPendingDestruction.reserve( 256 );

				// Destroy has been routed to all objects so it's safe to delete objects now
				bFinishDestroyHasBeenRoutedToAllObjects = true;
				currentPurgeObjectIndex					= 0;
			}
		}
	}

	if ( bFinishDestroyHasBeenRoutedToAllObjects && !bTimeLimitReached )
	{
		// Perform actual object deletion
		while ( currentPurgeObjectIndex < unreachableObjectsIndices.size() )
		{
			CObject*	object = allocatedObjects[ unreachableObjectsIndices[currentPurgeObjectIndex] ];
			Assert( object && object->HasAnyObjectFlags( OBJECT_Unreachable ) );
			AssertMsg( object->HasAllObjectFlags( OBJECT_FinishDestroyed | OBJECT_BeginDestroyed ), TEXT( "%s was not destroyed properly" ), object->GetName().c_str() );
			delete object;

			// Advance to the next object
			currentPurgeObjectIndex++;

			// Check time limit
			if ( InIsUseTimeLimit )
			{
				bTimeLimitReached = IsTimeLimitExceeded( startTime, InTimeLimit );
				if ( bTimeLimitReached )
				{
					break;
				}
			}
		}

		if ( currentPurgeObjectIndex == unreachableObjectsIndices.size() )
		{
			// Log status information
			Logf( TEXT( "GC purged %i objects\n" ), unreachableObjectsIndices.size() );

			// Incremental purge is finished, time to reset variables
			bDelayedBeginDestroyHasBeenRoutedToAllObjects	= false;
			bFinishDestroyHasBeenRoutedToAllObjects			= false;
			bPurgeIsRequired								= false;
			currentPurgeObjectIndex							= 0;
			unreachableObjectsIndices.clear();
		}
	}

	bIsGarbageCollecting = false;
}

/*
==================
CObjectGC::PerformReachabilityAnalysis
==================
*/
void CObjectGC::PerformReachabilityAnalysis( ObjectFlags_t InKeepFlags )
{
	// Presize array and add a bit of extra stack for prefetching
	std::vector<CObject*>	reachableObjects;
	reachableObjects.reserve( allocatedObjects.size() - firstGCIndex + 2 );

	MarkObjectsUnreachable( reachableObjects, InKeepFlags );
	TraverseObjectReferences( reachableObjects );
}

/*
==================
CObjectGC::MarkObjectsUnreachable
==================
*/
void CObjectGC::MarkObjectsUnreachable( std::vector<class CObject*>& InOutReachableObjects, ObjectFlags_t InKeepFlags )
{
	// The CClass class
	const CClass*	CClassClass = CClass::StaticClass();

	// Iterate over all objects
	for ( uint32 objectIndex = firstGCIndex, numObjects = allocatedObjects.size(); objectIndex < numObjects; ++objectIndex )
	{
		CObject*	object = allocatedObjects[objectIndex];

		// Skip NULL entries
		if ( !object )
		{
			continue;
		}

		// Special case handling for objects that are part of the root set
		if ( object->HasAnyObjectFlags( OBJECT_RootSet ) )
		{
			// We cannot use OBJECT_PendingKill on objects that are part of the root set
			Assert( object->IsValid() );
			AssertMsg( !object->HasAnyObjectFlags( OBJECT_PendingKill ), TEXT( "Object %s is part of root set though has been marked OBJECT_PendingKill" ), object->GetName().c_str() );

			InOutReachableObjects.push_back( object );
		}
		// Regular objects
		else
		{
			// Mark objects as unreachable unless they have any of the passed in InKeepFlags set and it's not marked for elimination
			if ( object->HasAnyObjectFlags( InKeepFlags ) && !object->IsPendingKill() )
			{
				InOutReachableObjects.push_back( object );
			}
			else
			{
				object->AddObjectFlag( OBJECT_Unreachable );
			}
		}

		// Assemble token stream for CClass objects. This is only done once for each class
		if ( object->GetClass() == CClassClass )
		{
			CClass*		theClass = Cast<CClass>( object );
			Assert( theClass );

			if ( !theClass->IsAssembledReferenceTokenStream() )
			{
				theClass->AssembleReferenceTokenStream();
			}
		}
	}
}

/*
==================
CObjectGC::TraverseObjectReferences
==================
*/
void CObjectGC::TraverseObjectReferences( std::vector<class CObject*>& InOutReachableObjects )
{
	// Presized recusrion stack for handling arrays and structs
	std::vector<StackEntry>		stack;
	stack.resize( 128 );

	// Keep reachable objects till we reach the end of the growing array at which point we are done
	uint32		currentIndex = 0;
	while ( currentIndex < InOutReachableObjects.size() )
	{
		ProcessObjectForReferences( InOutReachableObjects[currentIndex++], InOutReachableObjects, stack );
	}
}

/*
==================
CObjectGC::ProcessObjectForReferences
==================
*/
void CObjectGC::ProcessObjectForReferences( class CObject* InCurrentObject, std::vector<class CObject*>& InOutNewReachableObjects, std::vector<StackEntry>& InOutStack )
{
	// Add referenced objects to InOutNewReachableObjects
	InCurrentObject->AddReferencedObjects( InOutNewReachableObjects );

	// Make sure that token stream has been assembled at this point as the below code relies on it
	Assert( InCurrentObject->GetClass()->IsAssembledReferenceTokenStream() );

	// Get pointer to token stream and jump to the start
	const CGCReferenceTokenStream*		tokenStream = &InCurrentObject->GetClass()->GetReferenceTokenStream();
	uint32								tokenStreamIndex = 0;	
	uint32								referenceTokenStreamIndex = 0;	// Keep track of index to reference info. Used to avoid LHSs
	uint32								tokenReturnCount = 0;			// Keep track of token return count in separate integer as arrays need to fiddle with it

	// Create strack entry and initialize sane values
	StackEntry*					stackEntry = &InOutStack[ 0 ];
	byte*						stackEntryData = ( byte* )InCurrentObject;
	stackEntry->data			= stackEntryData;
	stackEntry->stride			= 0;
	stackEntry->count			= -1;
	stackEntry->loopStartIndex	= -1;

	// Parse the token stream
	while ( true )
	{
		// Cache current token index as it is the one pointing to the reference info
		referenceTokenStreamIndex = tokenStreamIndex;

		// Handle returning from an array of structs, array of structs of arrays of ... etc
		for ( uint32 returnCount = 0; returnCount < tokenReturnCount; ++returnCount )
		{
			// Make sure there's no stack underflow
			Assert( stackEntry->count != -1 );

			// We pre-decrement as we're already through the loop once at this point
			if ( --stackEntry->count > 0 )
			{
				// Point data to next entry
				stackEntryData				= stackEntry->data + stackEntry->stride;
				stackEntry->data			= stackEntryData;

				// Jump back to the beginning of the loop
				tokenStreamIndex			= stackEntry->loopStartIndex;
				referenceTokenStreamIndex	= stackEntry->loopStartIndex;

				// We're not done with this token loop so we need to early out instead of backing out further
				break;
			}
			else
			{
				--stackEntry;
				stackEntryData = stackEntry->data;
			}
		}

		// Instead of reading information about reference from stream and caching it like below we access
		// the same memory address over and over and over again to avoid a nasty LHS penalty. Not reading 
		// the reference info means we need to manually increment the token index to skip to the next one
		++tokenStreamIndex;

		// Helper to make code more readable and hide the ugliness that is avoiding LHSs from caching
#define	REFERENCE_INFO tokenStream->AccessReferenceInfo( referenceTokenStreamIndex )

		// CObject
		if ( REFERENCE_INFO.type == GCRT_Object )
		{
			// We're dealing with an object reference
			CObject**	objectPtr	= ( CObject** )( stackEntryData + REFERENCE_INFO.offset );
			CObject*&	object		= *objectPtr;
			tokenReturnCount		= REFERENCE_INFO.returnCount;
			HandleObjectReference( InOutNewReachableObjects, object, true );
		}

		// Array of CObject
		else if ( REFERENCE_INFO.type == GCRT_ArrayObject )
		{
			// We're dealing with an array of object references
			std::vector<CObject*>&		objectArray = *( ( std::vector<CObject*>* )( stackEntryData + REFERENCE_INFO.offset ) );
			tokenReturnCount			= REFERENCE_INFO.returnCount;
			
			for ( uint32 objectIndex = 0, numObjects = objectArray.size(); objectIndex < numObjects; ++objectIndex )
			{
				CObject*&		object = objectArray[ objectIndex ];
				HandleObjectReference( InOutNewReachableObjects, object, true );
			}
		}

		// Array of structs with CObject
		else if ( REFERENCE_INFO.type == GCRT_ArrayStruct )
		{
			const std::vector<byte>&	array = *( ( std::vector<byte>* )( stackEntryData + REFERENCE_INFO.offset ) );

			++stackEntry;
			stackEntryData				= ( byte* )array.data();
			stackEntry->data			= stackEntryData;
			stackEntry->stride			= tokenStream->ReadStride( tokenStreamIndex );
			stackEntry->count			= array.size() / stackEntry->stride;	
			
			const GCSkipInfo			skipInfo = tokenStream->ReadSkipInfo( tokenStreamIndex );
			stackEntry->loopStartIndex	= tokenStreamIndex;

			if ( stackEntry->count == 0 )
			{
				// Skip empty array by jumping to skip index and set return count to the one about to be read in
				tokenStreamIndex = skipInfo.skipIndex;
				tokenReturnCount = tokenStream->GetSkipReturnCount( skipInfo );
			}
			else
			{
				// Loop again
				Assert( stackEntry->data );
				tokenReturnCount = 0;
			}
		}

		// Fixed array
		else if ( REFERENCE_INFO.type == GCRT_FixedArray )
		{
			byte*	previousData = stackEntryData;
			stackEntry++;
			stackEntryData				= previousData;
			stackEntry->data			= previousData;
			stackEntry->stride			= tokenStream->ReadStride( tokenStreamIndex );
			stackEntry->count			= tokenStream->ReadCount( tokenStreamIndex );
			stackEntry->loopStartIndex	= tokenStreamIndex;
			tokenReturnCount			= 0;
		}

		// End of stream
		else if ( REFERENCE_INFO.type == GCRT_EndOfStream )
		{
			// Break out of loop
			break;
		}
	}

	Assert( stackEntry == &InOutStack[ 0 ] );
}