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

#include "Core/Misc/CoreGlobals.h"
#include "Core/System/BaseWindow.h"
#include "Engine/Actors/PlayerController.h"
#include "Engine/Actors/Character.h"
#include "Engine/Components/CameraComponent.h"

IMPLEMENT_CLASS( APlayerController )
IMPLEMENT_DEFAULT_INITIALIZE_CLASS( APlayerController )

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
	inputComponent = CreateComponent< CInputComponent >( TEXT( "InputComponent0" ) );
}

/*
==================
APlayerController::~APlayerController
==================
*/
APlayerController::~APlayerController()
{}

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