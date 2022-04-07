#include "Misc/EngineGlobals.h"
#include "Logger/LoggerMacros.h"
#include "System/Config.h"
#include "System/World.h"
#include "Actors/Sprite.h"
#include "Misc/CoreGlobals.h"
#include "System/Package.h"

IMPLEMENT_CLASS( ASprite )

ASprite::ASprite()
{
    spriteComponent = CreateComponent< LSpriteComponent >( TEXT( "SpriteComponent0" ) );   
    boxComponent = CreateComponent< LBoxComponent >( TEXT( "BoxComponent0" ) );
}

ASprite::~ASprite()
{}
