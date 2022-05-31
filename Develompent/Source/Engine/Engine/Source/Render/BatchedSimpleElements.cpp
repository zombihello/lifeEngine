#include "Misc/EngineGlobals.h"
#include "Render/SceneUtils.h"
#include "Render/BatchedSimpleElements.h"
#include "Render/VertexFactory/SimpleElementVertexFactory.h"
#include "Render/Shaders/SimpleElementShader.h"

void CBatchedSimpleElements::Draw( class CBaseDeviceContextRHI* InDeviceContext, const CSceneView& InSceneView ) const
{
	CSimpleElementVertexShader*			vertexShader	= GShaderManager->FindInstance< CSimpleElementVertexShader, CSimpleElementVertexFactory >();
	CSimpleElementPixelShader*			pixelShader		= GShaderManager->FindInstance< CSimpleElementPixelShader, CSimpleElementVertexFactory >();

	GRHI->SetBoundShaderState( InDeviceContext, GRHI->CreateBoundShaderState( TEXT( "SimpleElementBoundShaderState" ), GSimpleElementVertexDeclaration.GetVertexDeclarationRHI(), vertexShader->GetVertexShader(), pixelShader->GetPixelShader() ) );
	
	// Draw lines
	if ( !lineVerteces.empty() )
	{
		SCOPED_DRAW_EVENT( EventSimpleElements, DEC_SIMPLEELEMENTS, TEXT( "Lines" ) );
		GRHI->DrawPrimitiveUP( InDeviceContext, PT_LineList, 0, lineVerteces.size() / 2, lineVerteces.data(), sizeof( SSimpleElementVertexType ) );
	}
}