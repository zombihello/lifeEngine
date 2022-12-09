#include "Misc/UIGlobals.h"
#include "UIEngine.h"

#if WITH_IMGUI
#include "ImGUI/ImGUIEngine.h"
#endif // WITH_IMGUI

void CUIEngine::Init()
{
#if WITH_IMGUI
	GImGUIEngine->Init();
#endif // WITH_IMGUI
}

void CUIEngine::Tick( float InDeltaSeconds )
{
#if WITH_IMGUI
	GImGUIEngine->Tick( InDeltaSeconds );
#endif // WITH_IMGUI
}

void CUIEngine::Shutdown()
{
#if WITH_IMGUI
	GImGUIEngine->Shutdown();
#endif // WITH_IMGUI
}

void CUIEngine::ProcessEvent( struct SWindowEvent& InWindowEvent )
{
#if WITH_IMGUI
	GImGUIEngine->ProcessEvent( InWindowEvent );
#endif // WITH_IMGUI
}

void CUIEngine::BeginDraw()
{
#if WITH_IMGUI
	GImGUIEngine->BeginDraw();
#endif // WITH_IMGUI
}

void CUIEngine::EndDraw()
{
#if WITH_IMGUI
	GImGUIEngine->EndDraw();
#endif // WITH_IMGUI
}