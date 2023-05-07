#include "LEBuild.h"
#include "Misc/UIGlobals.h"
#include "ImGUI/ImGUIEngine.h"
#include "UIEngine.h"

// -------------
// GLOBALS
// -------------

#if WITH_IMGUI
CImGUIEngine*			g_ImGUIEngine = new CImGUIEngine();
#endif // WITH_IMGUI

CUIEngine*				g_UIEngine = new CUIEngine();