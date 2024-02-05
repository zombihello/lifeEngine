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
#include "Engine/Render/VertexFactory/DynamicMeshVertexFactory.h"
#include "Engine/Render/VertexFactory/GeneralVertexFactoryParams.h"

IMPLEMENT_VERTEX_FACTORY_TYPE( CDynamicMeshVertexFactory, TEXT( "DynamicMeshVertexFactory.hlsl" ), false, 0 )

//
// GLOBALS
//
TGlobalResource< CDynamicMeshVertexDeclaration >			g_DynamicMeshVertexDeclaration;

/*
==================
CDynamicMeshVertexDeclaration::InitRHI
==================
*/
void CDynamicMeshVertexDeclaration::InitRHI()
{
	VertexDeclarationElementList_t		vertexDeclElementList =
	{
		VertexElement( CDynamicMeshVertexFactory::SSS_Main, sizeof( DynamicMeshVertexType ), STRUCT_OFFSET( DynamicMeshVertexType, position ),    VET_Float4, VEU_Position, 0 ),
		VertexElement( CDynamicMeshVertexFactory::SSS_Main, sizeof( DynamicMeshVertexType ), STRUCT_OFFSET( DynamicMeshVertexType, texCoord ),    VET_Float2, VEU_TextureCoordinate, 0 ),
		VertexElement( CDynamicMeshVertexFactory::SSS_Main, sizeof( DynamicMeshVertexType ), STRUCT_OFFSET( DynamicMeshVertexType, normal ),      VET_Float4, VEU_Normal, 0 ),
		VertexElement( CDynamicMeshVertexFactory::SSS_Main, sizeof( DynamicMeshVertexType ), STRUCT_OFFSET( DynamicMeshVertexType, tangent ),     VET_Float4, VEU_Tangent, 0 ),
		VertexElement( CDynamicMeshVertexFactory::SSS_Main, sizeof( DynamicMeshVertexType ), STRUCT_OFFSET( DynamicMeshVertexType, binormal ),    VET_Float4, VEU_Binormal, 0 ),
		VertexElement( CDynamicMeshVertexFactory::SSS_Main, sizeof( DynamicMeshVertexType ), STRUCT_OFFSET( DynamicMeshVertexType, color ),		VET_Float4, VEU_Color, 0 )
	};
	vertexDeclarationRHI = g_RHI->CreateVertexDeclaration( vertexDeclElementList );
}

/*
==================
CDynamicMeshVertexDeclaration::ReleaseRHI
==================
*/
void CDynamicMeshVertexDeclaration::ReleaseRHI()
{
    vertexDeclarationRHI.SafeRelease();
}

/*
==================
CDynamicMeshVertexFactory::InitRHI
==================
*/
void CDynamicMeshVertexFactory::InitRHI()
{
	InitDeclaration( g_DynamicMeshVertexDeclaration.GetVertexDeclarationRHI() );
}

/*
==================
CDynamicMeshVertexFactory::ConstructShaderParameters
==================
*/
CVertexFactoryShaderParameters* CDynamicMeshVertexFactory::ConstructShaderParameters( EShaderFrequency InShaderFrequency )
{
    return InShaderFrequency == SF_Vertex ? new CGeneralVertexShaderParameters( staticType.SupportsInstancing() ) : nullptr;
}
