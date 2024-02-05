/**
 * ************************************************************
 *                  This file is part of:
 *                      LIFEENGINE
 *          https://github.com/zombihello/lifeEngine
 * ************************************************************
 * Copyright (C) 2024 Yehor Pohuliaka.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "Engine/Misc/EngineGlobals.h"
#include "Engine/Render/SceneUtils.h"
#include "Engine/Render/BatchedSimpleElements.h"
#include "Engine/Render/VertexFactory/SimpleElementVertexFactory.h"
#include "Engine/Render/Shaders/SimpleElementShader.h"
#include "Engine/Render/Scene.h"

/*
==================
CBatchedSimpleElements::Draw
==================
*/
void CBatchedSimpleElements::Draw( class CBaseDeviceContextRHI* InDeviceContext, const CSceneView& InSceneView ) const
{
	CSimpleElementVertexShader*			vertexShader	= g_ShaderManager->FindInstance< CSimpleElementVertexShader, CSimpleElementVertexFactory >();
	CSimpleElementPixelShader*			pixelShader		= g_ShaderManager->FindInstance< CSimpleElementPixelShader, CSimpleElementVertexFactory >();

	g_RHI->SetBoundShaderState( InDeviceContext, g_RHI->CreateBoundShaderState( TEXT( "SimpleElementBoundShaderState" ), g_SimpleElementVertexDeclaration.GetVertexDeclarationRHI(), vertexShader->GetVertexShader(), pixelShader->GetPixelShader() ) );
	
	// Draw lines
	if ( !lineVerteces.empty() )
	{
		SCOPED_DRAW_EVENT( EventSimpleElements, DEC_SIMPLEELEMENTS, TEXT( "Lines" ) );
		g_RHI->DrawPrimitiveUP( InDeviceContext, PT_LineList, 0, lineVerteces.size() / 2, lineVerteces.data(), sizeof( SimpleElementVertexType ) );
	}

	// Draw thick lines
	if ( !thickLines.empty() )
	{
		SCOPED_DRAW_EVENT( EventSimpleElements, DEC_SIMPLEELEMENTS, TEXT( "Thick Lines" ) );	
		Vector						cameraZ = Math::NormalizeVector( Math::InverseMatrix( InSceneView.GetViewMatrix() ) * Vector4D( 0.f, 0.f, -1.f, 0.f ) );
		SimpleElementVertexType	thickVertices[ 4 ];

		for ( uint32 index = 0, count = thickLines.size(); index < count; ++index )
		{
			const BatchedThickLines&	thickLine	= thickLines[ index ];
			Vector						rectLength	= Math::NormalizeVector( thickLine.end - thickLine.start );
			Vector						rectUp		= Math::NormalizeVector( Math::CrossVector( rectLength, cameraZ ) );
			rectUp						*= thickLine.thickness * 0.5f;
			
			thickVertices[ 2 ]			= SimpleElementVertexType{ Vector4D( thickLine.start - rectUp, 1.f ), Vector2D( 0.f, 0.f ), thickLine.color };
			thickVertices[ 3 ]			= SimpleElementVertexType{ Vector4D( thickLine.start + rectUp, 1.f ), Vector2D( 0.f, 1.f ), thickLine.color };
			thickVertices[ 1 ]			= SimpleElementVertexType{ Vector4D( thickLine.end + rectUp, 1.f ), Vector2D( 1.f, 1.f ), thickLine.color };
			thickVertices[ 0 ]			= SimpleElementVertexType{ Vector4D( thickLine.end - rectUp, 1.f ), Vector2D( 1.f, 0.f ), thickLine.color };
			g_RHI->DrawPrimitiveUP( InDeviceContext, PT_TriangleStrip, 0, 2, &thickVertices[ 0 ], sizeof( SimpleElementVertexType ) );
		}
	}
}