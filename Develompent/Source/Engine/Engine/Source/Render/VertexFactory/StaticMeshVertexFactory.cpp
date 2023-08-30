#include "Misc/Template.h"
#include "Render/VertexFactory/StaticMeshVertexFactory.h"
#include "Render/VertexFactory/GeneralVertexFactoryParams.h"

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
