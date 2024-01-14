#include "Misc/CoreGlobals.h"
#include "System/BaseWindow.h"
#include "Actors/PlayerController.h"
#include "Actors/Character.h"
#include "Components/CameraComponent.h"

IMPLEMENT_CLASS( APlayerController )

/*
==================
APlayerController::APlayerController
==================
*/
APlayerController::APlayerController()
	: bShowMouseCursor( false )
	, bConstrainYaw( true )
	, rotationInput( Math::vectorZero )
	, viewRotation( Math::vectorZero )
{
	inputComponent = CreateComponent<CInputComponent>( TEXT( "InputComponent0" ) );
}

/*
==================
APlayerController::StaticInitializeClass
==================
*/
void APlayerController::StaticInitializeClass()
{
	// Native properties
	new( staticClass, NAME_None, OBJECT_Public ) CObjectProperty( CPP_PROPERTY( ThisClass, inputComponent ), NAME_None, TEXT( "" ), CPF_None, CInputComponent::StaticClass() );
}

/*
==================
APlayerController::BeginPlay
==================
*/
void APlayerController::BeginPlay()
{
	Super::BeginPlay();
	SetupInputComponent();
}

/*
==================
APlayerController::Tick
==================
*/
void APlayerController::Tick( float InDeltaTime )
{
	Super::Tick( InDeltaTime );

	// Update show mouse cursor if need
	if ( bShowMouseCursor != g_Window->IsShowingCursor() )
	{
		if ( bShowMouseCursor )
		{
			g_Window->ShowCursor();
		}
		else
		{
			g_Window->HideCursor();
		}
	}

	// Update rotation
	UpdateRotation( InDeltaTime );
}

/*
==================
APlayerController::SetupInputComponent
==================
*/
void APlayerController::SetupInputComponent()
{}

/*
==================
APlayerController::UpdateRotation
==================
*/
void APlayerController::UpdateRotation( float InDeltaTime )
{
	if ( rotationInput == Math::vectorZero )
	{
		return;
	}

	if ( rotationInput.x != 0.f )
	{
		viewRotation.x += rotationInput.x;
		if ( bConstrainYaw )
		{
			if ( viewRotation.x > 90.f )
			{
				viewRotation.x = 90.f;
			}
			else if ( viewRotation.x < -90.f )
			{
				viewRotation.x = -90.f;
			}
		}
		else if ( viewRotation.x < -360.f || viewRotation.x > 360.f )
		{
			viewRotation.x = 0.f;
		}
	}

	if ( rotationInput.y != 0.f )
	{
		viewRotation.y -= rotationInput.y;
		if ( viewRotation.y < -360.f || viewRotation.y > 360.f )
		{
			viewRotation.y = 0.f;
		}
	}

	rotationInput = Math::vectorZero;
	if ( character )
	{
		character->SetActorRotation( Math::AnglesToQuaternion( viewRotation ) );
	}
}