#include "LEBuild.h"
#include "Misc/UIGlobals.h"
#include "ImGUI/ImGUIEngine.h"
#include "UIEngine.h"

// -------------
// GLOBALS
// -------------

#if WITH_IMGUI
ĐImGUIEngine*			GImGUIEngine = new ĐImGUIEngine();
#endif // WITH_IMGUI

ĐUIEngine*				GUIEngine = new ĐUIEngine();