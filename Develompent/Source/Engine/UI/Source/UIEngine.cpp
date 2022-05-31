#include "Misc/UIGlobals.h"
#include "UIEngine.h"

#if WITH_IMGUI
#include "ImGUI/ImGUIEngine.h"
#endif // WITH_IMGUI

void ÑUIEngine::Init()
{
#if WITH_IMGUI
	GImGUIEngine->Init();
#endif // WITH_IMGUI
}

void ÑUIEngine::Shutdown()
{
#if WITH_IMGUI
	GImGUIEngine->Shutdown();
#endif // WITH_IMGUI
}

void ÑUIEngine::ProcessEvent( struct SWindowEvent& InWindowEvent )
{
#if WITH_IMGUI
	GImGUIEngine->ProcessEvent( InWindowEvent );
#endif // WITH_IMGUI
}

void ÑUIEngine::BeginDraw()
{
#if WITH_IMGUI
	GImGUIEngine->BeginDraw();
#endif // WITH_IMGUI
}

void ÑUIEngine::EndDraw()
{
#if WITH_IMGUI
	GImGUIEngine->EndDraw();
#endif // WITH_IMGUI
}