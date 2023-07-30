#include "Components/BoxComponent.h"
#include "Render/Scene.h"
#include "Render/SceneUtils.h"
#include "System/World.h"
#include "Misc/EngineGlobals.h"

IMPLEMENT_CLASS( CBoxComponent )

/*
==================
CBoxComponent::CBoxComponent
==================
*/
CBoxComponent::CBoxComponent()
	: size( 1.f, 1.f, 1.f )
{
	if ( g_IsEditor )
	{
		SetVisibility( true );
	}
	bodyInstance.SetSimulatePhysics( true );
}

#if WITH_EDITOR
/*
==================
CBoxComponent::DrawDebugComponent
==================
*/
void CBoxComponent::DrawDebugComponent()
{
	boundbox = CBox::BuildAABB( GetComponentLocation() + size / 2.f, size / 2.f );
	DrawWireframeBox( ( ( CScene* )g_World->GetScene() )->GetSDG( SDG_WorldEdForeground ), boundbox, DEC_COLLISION );
}
#endif // WITH_EDITOR

/*
==================
CBoxComponent::UpdateBodySetup
==================
*/
void CBoxComponent::UpdateBodySetup()
{
	bodySetup = new CPhysicsBodySetup();

	SPhysicsBoxGeometry				boxGeometry( size.x, size.y, size.z );
	boxGeometry.collisionProfile	= collisionProfile;
	boxGeometry.material			= physicsMaterial;
	bodySetup->AddBoxGeometry( boxGeometry );
}

/*
==================
CBoxComponent::Serialize
==================
*/
void CBoxComponent::Serialize( class CArchive& InArchive )
{
	Super::Serialize( InArchive );
	InArchive << size;
}