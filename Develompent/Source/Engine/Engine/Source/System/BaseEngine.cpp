#include "Misc/Class.h"
#include "Misc/EngineGlobals.h"
#include "System/BaseEngine.h"
#include "System/InputSystem.h"

IMPLEMENT_CLASS( LBaseEngine )

void LBaseEngine::Init()
{
	GInputSystem->Init();
}

void LBaseEngine::ProcessEvent( struct SWindowEvent& InWindowEvent )
{
	GInputSystem->ProcessEvent( InWindowEvent );
}