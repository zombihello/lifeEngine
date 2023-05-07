#include "Misc/PhysicsTypes.h"
#include "Misc/PhysicsGlobals.h"
#include "System/PhysicsEngine.h"

const std::wstring		SCollisionProfile::noCollision_ProfileName		= TEXT( "NoCollision" );
const std::wstring		SCollisionProfile::blockAll_ProfileName			= TEXT( "BlockAll" );
SCollisionQueryParams	SCollisionQueryParams::defaultQueryParam;

/*
==================
operator<<
==================
*/
CArchive& operator<<( CArchive& InAr, SCollisionProfile*& InCollisionProfile )
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
			InCollisionProfile = g_PhysicsEngine.FindCollisionProfile( SCollisionProfile::noCollision_ProfileName );
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
CArchive& operator<<( CArchive& InAr, const SCollisionProfile*& InCollisionProfile )
{
	Assert( InAr.IsSaving() && InCollisionProfile );
	InAr << InCollisionProfile->name;
	return InAr;
}