#include "Misc/Template.h"
#include "Render/VertexFactory/StaticMeshVertexFactory.h"
#include "Render/VertexFactory/GeneralVertexFactoryParams.h"

IMPLEMENT_VERTEX_FACTORY_TYPE( FStaticMeshVertexFactory, TEXT( "StaticMeshVertexFactory.hlsl" ), false, 0 )

//
// GLOBALS
//
TGlobalResource< FStaticMeshVertexDeclaration >			GStaticMeshVertexDeclaration;

void FStaticMeshVertexDeclaration::InitRHI()
{
	FVertexDeclarationElementList		vertexDeclElementList =
	{
		FVertexElement( FStaticMeshVertexFactory::SSS_Main, sizeof( FStaticMeshVertexType ), STRUCT_OFFSET( FStaticMeshVertexType, position ),    VET_Float4, VEU_Position, 0 ),
		FVertexElement( FStaticMeshVertexFactory::SSS_Main, sizeof( FStaticMeshVertexType ), STRUCT_OFFSET( FStaticMeshVertexType, texCoord ),    VET_Float2, VEU_TextureCoordinate, 0 ),
		FVertexElement( FStaticMeshVertexFactory::SSS_Main, sizeof( FStaticMeshVertexType ), STRUCT_OFFSET( FStaticMeshVertexType, normal ),      VET_Float4, VEU_Normal, 0 ),
		FVertexElement( FStaticMeshVertexFactory::SSS_Main, sizeof( FStaticMeshVertexType ), STRUCT_OFFSET( FStaticMeshVertexType, tangent ),     VET_Float4, VEU_Tangent, 0 ),
		FVertexElement( FStaticMeshVertexFactory::SSS_Main, sizeof( FStaticMeshVertexType ), STRUCT_OFFSET( FStaticMeshVertexType, binormal ),    VET_Float4, VEU_Binormal, 0 )
	};
	vertexDeclarationRHI = GRHI->CreateVertexDeclaration( vertexDeclElementList );
}

void FStaticMeshVertexDeclaration::ReleaseRHI()
{
    vertexDeclarationRHI.SafeRelease();
}

void FStaticMeshVertexFactory::InitRHI()
{
	InitDeclaration( GStaticMeshVertexDeclaration.GetVertexDeclarationRHI() );
}

uint64 FStaticMeshVertexFactory::GetTypeHash() const
{
    return staticType.GetHash();
}

FVertexFactoryShaderParameters* FStaticMeshVertexFactory::ConstructShaderParameters( EShaderFrequency InShaderFrequency )
{
    return InShaderFrequency == SF_Vertex ? new FGeneralVertexShaderParameters( staticType.SupportsInstancing() ) : nullptr;
}
