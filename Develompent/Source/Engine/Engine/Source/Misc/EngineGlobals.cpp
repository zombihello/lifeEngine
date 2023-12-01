#include "Misc/EngineGlobals.h"
#include "Scripts/ScriptEngine.h"
#include "Render/Shaders/ShaderManager.h"
#include "System/InputSystem.h"
#include "System/World.h"
#include "System/CameraManager.h"
#include "System/ConsoleSystem.h"

// -------------
// GLOBALS
// -------------

CScriptEngine*												g_ScriptEngine = new CScriptEngine();
CShaderManager*												g_ShaderManager = new CShaderManager();
std::wstring												g_CookedDir = TEXT( "" );
class CBaseEngine*											g_Engine = nullptr;
CInputSystem*												g_InputSystem = new CInputSystem();
CWorld*														g_World = nullptr;
bool														g_UseMaxTickRate = true;
CCameraManager*												g_CameraManager = new CCameraManager();

#if WITH_EDITOR
std::vector< std::pair< std::wstring, ESurfaceType > >		g_SurfaceTypeNames( 1, std::make_pair( TEXT( "Default" ), ST_Default ) );
bool														g_IsCookEditorContent = false;
bool														g_AllowDebugShaderDump = false;
#endif // WITH_EDITOR

class CFullScreenMovieSupport*								g_FullScreenMovie = nullptr;
CConsoleSystem												g_ConsoleSystem;
