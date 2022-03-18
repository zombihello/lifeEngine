#include "Misc/EngineGlobals.h"
#include "Scripts/ScriptEngine.h"
#include "Render/Shaders/ShaderManager.h"
#include "System/InputSystem.h"
#include "System/World.h"

// -------------
// GLOBALS
// -------------

FScriptEngine*		GScriptEngine = new FScriptEngine();
FShaderManager*		GShaderManager = new FShaderManager();
std::wstring		GGameName = TEXT( "" );
class LBaseEngine*	GEngine = nullptr;
FInputSystem*		GInputSystem = new FInputSystem();
FWorld*				GWorld = new FWorld();
bool                GUseMaxTickRate = true;
bool                GAllowRenderThread = true;