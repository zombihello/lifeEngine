#include "Misc/PhysicsTypes.h"
#include "Misc/PhysicsGlobals.h"
#include "System/PhysicsEngine.h"

const std::wstring		SCollisionProfile::noCollision_ProfileName		= TEXT( "NoCollision" );
const std::wstring		SCollisionProfile::blockAll_ProfileName			= TEXT( "BlockAll" );
SCollisionQueryParams	SCollisionQueryParams::defaultQueryParam;

CArchive& operator<<( CArchive& InAr, SCollisionProfile*& InCollisionProfile )
{
	if ( InAr.IsSaving() )
	{
		check( InCollisionProfile );
		InAr << InCollisionProfile->name;
	}
	else
	{
		std::wstring		profileName;
		InAr << profileName;
		InCollisionProfile = GPhysicsEngine.FindCollisionProfile( profileName );
		if ( !InCollisionProfile )
		{
			InCollisionProfile = GPhysicsEngine.FindCollisionProfile( SCollisionProfile::noCollision_ProfileName );
			check( InCollisionProfile );
		}
	}

	return InAr;
}

CArchive& operator<<( CArchive& InAr, const SCollisionProfile*& InCollisionProfile )
{
	check( InAr.IsSaving() && InCollisionProfile );
	InAr << InCollisionProfile->name;
	return InAr;
}