#include "Misc/RefCounted.h"

/*
==================
CRefCounted::CRefCounted
==================
*/
CRefCounted::CRefCounted() :
	countReferences( 0 )
{}

/*
==================
CRefCounted::~CRefCounted
==================
*/
CRefCounted::~CRefCounted()
{
	Assert( !countReferences );
}

/*
==================
CRefCounted::ReleaseRef
==================
*/
void CRefCounted::ReleaseRef()
{	 
	if ( !countReferences || !Sys_InterlockedDecrement( ( int32* )&countReferences ) )
	{
		delete this;
	}
}
