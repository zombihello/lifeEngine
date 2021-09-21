#include "Misc/UIGlobals.h"
#include "UIEngine.h"

#if WITH_IMGUI
#include "ImGUI/ImGUIEngine.h"
#endif // WITH_IMGUI

void FUIEngine::Init()
{
#if WITH_IMGUI
	GImGUIEngine->Init();
#endif // WITH_IMGUI
}

void FUIEngine::Shutdown()
{
#if WITH_IMGUI
	GImGUIEngine->Shutdown();
#endif // WITH_IMGUI
}

void FUIEngine::ProcessEvent( struct SWindowEvent& InWindowEvent )
{
#if WITH_IMGUI
	GImGUIEngine->ProcessEvent( InWindowEvent );
#endif // WITH_IMGUI
}

void FUIEngine::BeginDraw()
{
#if WITH_IMGUI
	GImGUIEngine->BeginDraw();
#endif // WITH_IMGUI
}

void FUIEngine::EndDraw()
{
#if WITH_IMGUI
	GImGUIEngine->EndDraw();
#endif // WITH_IMGUI
}