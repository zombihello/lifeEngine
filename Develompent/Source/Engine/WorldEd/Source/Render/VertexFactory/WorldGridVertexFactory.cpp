#include "Render/VertexFactory/WorldGridVertexFactory.h"

IMPLEMENT_VERTEX_FACTORY_TYPE( FWorldGridVertexFactory, TEXT( "WorldGridVertexFactory.hlsl" ) )

void FWorldGridVertexFactory::InitRHI()
{
	FVertexDeclarationElementList		vertexDeclElementList =
	{
		FVertexElement( SSS_Main, sizeof( FWorldGridVertexType ), offsetof( FWorldGridVertexType, position ), VET_Float4, VEU_Position, 0 ),
	};

	InitDeclaration( vertexDeclElementList );
}