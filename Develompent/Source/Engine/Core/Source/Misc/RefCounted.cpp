#include "Misc/RefCounted.h"

/**
 * Constructor
 */
FRefCounted::FRefCounted() :
	countReferences( 0 )
{}

/**
 * Destructor
 */
FRefCounted::~FRefCounted()
{
	check( !countReferences );
}

/**
 * Decrement reference count and delete self if no more references
 */
void FRefCounted::ReleaseRef()
{	 
	if ( !countReferences || !appInterlockedDecrement( ( int32* )&countReferences ) )
	{
		delete this;
	}
}
