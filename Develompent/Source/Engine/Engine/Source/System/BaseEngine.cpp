#include "Misc/Class.h"
#include "System/Config.h"
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

float LBaseEngine::GetMaxTickRate() const
{
	FConfigValue		configMaxTickRate = GEngineConfig.GetValue( TEXT( "Engine.Engine" ), TEXT( "MaxTickRate" ) );
	if ( configMaxTickRate.IsValid() )
	{
		return configMaxTickRate.GetNumber();
	}

	return 0.f;
}