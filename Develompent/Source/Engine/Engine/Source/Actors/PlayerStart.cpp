#include "Misc/EngineGlobals.h"
#include "Logger/LoggerMacros.h"
#include "Reflection/ObjectGlobals.h"
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
	new( staticClass, NAME_None ) CObjectProperty( CPP_PROPERTY( ThisClass, gizmoComponent ), NAME_None, TEXT( "" ), CPF_EditorOnly, CSpriteComponent::StaticClass() );
	new( staticClass, NAME_None ) CObjectProperty( CPP_PROPERTY( ThisClass, arrowComponent ), NAME_None, TEXT( "" ), CPF_EditorOnly, CArrowComponent::StaticClass() );
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
	const CJsonValue*	configPlayerController = CConfig::Get().GetValue( CT_Game, TEXT( "Game.GameInfo" ), TEXT( "DefaultPlayerController" ) );
	CClass*				classPlayerController = nullptr;
	if ( configPlayerController && configPlayerController->IsA( JVT_String ) )
	{
		classPlayerController = FindObject<CClass>( ANY_PACKAGE, configPlayerController->GetString().c_str(), true );
	}

	// If not found - use APlayerController
	if ( !classPlayerController )
	{
		classPlayerController = APlayerController::StaticClass();
		if ( configPlayerController && configPlayerController->IsValid() )
		{
			Warnf( TEXT( "Not found default player controller '%s', will be use Engine.APlayerController\n" ), configPlayerController->GetString().c_str() );
		}
		else
		{
			Warnf( TEXT( "Not set default player controller in game config (parameter 'Game.GameInfo:DefaultPlayerController'), will be use Engine.APlayerController\n" ) );
		}
	}

	// Find class of default player character
	const CJsonValue*	configPlayerCharacter = CConfig::Get().GetValue( CT_Game, TEXT( "Game.GameInfo" ), TEXT( "DefaultPlayerCharacter" ) );
	CClass*				classPlayerCharacter = nullptr;
	if ( configPlayerCharacter && configPlayerCharacter->IsA( JVT_String ) )
	{
		classPlayerCharacter = FindObject<CClass>( ANY_PACKAGE, configPlayerCharacter->GetString().c_str(), true );
	}

	// If not found - use ACharacter
	if ( !classPlayerCharacter )
	{
		classPlayerCharacter = ACharacter::StaticClass();
		if ( configPlayerCharacter && configPlayerCharacter->IsValid() )
		{
			Warnf( TEXT( "Not found default player character '%s', will be use Engine.ACharacter\n" ), configPlayerCharacter->GetString().c_str() );
		}
		else
		{
			Warnf( TEXT( "Not set default player character in game config (parameter 'Game.GameInfo:DefaultPlayerCharacter'), will be use Engine.ACharacter\n" ) );
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