#include "Misc/EngineGlobals.h"
#include "Logger/LoggerMacros.h"
#include "System/Config.h"
#include "System/World.h"
#include "Actors/PlayerStart.h"
#include "Actors/PlayerController.h"
#include "Actors/Character.h"

IMPLEMENT_CLASS( APlayerStart )

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
APlayerStart::StaticInitializeClass
==================
*/
void APlayerStart::StaticInitializeClass()
{
	// Native properties
	new( staticClass, NAME_None ) CObjectProperty( CPP_PROPERTY( ThisClass, rootComponent ), NAME_None, TEXT( "" ), CPF_None, CSceneComponent::StaticClass() );

#if WITH_EDITOR
	new( staticClass, NAME_None ) CObjectProperty( CPP_PROPERTY( ThisClass, gizmoComponent ), NAME_None, TEXT( "" ), CPF_None, CSpriteComponent::StaticClass() );
	new( staticClass, NAME_None ) CObjectProperty( CPP_PROPERTY( ThisClass, arrowComponent ), NAME_None, TEXT( "" ), CPF_None, CArrowComponent::StaticClass() );
#endif // WITH_EDITOR
}

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
		classPlayerController = FindObjectFast<CClass>( nullptr, configPlayerController.GetString(), true, true );
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
		classPlayerCharacter = FindObjectFast<CClass>( nullptr, configPlayerCharacter.GetString(), true, true );
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
	APlayerController*		playerController = ( APlayerController* )g_World->SpawnActor( classPlayerController, Vector( 0.f, 0.f, 0.f ) );
	ACharacter*				playerCharacter = ( ACharacter* )g_World->SpawnActor( classPlayerCharacter, GetActorLocation(), GetActorRotation() );
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