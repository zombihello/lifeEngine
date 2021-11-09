#include "Render/VertexFactory/StaticMeshVertexFactory.h"

IMPLEMENT_VERTEX_FACTORY_TYPE( FStaticMeshVertexFactory, TEXT( "StaticMeshVertexFactory.hlsl" ) )

void FStaticMeshVertexFactory::InitRHI()
{
	FVertexDeclarationElementList		vertexDeclElementList =
	{
		FVertexElement( SSS_Main, sizeof( FStaticMeshVertexType ), offsetof( FStaticMeshVertexType, position ), VET_Float4, VEU_Position, 0 ),
		FVertexElement( SSS_Main, sizeof( FStaticMeshVertexType ), offsetof( FStaticMeshVertexType, texCoord ), VET_Float2, VEU_TextureCoordinate, 0 ),
		FVertexElement( SSS_Main, sizeof( FStaticMeshVertexType ), offsetof( FStaticMeshVertexType, normal ), VET_Float4, VEU_Normal, 0 ),
		FVertexElement( SSS_Main, sizeof( FStaticMeshVertexType ), offsetof( FStaticMeshVertexType, tangent ), VET_Float4, VEU_Tangent, 0 ),
		FVertexElement( SSS_Main, sizeof( FStaticMeshVertexType ), offsetof( FStaticMeshVertexType, binormal ), VET_Float4, VEU_Binormal, 0 )
	};

	InitDeclaration( vertexDeclElementList );
}