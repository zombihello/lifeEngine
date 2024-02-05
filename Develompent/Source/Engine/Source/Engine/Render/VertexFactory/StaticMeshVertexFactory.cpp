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

#include "Core/Misc/Template.h"
#include "Engine/Render/VertexFactory/StaticMeshVertexFactory.h"
#include "Engine/Render/VertexFactory/GeneralVertexFactoryParams.h"

IMPLEMENT_VERTEX_FACTORY_TYPE( CStaticMeshVertexFactory, TEXT( "StaticMeshVertexFactory.hlsl" ), false, 0 )

//
// GLOBALS
//
TGlobalResource< CStaticMeshVertexDeclaration >			g_StaticMeshVertexDeclaration;

/*
==================
CStaticMeshVertexDeclaration::InitRHI
==================
*/
void CStaticMeshVertexDeclaration::InitRHI()
{
	VertexDeclarationElementList_t		vertexDeclElementList =
	{
		VertexElement( CStaticMeshVertexFactory::SSS_Main, sizeof( StaticMeshVertexType ), STRUCT_OFFSET( StaticMeshVertexType, position ),    VET_Float4, VEU_Position, 0 ),
		VertexElement( CStaticMeshVertexFactory::SSS_Main, sizeof( StaticMeshVertexType ), STRUCT_OFFSET( StaticMeshVertexType, texCoord ),    VET_Float2, VEU_TextureCoordinate, 0 ),
		VertexElement( CStaticMeshVertexFactory::SSS_Main, sizeof( StaticMeshVertexType ), STRUCT_OFFSET( StaticMeshVertexType, normal ),      VET_Float4, VEU_Normal, 0 ),
		VertexElement( CStaticMeshVertexFactory::SSS_Main, sizeof( StaticMeshVertexType ), STRUCT_OFFSET( StaticMeshVertexType, tangent ),     VET_Float4, VEU_Tangent, 0 ),
		VertexElement( CStaticMeshVertexFactory::SSS_Main, sizeof( StaticMeshVertexType ), STRUCT_OFFSET( StaticMeshVertexType, binormal ),    VET_Float4, VEU_Binormal, 0 )
	};
	vertexDeclarationRHI = g_RHI->CreateVertexDeclaration( vertexDeclElementList );
}

/*
==================
CStaticMeshVertexDeclaration::ReleaseRHI
==================
*/
void CStaticMeshVertexDeclaration::ReleaseRHI()
{
    vertexDeclarationRHI.SafeRelease();
}

/*
==================
CStaticMeshVertexFactory::InitRHI
==================
*/
void CStaticMeshVertexFactory::InitRHI()
{
	InitDeclaration( g_StaticMeshVertexDeclaration.GetVertexDeclarationRHI() );
}

/*
==================
CStaticMeshVertexFactory::ConstructShaderParameters
==================
*/
CVertexFactoryShaderParameters* CStaticMeshVertexFactory::ConstructShaderParameters( EShaderFrequency InShaderFrequency )
{
    return InShaderFrequency == SF_Vertex ? new CGeneralVertexShaderParameters( staticType.SupportsInstancing() ) : nullptr;
}
