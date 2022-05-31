#include "Misc/AudioGlobals.h"
#include "System/AudioEngine.h"

void ÑAudioEngine::Init()
{
	GAudioDevice.Init();
}

void ÑAudioEngine::Shutdown()
{
	GAudioDevice.Shutdown();
}