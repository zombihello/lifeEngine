#include "Misc/EngineGlobals.h"
#include "Logger/LoggerMacros.h"
#include "System/Config.h"
#include "System/World.h"
#include "Actors/PlayerStart.h"
#include "Actors/PlayerController.h"
#include "Actors/Character.h"

IMPLEMENT_CLASS( APlayerStart )

APlayerStart::APlayerStart()
{
	rootComponent = CreateComponent<CSceneComponent>( TEXT( "RootComponent0" ) );

#if WITH_EDITOR
	gizmoComponent = CreateComponent<CSpriteComponent>( TEXT( "GizmoComponent0" ), true );
	gizmoComponent->SetGizmo( true );
	gizmoComponent->SetType( ST_Rotating );
	gizmoComponent->SetSpriteSize( Vector2D( 64.f, 64.f ) );
	gizmoComponent->SetMaterial( GPackageManager->FindAsset( TEXT( "Material'EditorMaterials:APlayerStart_Gizmo_Mat" ), AT_Material ) );
#endif // WITH_EDITOR
}

APlayerStart::~APlayerStart()
{}

void APlayerStart::BeginPlay()
{
	Super::BeginPlay();

	// Find class of default player controller
	CConfigValue			configPlayerController = GConfig.GetValue( CT_Game, TEXT( "Game.GameInfo" ), TEXT( "DefaultPlayerController" ) );
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
			LE_LOG( LT_Warning, LC_General, TEXT( "Not found default player controller '%s', used APlayerController" ), configPlayerController.GetString().c_str() );
		}
		else
		{
			LE_LOG( LT_Warning, LC_General, TEXT( "Not setted default player controller in game config (parameter 'Game.GameInfo:DefaultPlayerController'), used APlayerController" ) );
		}
	}

	// Find class of default player character
	CConfigValue		configPlayerCharacter = GConfig.GetValue( CT_Game, TEXT( "Game.GameInfo" ), TEXT( "DefaultPlayerCharacter" ) );
	CClass*				classPlayerCharacter = nullptr;
	if ( configPlayerCharacter.IsA( CConfigValue::T_String ) )
	{
		classPlayerCharacter = CClass::StaticFindClass( configPlayerCharacter.GetString().c_str() );
	}

	// If not found - use ACharacter
	if ( !classPlayerController )
	{
		classPlayerCharacter = ACharacter::StaticClass();
		if ( configPlayerCharacter.IsValid() )
		{
			LE_LOG( LT_Warning, LC_General, TEXT( "Not found default player character '%s', used ACharacter" ), configPlayerCharacter.GetString().c_str() );
		}
		else
		{
			LE_LOG( LT_Warning, LC_General, TEXT( "Not setted default player character in game config (parameter 'Game.GameInfo:DefaultPlayerCharacter'), used ACharacter" ) );
		}
	}

	// Spawn player controller and character
	TRefCountPtr< APlayerController >		playerController = GWorld->SpawnActor( classPlayerController, Vector( 0.f, 0.f, 0.f ) );
	TRefCountPtr< ACharacter >				playerCharacter = GWorld->SpawnActor( classPlayerCharacter, GetActorLocation(), GetActorRotation() );
	playerController->SetCharacter( playerCharacter );
}

#if WITH_EDITOR
std::wstring APlayerStart::GetActorIcon() const
{
	return TEXT( "Engine/Editor/Icons/Actor_PlayerStart.png" );
}
#endif // WITH_EDITOR