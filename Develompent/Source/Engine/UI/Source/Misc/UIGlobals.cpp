#include "LEBuild.h"
#include "Misc/UIGlobals.h"
#include "ImGUI/ImGUIEngine.h"
#include "UIEngine.h"

// -------------
// GLOBALS
// -------------

#if WITH_IMGUI
ÑImGUIEngine*			GImGUIEngine = new ÑImGUIEngine();
#endif // WITH_IMGUI

ÑUIEngine*				GUIEngine = new ÑUIEngine();