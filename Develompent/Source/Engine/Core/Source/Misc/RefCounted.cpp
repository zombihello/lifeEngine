#include "Misc/RefCounted.h"

/**
 * Constructor
 */
RefCounted::RefCounted() :
	countReferences( 0 )
{}

/**
 * Destructor
 */
RefCounted::~RefCounted()
{
	check( !countReferences );
}

/**
 * Decrement reference count and delete self if no more references
 */
void RefCounted::ReleaseRef()
{	 
	if ( !countReferences || !--countReferences )
	{
		delete this;
	}
}
