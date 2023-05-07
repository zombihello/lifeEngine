#include "Misc/AudioGlobals.h"
#include "System/AudioEngine.h"

/*
==================
CAudioEngine::Init
==================
*/
void CAudioEngine::Init()
{
	g_AudioDevice.Init();
}

/*
==================
CAudioEngine::Shutdown
==================
*/
void CAudioEngine::Shutdown()
{
	g_AudioDevice.Shutdown();
}