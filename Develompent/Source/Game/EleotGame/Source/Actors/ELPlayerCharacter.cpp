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

#include "Core/Logger/LoggerMacros.h"
#include "Core/System/Package.h"
#include "Engine/System/World.h"
#include "Engine/System/CameraManager.h"
#include "Engine/Components/BoxComponent.h"
#include "Engine/Misc/EngineGlobals.h"
#include "Actors/ELPlayerCharacter.h"

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
AELPlayerCharacter::~AELPlayerCharacter
==================
*/
AELPlayerCharacter::~AELPlayerCharacter()
{
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