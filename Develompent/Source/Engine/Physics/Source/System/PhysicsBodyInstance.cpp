#include "Misc/PhysicsGlobals.h"
#include "System/PhysicsEngine.h"
#include "System/PhysicsBodyInstance.h"
#include "Components/PrimitiveComponent.h"

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