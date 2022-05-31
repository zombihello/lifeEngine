#include "Misc/RefCounted.h"

/**
 * Constructor
 */
CRefCounted::CRefCounted() :
	countReferences( 0 )
{}

/**
 * Destructor
 */
CRefCounted::~CRefCounted()
{
	check( !countReferences );
}

/**
 * Decrement reference count and delete self if no more references
 */
void CRefCounted::ReleaseRef()
{	 
	if ( !countReferences || !appInterlockedDecrement( ( int32* )&countReferences ) )
	{
		delete this;
	}
}
