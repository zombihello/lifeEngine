// AngleScript
#include <angelscript.h>

// AngleScript addons
#include "Scripts/AngelScriptAddons/scriptstdstring/scriptstdstring.h"

#include "Containers/StringConv.h"
#include "Logger/LoggerMacros.h"
#include "Scripts/ScriptEngine.h"

/**
 * Message callback from AngleScript engine
 */
void CDECL ASMessageCallback( const asSMessageInfo* InMessage, void* InParam )
{
	check( InMessage );

	ELogType		logType = LT_Log;
	switch ( InMessage->type )
	{
	case asMSGTYPE_WARNING:
		logType = LT_Warning;
		break;

	case asMSGTYPE_ERROR:
		logType = LT_Error;
		break;
	}

	LE_LOG( logType, LC_Script, TEXT( "%s (%d, %d) : %s" ), ANSI_TO_TCHAR( InMessage->section ), InMessage->row, InMessage->col, ANSI_TO_TCHAR( InMessage->message ) );
}

/**
 * Constructor
 */
ScriptEngine::ScriptEngine() :
	asScriptEngine( nullptr )
{}

/**
 * Destructor
 */
ScriptEngine::~ScriptEngine()
{
	if ( !asScriptEngine )		return;
	asScriptEngine->ShutDownAndRelease();
}

/**
 * Initialize
 */
void ScriptEngine::Init()
{
	if ( asScriptEngine )		return;

	// Create script engine
	asScriptEngine = asCreateScriptEngine( ANGELSCRIPT_VERSION );
	check( asScriptEngine );

	// The script compiler will send any compiler messages to the callback
	asScriptEngine->SetMessageCallback( asFUNCTION( ASMessageCallback ), 0, asCALL_CDECL );

	// Register std strings in scripts
	RegisterStdString( asScriptEngine );
}