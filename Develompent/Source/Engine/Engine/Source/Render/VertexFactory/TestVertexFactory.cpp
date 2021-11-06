#include "Render/VertexFactory/TestVertexFactory.h"
#include "Render/Shaders/TestShader.h"

IMPLEMENT_VERTEX_FACTORY_TYPE( FTestVertexFactory, FTestVertexShader, TEXT( "TestVertexFactory.hlsl" ) )

void FTestVertexFactory::InitRHI()
{
	FVertexDeclarationElementList		vertexDeclElementList;
	vertexDeclElementList.push_back( FVertexElement( SSS_Main, sizeof( FTestVertexType ), offsetof( FTestVertexType, position ), VET_Float3, VEU_Position, 0 ) );
	vertexDeclElementList.push_back( FVertexElement( SSS_Main, sizeof( FTestVertexType ), offsetof( FTestVertexType, texCoord ), VET_Float2, VEU_TextureCoordinate, 0 ) );
	InitDeclaration( vertexDeclElementList );
}