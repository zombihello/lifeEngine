#include "Misc/CoreGlobals.h"
#include "System/BaseWindow.h"
#include "Actors/PlayerController.h"
#include "Actors/Character.h"
#include "Components/CameraComponent.h"

IMPLEMENT_CLASS( APlayerController )

APlayerController::APlayerController()
	: bShowMouseCursor( false )
	, bConstrainYaw( true )
	, rotationInput( SMath::vectorZero )
	, viewRotation( SMath::vectorZero )
{
	inputComponent = CreateComponent< CInputComponent >( TEXT( "InputComponent0" ) );
}

APlayerController::~APlayerController()
{}

void APlayerController::BeginPlay()
{
	Super::BeginPlay();
	SetupInputComponent();
}

void APlayerController::Tick( float InDeltaTime )
{
	Super::Tick( InDeltaTime );

	// Update show mouse cursor if need
	if ( bShowMouseCursor != GWindow->IsShowingCursor() )
	{
		if ( bShowMouseCursor )
		{
			GWindow->ShowCursor();
		}
		else
		{
			GWindow->HideCursor();
		}
	}

	// Update rotation
	UpdateRotation( InDeltaTime );
}

void APlayerController::SetupInputComponent()
{}

void APlayerController::UpdateRotation( float InDeltaTime )
{
	if ( rotationInput == SMath::vectorZero )
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

	rotationInput = SMath::vectorZero;
	if ( character )
	{
		character->SetActorRotation( SMath::AnglesToQuaternionZYX( viewRotation ) );
	}
}