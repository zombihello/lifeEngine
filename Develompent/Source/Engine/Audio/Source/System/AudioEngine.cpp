#include "Misc/AudioGlobals.h"
#include "System/AudioEngine.h"

void FAudioEngine::Init()
{
	GAudioDevice.Init();
}

void FAudioEngine::Shutdown()
{
	GAudioDevice.Shutdown();
}