#include "Misc/Class.h"
#include "Render/Viewport.h"
#include "System/EditorEngine.h"

IMPLEMENT_CLASS( LEditorEngine )

LEditorEngine::LEditorEngine()
{}

LEditorEngine::~LEditorEngine()
{}

void LEditorEngine::Init()
{
}

void LEditorEngine::Tick( float InDeltaSeconds )
{
	for ( uint32 index = 0, count = ( uint32 )viewports.size(); index < count; ++index )
	{
		viewports[ index ]->Draw();
	}
}

void LEditorEngine::Shutdown()
{}

void LEditorEngine::ProcessEvent( struct SWindowEvent& InWindowEvent )
{}