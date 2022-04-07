#include "Components/ShapeComponent.h"

IMPLEMENT_CLASS( LShapeComponent )

LShapeComponent::LShapeComponent()
{
	SetVisibility( false );
}

LShapeComponent::~LShapeComponent()
{}

void LShapeComponent::BeginPlay()
{
	Super::BeginPlay();
	UpdateBodySetup();
}