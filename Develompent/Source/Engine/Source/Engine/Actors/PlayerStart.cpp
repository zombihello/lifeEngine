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
#include "Core/System/Config.h"
#include "Engine/Misc/EngineGlobals.h"
#include "Engine/System/World.h"
#include "Engine/Actors/PlayerStart.h"
#include "Engine/Actors/PlayerController.h"
#include "Engine/Actors/Character.h"

IMPLEMENT_CLASS( APlayerStart )
IMPLEMENT_DEFAULT_INITIALIZE_CLASS( APlayerStart )

/*
==================
APlayerStart::APlayerStart
==================
*/
APlayerStart::APlayerStart()
{
	rootComponent = CreateComponent<CSceneComponent>( TEXT( "RootComponent0" ) );

#if WITH_EDITOR
	gizmoComponent = CreateComponent<CSpriteComponent>( TEXT( "GizmoComponent0" ), true );
	gizmoComponent->SetGizmo( true );
	gizmoComponent->SetType( ST_Rotating );
	gizmoComponent->SetSpriteSize( Vector2D( 64.f, 64.f ) );
	gizmoComponent->SetMaterial( g_PackageManager->FindAsset( TEXT( "Material'EditorMaterials:APlayerStart_Gizmo_Mat" ), AT_Material ) );

	arrowComponent = CreateComponent<CArrowComponent>( TEXT( "ArrowComponent0" ), true );
#endif // WITH_EDITOR
}

/*
==================
APlayerStart::~APlayerStart
==================
*/
APlayerStart::~APlayerStart()
{}

/*
==================
APlayerStart::BeginPlay
==================
*/
void APlayerStart::BeginPlay()
{
	Super::BeginPlay();

	// Find class of default player controller
	CConfigValue			configPlayerController = g_Config.GetValue( CT_Game, TEXT( "Game.GameInfo" ), TEXT( "DefaultPlayerController" ) );
	CClass*					classPlayerController = nullptr;
	if ( configPlayerController.IsA( CConfigValue::T_String ) )
	{
		classPlayerController = CClass::StaticFindClass( configPlayerController.GetString().c_str() );
	}

	// If not found - use APlayerController
	if ( !classPlayerController )
	{
		classPlayerController = APlayerController::StaticClass();
		if ( configPlayerController.IsValid() )
		{
			Warnf( TEXT( "Not found default player controller '%s', used APlayerController\n" ), configPlayerController.GetString().c_str() );
		}
		else
		{
			Warnf( TEXT( "Not setted default player controller in game config (parameter 'Game.GameInfo:DefaultPlayerController'), used APlayerController\n" ) );
		}
	}

	// Find class of default player character
	CConfigValue		configPlayerCharacter = g_Config.GetValue( CT_Game, TEXT( "Game.GameInfo" ), TEXT( "DefaultPlayerCharacter" ) );
	CClass*				classPlayerCharacter = nullptr;
	if ( configPlayerCharacter.IsA( CConfigValue::T_String ) )
	{
		classPlayerCharacter = CClass::StaticFindClass( configPlayerCharacter.GetString().c_str() );
	}

	// If not found - use ACharacter
	if ( !classPlayerCharacter )
	{
		classPlayerCharacter = ACharacter::StaticClass();
		if ( configPlayerCharacter.IsValid() )
		{
			Warnf( TEXT( "Not found default player character '%s', used ACharacter\n" ), configPlayerCharacter.GetString().c_str() );
		}
		else
		{
			Warnf( TEXT( "Not setted default player character in game config (parameter 'Game.GameInfo:DefaultPlayerCharacter'), used ACharacter\n" ) );
		}
	}

	// Spawn player controller and character
	TRefCountPtr< APlayerController >		playerController = g_World->SpawnActor( classPlayerController, Vector( 0.f, 0.f, 0.f ) );
	TRefCountPtr< ACharacter >				playerCharacter = g_World->SpawnActor( classPlayerCharacter, GetActorLocation(), GetActorRotation() );
	playerController->SetCharacter( playerCharacter );
}

#if WITH_EDITOR
/*
==================
APlayerStart::GetActorIcon
==================
*/
std::wstring APlayerStart::GetActorIcon() const
{
	return TEXT( "Engine/Editor/Icons/Actor_PlayerStart.png" );
}
#endif // WITH_EDITOR