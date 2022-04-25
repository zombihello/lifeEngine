#include "Misc/EngineGlobals.h"
#include "Scripts/ScriptEngine.h"
#include "Render/Shaders/ShaderManager.h"
#include "System/InputSystem.h"
#include "System/World.h"
#include "System/CameraManager.h"

// -------------
// GLOBALS
// -------------

FScriptEngine*												GScriptEngine = new FScriptEngine();
FShaderManager*												GShaderManager = new FShaderManager();
std::wstring												GCookedDir = TEXT( "" );
class LBaseEngine*											GEngine = nullptr;
FInputSystem*												GInputSystem = new FInputSystem();
FWorld*														GWorld = new FWorld();
bool														GUseMaxTickRate = true;
bool														GAllowRenderThread = true;
FCameraManager*												GCameraManager = new FCameraManager();
float														GPixelCenterOffset = 0.5f;

#if WITH_EDITOR
std::vector< std::pair< std::wstring, ESurfaceType > >		GSurfaceTypeNames( 1, std::make_pair( TEXT( "Default" ), ST_Default ) );
bool														GIsCookEditorContent = false;
#endif // WITH_EDITOR
