#include "Misc/Template.h"
#include "Render/VertexFactory/StaticMeshVertexFactory.h"
#include "Render/VertexFactory/GeneralVertexFactoryParams.h"

IMPLEMENT_VERTEX_FACTORY_TYPE( CStaticMeshVertexFactory, TEXT( "StaticMeshVertexFactory.hlsl" ), false, 0 )

//
// GLOBALS
//
TGlobalResource< CStaticMeshVertexDeclaration >			GStaticMeshVertexDeclaration;

void CStaticMeshVertexDeclaration::InitRHI()
{
	VertexDeclarationElementList_t		vertexDeclElementList =
	{
		SVertexElement( CStaticMeshVertexFactory::SSS_Main, sizeof( SStaticMeshVertexType ), STRUCT_OFFSET( SStaticMeshVertexType, position ),    VET_Float4, VEU_Position, 0 ),
		SVertexElement( CStaticMeshVertexFactory::SSS_Main, sizeof( SStaticMeshVertexType ), STRUCT_OFFSET( SStaticMeshVertexType, texCoord ),    VET_Float2, VEU_TextureCoordinate, 0 ),
		SVertexElement( CStaticMeshVertexFactory::SSS_Main, sizeof( SStaticMeshVertexType ), STRUCT_OFFSET( SStaticMeshVertexType, normal ),      VET_Float4, VEU_Normal, 0 ),
		SVertexElement( CStaticMeshVertexFactory::SSS_Main, sizeof( SStaticMeshVertexType ), STRUCT_OFFSET( SStaticMeshVertexType, tangent ),     VET_Float4, VEU_Tangent, 0 ),
		SVertexElement( CStaticMeshVertexFactory::SSS_Main, sizeof( SStaticMeshVertexType ), STRUCT_OFFSET( SStaticMeshVertexType, binormal ),    VET_Float4, VEU_Binormal, 0 )
	};
	vertexDeclarationRHI = GRHI->CreateVertexDeclaration( vertexDeclElementList );
}

void CStaticMeshVertexDeclaration::ReleaseRHI()
{
    vertexDeclarationRHI.SafeRelease();
}

void CStaticMeshVertexFactory::InitRHI()
{
	InitDeclaration( GStaticMeshVertexDeclaration.GetVertexDeclarationRHI() );
}

uint64 CStaticMeshVertexFactory::GetTypeHash() const
{
    return staticType.GetHash();
}

CVertexFactoryShaderParameters* CStaticMeshVertexFactory::ConstructShaderParameters( EShaderFrequency InShaderFrequency )
{
    return InShaderFrequency == SF_Vertex ? new CGeneralVertexShaderParameters( staticType.SupportsInstancing() ) : nullptr;
}
