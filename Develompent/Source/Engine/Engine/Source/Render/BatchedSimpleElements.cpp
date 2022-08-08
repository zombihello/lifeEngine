#include "Misc/EngineGlobals.h"
#include "Render/SceneUtils.h"
#include "Render/BatchedSimpleElements.h"
#include "Render/VertexFactory/SimpleElementVertexFactory.h"
#include "Render/Shaders/SimpleElementShader.h"
#include "Render/Scene.h"

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

	// Draw thick lines
	if ( !thickLines.empty() )
	{
		SCOPED_DRAW_EVENT( EventSimpleElements, DEC_SIMPLEELEMENTS, TEXT( "Thick Lines" ) );	
		Vector						cameraZ = SMath::NormalizeVector( SMath::InverseMatrix( InSceneView.GetViewMatrix() ) * Vector4D( 0.f, 0.f, -1.f, 0.f ) );
		SSimpleElementVertexType	thickVertices[ 4 ];

		for ( uint32 index = 0, count = thickLines.size(); index < count; ++index )
		{
			const SBatchedThickLines&	thickLine	= thickLines[ index ];
			Vector						rectLength	= SMath::NormalizeVector( thickLine.end - thickLine.start );
			Vector						rectUp		= SMath::NormalizeVector( SMath::CrossVector( rectLength, cameraZ ) );
			rectUp						*= thickLine.thickness * 0.5f;
			
			thickVertices[ 2 ]			= SSimpleElementVertexType{ Vector4D( thickLine.start - rectUp, 1.f ), Vector2D( 0.f, 0.f ), thickLine.color };
			thickVertices[ 3 ]			= SSimpleElementVertexType{ Vector4D( thickLine.start + rectUp, 1.f ), Vector2D( 0.f, 1.f ), thickLine.color };
			thickVertices[ 1 ]			= SSimpleElementVertexType{ Vector4D( thickLine.end + rectUp, 1.f ), Vector2D( 1.f, 1.f ), thickLine.color };
			thickVertices[ 0 ]			= SSimpleElementVertexType{ Vector4D( thickLine.end - rectUp, 1.f ), Vector2D( 1.f, 0.f ), thickLine.color };
			GRHI->DrawPrimitiveUP( InDeviceContext, PT_TriangleStrip, 0, 2, &thickVertices[ 0 ], sizeof( SSimpleElementVertexType ) );
		}
	}
}