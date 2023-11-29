#include <vector>

#include "Containers/String.h"
#include "System/ScriptFrame.h"

/*
==================
ScriptFrame::ScriptFrame
==================
*/
ScriptFrame::ScriptFrame( CObject* InObject )
	: object( InObject )
	, node( nullptr )
	, bytecode( nullptr )
	, previousFrame( nullptr )
{}

/*
==================
ScriptFrame::ScriptFrame
==================
*/
ScriptFrame::ScriptFrame( CObject* InObject, CFunction* InFunction, ScriptFrame* InPreviousFrame /* = nullptr */ )
	: object( InObject )
	, node( InFunction )
	, bytecode( nullptr )
	, previousFrame( InPreviousFrame )
{
	Assert( node );
	bytecode = ( byte* )node->GetBytecode().data();
}

/*
==================
ScriptFrame::GetStackTrace
==================
*/
std::wstring ScriptFrame::GetStackTrace() const
{
	std::wstring	stackTrace;
	
	// Travel down the stack recording the frames
	std::vector<const ScriptFrame*>		frameStack;
	for ( const ScriptFrame* currentFrame = this; currentFrame; currentFrame = currentFrame->previousFrame )
	{
		if ( currentFrame->node )
		{
			frameStack.push_back( currentFrame );
		}
	}

	// Dump the stack trace to a string
	stackTrace += TEXT( "Script call stack:\n" );
	for ( uint32 index = 0, count = frameStack.size(); index < count; ++index )
	{
		const ScriptFrame*	currentFrame = frameStack[index];
		stackTrace += CString::Format( TEXT( "\t%s (%s)" ), currentFrame->node->GetName().c_str(), currentFrame->object->GetName().c_str() );
		if ( index + 1 < count )
		{
			stackTrace += TEXT( "\n" );
		}
	}

	// Return result
	return stackTrace;
}