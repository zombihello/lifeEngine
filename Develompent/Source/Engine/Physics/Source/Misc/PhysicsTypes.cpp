#include "Misc/PhysicsTypes.h"
#include "Misc/PhysicsGlobals.h"
#include "Misc/Property.h"
#include "System/PhysicsEngine.h"

IMPLEMENT_STRUCT( CollisionProfile )
IMPLEMENT_ENUM( ECollisionChannel, FOREACH_ENUM_COLLISIONCHANNEL )
IMPLEMENT_ENUM( ECollisionResponse, FOREACH_ENUM_COLLISIONRESPONSE )

const std::wstring		CollisionProfile::noCollision_ProfileName		= TEXT( "NoCollision" );
const std::wstring		CollisionProfile::blockAll_ProfileName			= TEXT( "BlockAll" );
CollisionQueryParams	CollisionQueryParams::defaultQueryParam;

/*
==================
StaticInitializeStruct
==================
*/
void CollisionProfile::StaticInitializeStruct()
{
	new( staticStruct, TEXT( "Object Type" ) )		CByteProperty( NAME_None, TEXT( "Object type" ), STRUCT_OFFSET( ThisStruct, objectType ), CPF_Edit, Enum::GetECollisionChannel() );
	new( staticStruct, TEXT( "Responses" ) )		CByteProperty( NAME_None, TEXT( "Map of responses each collision channel" ), STRUCT_OFFSET( ThisStruct, responses ), CPF_Edit, Enum::GetECollisionResponse(), CC_Max );
}

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