#include <angelscript.h>

#include "Misc/EngineGlobals.h"
#include "Containers/StringConv.h"
#include "Scripts/ScriptEngine.h"
#include "Scripts/ScriptObject.h"

/**
 * Destructor
 */
ScriptObject::~ScriptObject()
{
	check( self && typeInfo );

	self->SetUserData( nullptr );
	self->Release();
}

/**
 * Initialize script object
 */
void ScriptObject::Init( class asIScriptObject* InScriptObject )
{
	check( InScriptObject );

	self = InScriptObject;
	typeInfo = InScriptObject->GetObjectType();

	self->AddRef();
	self->SetUserData( this );
}