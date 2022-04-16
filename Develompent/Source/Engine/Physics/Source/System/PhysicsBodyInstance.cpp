#include "Misc/PhysicsGlobals.h"
#include "System/PhysicsEngine.h"
#include "System/PhysicsBodyInstance.h"
#include "Components/PrimitiveComponent.h"

FPhysicsBodyInstance::FPhysicsBodyInstance()
	: bStatic( true )
	, bEnableGravity( false )
	, bSimulatePhysics( false )
	, bStartAwake( true )
	, bDirty( false )
	, lockFlags( BLF_None )
	, mass( 1.f )
{}

FPhysicsBodyInstance::~FPhysicsBodyInstance()
{
	TermBody();
}

void FPhysicsBodyInstance::InitBody( FPhysicsBodySetup* InBodySetup, const FTransform& InTransform, LPrimitiveComponent* InPrimComp )
{
	bDirty = false;

	// If body is inited - terminate body for reinit
	if ( FPhysicsInterface::IsValidActor( handle ) )
	{
		TermBody();
	}

	check( InBodySetup );
	ownerComponent	= InPrimComp;
	bodySetup		= InBodySetup;

	FActorCreationParams	params;
	params.bStatic			= bStatic;
	params.lockFlags		= lockFlags;
	params.initialTM		= InTransform;
	params.mass				= mass;
	params.bSimulatePhysics = bSimulatePhysics;
	params.bEnableGravity	= bEnableGravity;
	params.bStartAwake		= bStartAwake;
	handle = FPhysicsInterface::CreateActor( params );
	check( FPhysicsInterface::IsValidActor( handle ) );

	// Attach all shapes in body setup to physics actor
	// Box shapes
	{
		std::vector< FPhysicsBoxGeometry >&		boxGeometries = bodySetup->GetBoxGeometries();
		for ( uint32 index = 0, count = boxGeometries.size(); index < count; ++index )
		{
			FPhysicsInterface::AttachShape( handle, boxGeometries[ index ].GetShapeHandle() );
		}
	}

	// Update mass and inertia if rigid body is not static
	if ( !bStatic )
	{
		FPhysicsInterface::UpdateMassAndInertia( handle, 10.f );
	}

	// Add rigid body to physics scene
	GPhysicsScene.AddBody( this );
}

void FPhysicsBodyInstance::TermBody()
{
	if ( !FPhysicsInterface::IsValidActor( handle ) )
	{
		return;
	}

	// Remove from scene
	GPhysicsScene.RemoveBody( this );

	// Release resource
	FPhysicsInterface::ReleaseActor( handle );
	ownerComponent = nullptr;
	bodySetup = nullptr;
	bDirty = false;
}