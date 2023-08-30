#include "Misc/UIGlobals.h"
#include "UIEngine.h"

#if WITH_IMGUI
#include "ImGUI/ImGUIEngine.h"
#endif // WITH_IMGUI

/*
==================
CUIEngine::Init
==================
*/
void CUIEngine::Init()
{
#if WITH_IMGUI
	g_ImGUIEngine->Init();
#endif // WITH_IMGUI
}

/*
==================
CUIEngine::Tick
==================
*/
void CUIEngine::Tick( float InDeltaSeconds )
{
#if WITH_IMGUI
	g_ImGUIEngine->Tick( InDeltaSeconds );
#endif // WITH_IMGUI
}

/*
==================
CUIEngine::Shutdown
==================
*/
void CUIEngine::Shutdown()
{
#if WITH_IMGUI
	g_ImGUIEngine->Shutdown();
#endif // WITH_IMGUI
}

/*
==================
CUIEngine::ProcessEvent
==================
*/
void CUIEngine::ProcessEvent( struct WindowEvent& InWindowEvent )
{
#if WITH_IMGUI
	g_ImGUIEngine->ProcessEvent( InWindowEvent );
#endif // WITH_IMGUI
}

/*
==================
CUIEngine::BeginDraw
==================
*/
void CUIEngine::BeginDraw()
{
#if WITH_IMGUI
	g_ImGUIEngine->BeginDraw();
#endif // WITH_IMGUI
}

/*
==================
CUIEngine::EndDraw
==================
*/
void CUIEngine::EndDraw()
{
#if WITH_IMGUI
	g_ImGUIEngine->EndDraw();
#endif // WITH_IMGUI
}