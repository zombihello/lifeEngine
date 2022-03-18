#include "Components/PrimitiveComponent.h"
#include "Misc/EngineGlobals.h"
#include "System/World.h"
#include "Render/Scene.h"

IMPLEMENT_CLASS( LPrimitiveComponent )

LPrimitiveComponent::LPrimitiveComponent()
	: bVisibility( true )
{}

void LPrimitiveComponent::BeginPlay()
{
	Super::BeginPlay();
	SetVisibility( bVisibility );
}

void LPrimitiveComponent::SetVisibility( bool InNewVisibility )
{
	bVisibility = InNewVisibility;
	FBaseScene*		scene = GWorld->GetScene();
	check( scene );

	if ( bVisibility )
	{
		scene->AddPrimitive( this );
	}
	else
	{
		scene->RemovePrimitive( this );
	}
}

void LPrimitiveComponent::AddToDrawList( class FScene* InScene, const class FSceneView& InSceneView )
{}
