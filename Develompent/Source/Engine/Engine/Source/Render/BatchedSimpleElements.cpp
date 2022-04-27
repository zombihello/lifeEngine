#include "Misc/EngineGlobals.h"
#include "Render/SceneUtils.h"
#include "Render/BatchedSimpleElements.h"
#include "Render/VertexFactory/SimpleElementVertexFactory.h"
#include "Render/Shaders/SimpleElementShader.h"

void FBatchedSimpleElements::Draw( class FBaseDeviceContextRHI* InDeviceContext, const FSceneView& InSceneView ) const
{
	FSimpleElementVertexShader*			vertexShader	= GShaderManager->FindInstance< FSimpleElementVertexShader, FSimpleElementVertexFactory >();
	FSimpleElementPixelShader*			pixelShader		= GShaderManager->FindInstance< FSimpleElementPixelShader, FSimpleElementVertexFactory >();

	GRHI->SetBoundShaderState( InDeviceContext, GRHI->CreateBoundShaderState( TEXT( "SimpleElementBoundShaderState" ), GSimpleElementVertexDeclaration.GetVertexDeclarationRHI(), vertexShader->GetVertexShader(), pixelShader->GetPixelShader() ) );
	
	// Draw lines
	if ( !lineVerteces.empty() )
	{
		SCOPED_DRAW_EVENT( EventSimpleElements, DEC_SIMPLEELEMENTS, TEXT( "Lines" ) );
		GRHI->DrawPrimitiveUP( InDeviceContext, PT_LineList, 0, lineVerteces.size() / 2, lineVerteces.data(), sizeof( FSimpleElementVertexType ) );
	}
}