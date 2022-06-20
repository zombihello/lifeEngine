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
    spriteComponent     = CreateComponent< CSpriteComponent >( TEXT( "SpriteComponent0" ) );   
}

ASprite::~ASprite()
{}

#if WITH_EDITOR
std::wstring ASprite::GetActorIcon() const
{
    return TEXT( "Engine/Editor/Icons/CB_Map.png" );
}
#endif // WITH_EDITOR
