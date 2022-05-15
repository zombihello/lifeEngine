#include "Misc/Template.h"
#include "Render/VertexFactory/DynamicMeshVertexFactory.h"
#include "Render/VertexFactory/GeneralVertexFactoryParams.h"

IMPLEMENT_VERTEX_FACTORY_TYPE( FDynamicMeshVertexFactory, TEXT( "DynamicMeshVertexFactory.hlsl" ), false, 0 )

//
// GLOBALS
//
TGlobalResource< FDynamicMeshVertexDeclaration >			GDynamicMeshVertexDeclaration;

void FDynamicMeshVertexDeclaration::InitRHI()
{
	FVertexDeclarationElementList		vertexDeclElementList =
	{
		FVertexElement( FDynamicMeshVertexFactory::SSS_Main, sizeof( FDynamicMeshVertexType ), STRUCT_OFFSET( FDynamicMeshVertexType, position ),    VET_Float4, VEU_Position, 0 ),
		FVertexElement( FDynamicMeshVertexFactory::SSS_Main, sizeof( FDynamicMeshVertexType ), STRUCT_OFFSET( FDynamicMeshVertexType, texCoord ),    VET_Float2, VEU_TextureCoordinate, 0 ),
		FVertexElement( FDynamicMeshVertexFactory::SSS_Main, sizeof( FDynamicMeshVertexType ), STRUCT_OFFSET( FDynamicMeshVertexType, normal ),      VET_Float4, VEU_Normal, 0 ),
		FVertexElement( FDynamicMeshVertexFactory::SSS_Main, sizeof( FDynamicMeshVertexType ), STRUCT_OFFSET( FDynamicMeshVertexType, tangent ),     VET_Float4, VEU_Tangent, 0 ),
		FVertexElement( FDynamicMeshVertexFactory::SSS_Main, sizeof( FDynamicMeshVertexType ), STRUCT_OFFSET( FDynamicMeshVertexType, binormal ),    VET_Float4, VEU_Binormal, 0 ),
		FVertexElement( FDynamicMeshVertexFactory::SSS_Main, sizeof( FDynamicMeshVertexType ), STRUCT_OFFSET( FDynamicMeshVertexType, color ),		VET_Float4, VEU_Color, 0 )
	};
	vertexDeclarationRHI = GRHI->CreateVertexDeclaration( vertexDeclElementList );
}

void FDynamicMeshVertexDeclaration::ReleaseRHI()
{
    vertexDeclarationRHI.SafeRelease();
}

void FDynamicMeshVertexFactory::InitRHI()
{
	if ( !GDynamicMeshVertexDeclaration.GetVertexDeclarationRHI() )
	{
		GDynamicMeshVertexDeclaration.InitRHI();
	}
	InitDeclaration( GDynamicMeshVertexDeclaration.GetVertexDeclarationRHI() );
}

uint64 FDynamicMeshVertexFactory::GetTypeHash() const
{
    return staticType.GetHash();
}

FVertexFactoryShaderParameters* FDynamicMeshVertexFactory::ConstructShaderParameters( EShaderFrequency InShaderFrequency )
{
    return InShaderFrequency == SF_Vertex ? new FGeneralVertexShaderParameters( staticType.SupportsInstancing() ) : nullptr;
}
