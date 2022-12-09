#include "LEBuild.h"
#include "Misc/UIGlobals.h"
#include "ImGUI/ImGUIEngine.h"
#include "UIEngine.h"

// -------------
// GLOBALS
// -------------

#if WITH_IMGUI
CImGUIEngine*			GImGUIEngine = new CImGUIEngine();
#endif // WITH_IMGUI

CUIEngine*				GUIEngine = new CUIEngine();