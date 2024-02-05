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

#include "Physics/Misc/PhysicsGlobals.h"
#include "Physics/System/PhysicsEngine.h"
#include "Physics/System/PhysicsBodyInstance.h"
#include "Engine/Components/PrimitiveComponent.h"

/*
==================
CPhysicsBodyInstance::CPhysicsBodyInstance
==================
*/
CPhysicsBodyInstance::CPhysicsBodyInstance()
	: bStatic( true )
	, bEnableGravity( false )
	, bSimulatePhysics( false )
	, bStartAwake( true )
	, bDirty( false )
	, lockFlags( BLF_None )
	, mass( 1.f )
{}

/*
==================
CPhysicsBodyInstance::~CPhysicsBodyInstance
==================
*/
CPhysicsBodyInstance::~CPhysicsBodyInstance()
{
	TermBody();
}

/*
==================
CPhysicsBodyInstance::InitBody
==================
*/
void CPhysicsBodyInstance::InitBody( CPhysicsBodySetup* InBodySetup, const CTransform& InTransform, CPrimitiveComponent* InPrimComp )
{
	bDirty = false;

	// If body is inited - terminate body for reinit
	if ( CPhysicsInterface::IsValidActor( handle ) )
	{
		TermBody();
	}

	Assert( InBodySetup );
	ownerComponent	= InPrimComp;
	bodySetup		= InBodySetup;

	ActorCreationParams	params;
	params.bStatic			= bStatic;
	params.lockFlags		= lockFlags;
	params.initialTM		= InTransform;
	params.mass				= mass;
	params.bSimulatePhysics = bSimulatePhysics;
	params.bEnableGravity	= bEnableGravity;
	params.bStartAwake		= bStartAwake;
	handle = CPhysicsInterface::CreateActor( params );
	Assert( CPhysicsInterface::IsValidActor( handle ) );

	// Attach all shapes in body setup to physics actor
	// Box shapes
	{
		std::vector< PhysicsBoxGeometry >&		boxGeometries = bodySetup->GetBoxGeometries();
		for ( uint32 index = 0, count = boxGeometries.size(); index < count; ++index )
		{
			CPhysicsInterface::AttachShape( handle, boxGeometries[ index ].GetShapeHandle() );
		}
	}

	// Update mass and inertia if rigid body is not static
	if ( !bStatic )
	{
		CPhysicsInterface::UpdateMassAndInertia( handle, 10.f );
	}

	// Add rigid body to physics scene
	g_PhysicsScene.AddBody( this );
}

/*
==================
CPhysicsBodyInstance::TermBody
==================
*/
void CPhysicsBodyInstance::TermBody()
{
	if ( !CPhysicsInterface::IsValidActor( handle ) )
	{
		return;
	}

	// Remove from scene
	g_PhysicsScene.RemoveBody( this );

	// Release resource
	CPhysicsInterface::ReleaseActor( handle );
	ownerComponent = nullptr;
	bodySetup = nullptr;
	bDirty = false;
}