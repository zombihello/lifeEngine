#include "Misc/EngineGlobals.h"
#include "Scripts/ScriptEngine.h"
#include "Render/Shaders/ShaderManager.h"
#include "System/InputSystem.h"
#include "System/World.h"
#include "System/CameraManager.h"

// -------------
// GLOBALS
// -------------

CScriptEngine*												GScriptEngine = new CScriptEngine();
CShaderManager*												GShaderManager = new CShaderManager();
std::wstring												GCookedDir = TEXT( "" );
class CBaseEngine*											GEngine = nullptr;
CInputSystem*												GInputSystem = new CInputSystem();
CWorld*														GWorld = new CWorld();
bool														GUseMaxTickRate = true;
bool														GAllowRenderThread = true;
CCameraManager*												GCameraManager = new CCameraManager();

#if WITH_EDITOR
std::vector< std::pair< std::wstring, ESurfaceType > >		GSurfaceTypeNames( 1, std::make_pair( TEXT( "Default" ), ST_Default ) );
bool														GIsCookEditorContent = false;
bool														GAllowDebugShaderDump = false;
#endif // WITH_EDITOR

class CFullScreenMovieSupport*								GFullScreenMovie = nullptr;
