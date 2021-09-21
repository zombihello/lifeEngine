#include "LEBuild.h"
#include "Misc/UIGlobals.h"
#include "ImGUI/ImGUIEngine.h"
#include "UIEngine.h"

// -------------
// GLOBALS
// -------------

#if WITH_IMGUI
FImGUIEngine*			GImGUIEngine = new FImGUIEngine();
#endif // WITH_IMGUI

FUIEngine*				GUIEngine = new FUIEngine();