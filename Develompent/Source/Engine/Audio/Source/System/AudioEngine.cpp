#include "Misc/AudioGlobals.h"
#include "System/AudioEngine.h"

void CAudioEngine::Init()
{
	GAudioDevice.Init();
}

void CAudioEngine::Shutdown()
{
	GAudioDevice.Shutdown();
}