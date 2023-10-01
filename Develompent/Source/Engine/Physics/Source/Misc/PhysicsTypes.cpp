#include "Misc/PhysicsTypes.h"
#include "Misc/PhysicsGlobals.h"
#include "Misc/Property.h"
#include "System/PhysicsEngine.h"

IMPLEMENT_ENUM( ECollisionChannel, FOREACH_ENUM_COLLISIONCHANNEL )
IMPLEMENT_ENUM( ECollisionResponse, FOREACH_ENUM_COLLISIONRESPONSE )

const std::wstring		CollisionProfile::noCollision_ProfileName		= TEXT( "NoCollision" );
const std::wstring		CollisionProfile::blockAll_ProfileName			= TEXT( "BlockAll" );
CollisionQueryParams	CollisionQueryParams::defaultQueryParam;

/*
==================
operator<<
==================
*/
CArchive& operator<<( CArchive& InAr, CollisionProfile*& InCollisionProfile )
{
	if ( InAr.IsSaving() )
	{
		Assert( InCollisionProfile );
		InAr << InCollisionProfile->name;
	}
	else
	{
		std::wstring		profileName;
		InAr << profileName;
		InCollisionProfile = g_PhysicsEngine.FindCollisionProfile( profileName );
		if ( !InCollisionProfile )
		{
			InCollisionProfile = g_PhysicsEngine.FindCollisionProfile( CollisionProfile::noCollision_ProfileName );
			Assert( InCollisionProfile );
		}
	}

	return InAr;
}

/*
==================
operator<<
==================
*/
CArchive& operator<<( CArchive& InAr, const CollisionProfile*& InCollisionProfile )
{
	Assert( InAr.IsSaving() && InCollisionProfile );
	InAr << InCollisionProfile->name;
	return InAr;
}