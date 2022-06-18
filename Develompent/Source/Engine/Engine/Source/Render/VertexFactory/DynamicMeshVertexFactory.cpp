#include "Misc/Template.h"
#include "Render/VertexFactory/DynamicMeshVertexFactory.h"
#include "Render/VertexFactory/GeneralVertexFactoryParams.h"

IMPLEMENT_VERTEX_FACTORY_TYPE( CDynamicMeshVertexFactory, TEXT( "DynamicMeshVertexFactory.hlsl" ), false, 0 )

//
// GLOBALS
//
TGlobalResource< CDynamicMeshVertexDeclaration >			GDynamicMeshVertexDeclaration;

void CDynamicMeshVertexDeclaration::InitRHI()
{
	VertexDeclarationElementList_t		vertexDeclElementList =
	{
		SVertexElement( CDynamicMeshVertexFactory::SSS_Main, sizeof( SDynamicMeshVertexType ), STRUCT_OFFSET( SDynamicMeshVertexType, position ),    VET_Float4, VEU_Position, 0 ),
		SVertexElement( CDynamicMeshVertexFactory::SSS_Main, sizeof( SDynamicMeshVertexType ), STRUCT_OFFSET( SDynamicMeshVertexType, texCoord ),    VET_Float2, VEU_TextureCoordinate, 0 ),
		SVertexElement( CDynamicMeshVertexFactory::SSS_Main, sizeof( SDynamicMeshVertexType ), STRUCT_OFFSET( SDynamicMeshVertexType, normal ),      VET_Float4, VEU_Normal, 0 ),
		SVertexElement( CDynamicMeshVertexFactory::SSS_Main, sizeof( SDynamicMeshVertexType ), STRUCT_OFFSET( SDynamicMeshVertexType, tangent ),     VET_Float4, VEU_Tangent, 0 ),
		SVertexElement( CDynamicMeshVertexFactory::SSS_Main, sizeof( SDynamicMeshVertexType ), STRUCT_OFFSET( SDynamicMeshVertexType, binormal ),    VET_Float4, VEU_Binormal, 0 ),
		SVertexElement( CDynamicMeshVertexFactory::SSS_Main, sizeof( SDynamicMeshVertexType ), STRUCT_OFFSET( SDynamicMeshVertexType, color ),		VET_Float4, VEU_Color, 0 )
	};
	vertexDeclarationRHI = GRHI->CreateVertexDeclaration( vertexDeclElementList );
}

void CDynamicMeshVertexDeclaration::ReleaseRHI()
{
    vertexDeclarationRHI.SafeRelease();
}

void CDynamicMeshVertexFactory::InitRHI()
{
	InitDeclaration( GDynamicMeshVertexDeclaration.GetVertexDeclarationRHI() );
}

uint64 CDynamicMeshVertexFactory::GetTypeHash() const
{
    return staticType.GetHash();
}

CVertexFactoryShaderParameters* CDynamicMeshVertexFactory::ConstructShaderParameters( EShaderFrequency InShaderFrequency )
{
    return InShaderFrequency == SF_Vertex ? new CGeneralVertexShaderParameters( staticType.SupportsInstancing() ) : nullptr;
}
