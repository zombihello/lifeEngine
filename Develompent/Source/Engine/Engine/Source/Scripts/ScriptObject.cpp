#include <angelscript.h>

#include "Misc/EngineGlobals.h"
#include "Containers/StringConv.h"
#include "Scripts/ScriptEngine.h"
#include "Scripts/ScriptObject.h"

/**
 * Contructor
 */
//ScriptObject::ScriptObject( uint32 InObjectTypeIndex, const tchar* InModuleName )
//{
//	asIScriptEngine* engine = GScriptEngine->GetASScriptEngine();
//	asIScriptModule* module = engine->GetModule( TCHAR_TO_ANSI( InModuleName ) );
//	check( module );
//
//	typeInfo = module->GetObjectTypeByIndex( InObjectTypeIndex );
//	check( typeInfo );
//
//	self = ( asIScriptObject* )engine->CreateScriptObject( module->GetObjectTypeByIndex( InObjectTypeIndex ) );
//	check( self );
//}

/**
 * Constructor
 */
//ScriptObject::ScriptObject( asIScriptObject* InScriptObject ) :
//	self( InScriptObject ),
//	typeInfo( InScriptObject->GetObjectType() )
//{
//	check( self && typeInfo );
//}

/**
 * Destructor
 */
ScriptObject::~ScriptObject()
{
	check( self && typeInfo );
	GScriptEngine->GetASScriptEngine()->ReleaseScriptObject( self, typeInfo );
}

/**
 * Initialize script object
 */
void ScriptObject::Init( class asIScriptObject* InScriptObject )
{
	check( InScriptObject );

	self = InScriptObject;
	typeInfo = InScriptObject->GetObjectType();
}