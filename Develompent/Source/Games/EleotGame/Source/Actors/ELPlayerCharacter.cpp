#include "Actors/ELPlayerCharacter.h"
#include "System/World.h"
#include "System/CameraManager.h"
#include "System/Package.h"
#include "Components/BoxComponent.h"
#include "Misc/EngineGlobals.h"
#include "Logger/LoggerMacros.h"

IMPLEMENT_CLASS( AELPlayerCharacter )

/*
==================
AELPlayerCharacter::AELPlayerCharacter
==================
*/
AELPlayerCharacter::AELPlayerCharacter()
{
	// Create camera component
	cameraComponent = CreateComponent<CCameraComponent>( TEXT( "CameraComponent0" ) );
	cameraComponent->SetProjectionMode( CPM_Perspective );
	cameraComponent->SetAutoViewData( true );
	cameraComponent->SetFieldOfView( 90.f );
}

/*
==================
AELPlayerCharacter::BeginDestroy
==================
*/
void AELPlayerCharacter::BeginDestroy()
{
	Super::BeginDestroy();

	// Deactive player camera if need
	if ( cameraComponent->IsActive() )
	{
		g_CameraManager->SetActiveCamera( nullptr );
	}
}

/*
==================
AELPlayerCharacter::StaticInitializeClass
==================
*/
void AELPlayerCharacter::StaticInitializeClass()
{
	new( staticClass, TEXT( "Camera Component" ) ) CObjectProperty( TEXT( "Camera" ), TEXT( "Camera component" ), STRUCT_OFFSET( ThisClass, cameraComponent ), CPF_Edit, CCameraComponent::StaticClass() );
}

/*
==================
AELPlayerCharacter::BeginPlay
==================
*/
void AELPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Set player camera to active
	g_CameraManager->SetActiveCamera( cameraComponent );
}