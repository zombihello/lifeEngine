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

#include "Engine/Actors/Character.h"
#include "Actors/ELPlayerController.h"

IMPLEMENT_CLASS( AELPlayerController )
IMPLEMENT_DEFAULT_INITIALIZE_CLASS( AELPlayerController )

/*
==================
AELPlayerController::AELPlayerController
==================
*/
AELPlayerController::AELPlayerController()
{
	bShowMouseCursor	= false;
	bConstrainYaw		= true;
}

/*
==================
AELPlayerController::SetupInputComponent
==================
*/
void AELPlayerController::SetupInputComponent()
{
	// Bind actions
	inputComponent->BindAction( TEXT( "Exit" ), IE_Released,	std::bind( &AELPlayerController::ExitFromGame, this ) );
	inputComponent->BindAction( TEXT( "Jump" ), IE_Pressed,		std::bind( &AELPlayerController::Jump, this ) );
	inputComponent->BindAction( TEXT( "Jump" ), IE_Released,	std::bind( &AELPlayerController::StopJump, this ) );

	// Bind axis
	inputComponent->BindAxis( TEXT( "MoveForward" ),	std::bind( &AELPlayerController::MoveForward, this, std::placeholders::_1	) );
	inputComponent->BindAxis( TEXT( "MoveRight" ),		std::bind( &AELPlayerController::MoveRight, this, std::placeholders::_1		) );
	inputComponent->BindAxis( TEXT( "Turn" ),			std::bind( &AELPlayerController::Turn, this, std::placeholders::_1			) );
	inputComponent->BindAxis( TEXT( "LookUp" ),			std::bind( &AELPlayerController::LookUp, this, std::placeholders::_1		) );
}

/*
==================
AELPlayerController::ExitFromGame
==================
*/
void AELPlayerController::ExitFromGame()
{
	g_IsRequestingExit = true;
}

/*
==================
AELPlayerController::MoveForward
==================
*/
void AELPlayerController::MoveForward( float InValue )
{
	character->Walk( character->GetActorForwardVector(), InValue );
}

/*
==================
AELPlayerController::MoveRight
==================
*/
void AELPlayerController::MoveRight( float InValue )
{
	character->Walk( character->GetActorRightVector(), InValue );
}

/*
==================
AELPlayerController::Turn
==================
*/
void AELPlayerController::Turn( float InValue )
{
	AddYawInput( InValue );
}

/*
==================
AELPlayerController::LookUp
==================
*/
void AELPlayerController::LookUp( float InValue )
{
	AddPitchInput( InValue );
}

/*
==================
AELPlayerController::Jump
==================
*/
void AELPlayerController::Jump()
{
	character->Jump();
}

/*
==================
AELPlayerController::StopJump
==================
*/
void AELPlayerController::StopJump()
{
	character->StopJump();
}