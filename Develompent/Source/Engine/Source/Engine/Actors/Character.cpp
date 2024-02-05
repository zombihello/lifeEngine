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

IMPLEMENT_CLASS( ACharacter )

/*
==================
ACharacter::ACharacter
==================
*/
ACharacter::ACharacter()
{
	characterMovement = CreateComponent< CCharacterMovementComponent >( TEXT( "CharacterMovementComponent0" ) );
}

/*
==================
ACharacter::~ACharacter
==================
*/
ACharacter::~ACharacter()
{}

/*
==================
ACharacter::StaticInitializeClass
==================
*/
void ACharacter::StaticInitializeClass()
{
	new( staticClass, TEXT( "Character Movement" ) ) CObjectProperty( TEXT( "Character" ), TEXT( "Character movement component" ), STRUCT_OFFSET( ThisClass, characterMovement ), CPF_Edit, CCharacterMovementComponent::StaticClass() );
}

/*
==================
ACharacter::Walk
==================
*/
void ACharacter::Walk( const Vector& InWorldDirection, float InScale )
{
	characterMovement->Walk( InWorldDirection, InScale );
}

/*
==================
ACharacter::Jump
==================
*/
void ACharacter::Jump()
{
	characterMovement->Jump();
}

/*
==================
ACharacter::StopJump
==================
*/
void ACharacter::StopJump()
{
	characterMovement->StopJump();
}

/*
==================
ACharacter::Landed
==================
*/
void ACharacter::Landed()
{}