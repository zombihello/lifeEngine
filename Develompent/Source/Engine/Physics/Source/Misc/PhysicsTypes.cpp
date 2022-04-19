#include "Misc/PhysicsTypes.h"
#include "Misc/PhysicsGlobals.h"
#include "System/PhysicsEngine.h"

const std::wstring		FCollisionProfile::noCollision_ProfileName		= TEXT( "NoCollision" );
const std::wstring		FCollisionProfile::blockAll_ProfileName			= TEXT( "BlockAll" );
FCollisionQueryParams	FCollisionQueryParams::defaultQueryParam;

FArchive& operator<<( FArchive& InAr, FCollisionProfile*& InCollisionProfile )
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
			InCollisionProfile = GPhysicsEngine.FindCollisionProfile( FCollisionProfile::noCollision_ProfileName );
			check( InCollisionProfile );
		}
	}

	return InAr;
}

FArchive& operator<<( FArchive& InAr, const FCollisionProfile*& InCollisionProfile )
{
	check( InAr.IsSaving() && InCollisionProfile );
	InAr << InCollisionProfile->name;
	return InAr;
}