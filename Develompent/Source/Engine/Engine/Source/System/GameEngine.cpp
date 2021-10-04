#include "Misc/Class.h"
#include "System/GameEngine.h"

IMPLEMENT_CLASS( LGameEngine )

LGameEngine::LGameEngine()
{}

LGameEngine::~LGameEngine()
{}

void LGameEngine::Init()
{
	Super::Init();
}

void LGameEngine::Tick( float InDeltaSeconds )
{}

void LGameEngine::Shutdown()
{
	Super::Shutdown();
}