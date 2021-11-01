#include "Misc/EngineGlobals.h"
#include "Logger/LoggerMacros.h"
#include "System/Config.h"
#include "System/World.h"
#include "Actors/PlayerStart.h"
#include "Actors/PlayerController.h"

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

	GWorld->SpawnActor( classPlayerController, GetActorLocation(), GetActorRotation() );
}