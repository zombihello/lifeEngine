/**
 * ************************************************************
 *                  This file is part of:
 *                      LIFEENGINE
 *          https://github.com/zombihello/lifeEngine
 * ************************************************************
 * Copyright (C) 2024 Yehor Pohuliaka.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "Core/Misc/Property.h"
#include "Physics/Misc/PhysicsTypes.h"
#include "Physics/Misc/PhysicsGlobals.h"
#include "Physics/System/PhysicsEngine.h"

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