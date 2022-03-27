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
	rootComponent = CreateComponent< LSceneComponent >( TEXT( "RootComponent0" ) );
}

APlayerStart::~APlayerStart()
{}

void APlayerStart::BeginPlay()
{
	// Find class of default player controller
	FConfigValue			configPlayerController = GGameConfig.GetValue( TEXT( "Game.GameInfo" ), TEXT( "DefaultPlayerController" ) );
	LClass*					classPlayerController = nullptr;
	if ( configPlayerController.IsA( FConfigValue::T_String ) )
	{
		classPlayerController = LClass::StaticFindClass( configPlayerController.GetString().c_str() );
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
	FConfigValue		configPlayerCharacter = GGameConfig.GetValue( TEXT( "Game.GameInfo" ), TEXT( "DefaultPlayerCharacter" ) );
	LClass*				classPlayerCharacter = nullptr;
	if ( configPlayerCharacter.IsA( FConfigValue::T_String ) )
	{
		classPlayerCharacter = LClass::StaticFindClass( configPlayerCharacter.GetString().c_str() );
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
	TRefCountPtr< APlayerController >		playerController = GWorld->SpawnActor( classPlayerController, FVector( 0.f, 0.f, 0.f ) );
	TRefCountPtr< ACharacter >				playerCharacter = GWorld->SpawnActor( classPlayerCharacter, GetActorLocation(), GetActorRotation() );
	playerController->SetCharacter( playerCharacter );
}
