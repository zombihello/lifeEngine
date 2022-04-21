#include "LEBuild.h"
#include "Misc/Template.h"
#include "Render/VertexFactory/SpriteVertexFactory.h"
#include "Render/Scene.h"

IMPLEMENT_VERTEX_FACTORY_TYPE( FSpriteVertexFactory, TEXT( "SpriteVertexFactory.hlsl" ), true, FSpriteVertexFactory::SSS_Instance )

/**
 * Struct of instance buffer for sprite
 */
struct FSpriteInstanceBuffer
{
	FMatrix		instanceLocalToWorld;		/**< Local to World matrix for each instance */
};

FSpriteVertexShaderParameters::FSpriteVertexShaderParameters()
	: FGeneralVertexShaderParameters( FSpriteVertexFactory::staticType.SupportsInstancing() )
{}

void FSpriteVertexShaderParameters::Bind( const class FShaderParameterMap& InParameterMap )
{
	FGeneralVertexShaderParameters::Bind( InParameterMap );
	flipVerticalParameter.Bind( InParameterMap, TEXT( "bFlipVertical" ) );
	flipHorizontalParameter.Bind( InParameterMap, TEXT( "bFlipHorizontal" ) );
	textureRectParameter.Bind( InParameterMap, TEXT( "textureRect" ) );
	spriteSizeParameter.Bind( InParameterMap, TEXT( "spriteSize" ) );
}

void FSpriteVertexShaderParameters::Set( class FBaseDeviceContextRHI* InDeviceContextRHI, const class FVertexFactory* InVertexFactory ) const
{
	FGeneralVertexShaderParameters::Set( InDeviceContextRHI, InVertexFactory );
	FSpriteVertexFactory*		vertexFactory = ( FSpriteVertexFactory* )InVertexFactory;
	check( InVertexFactory );

	SetVertexShaderValue( InDeviceContextRHI, flipVerticalParameter, vertexFactory->IsFlipedVertical() );
	SetVertexShaderValue( InDeviceContextRHI, flipHorizontalParameter, vertexFactory->IsFlipedHorizontal() );
	SetVertexShaderValue( InDeviceContextRHI, textureRectParameter, vertexFactory->GetTextureRect() );
	SetVertexShaderValue( InDeviceContextRHI, spriteSizeParameter, vertexFactory->GetSpriteSize() );
}

FSpriteVertexFactory::FSpriteVertexFactory()
	: bFlipVertical( false )
	, bFlipHorizontal( false )
	, textureRect( 0.f, 0.f, 1.f, 1.f )
	, spriteSize( 1.f, 1.f )
{}

uint64 FSpriteVertexFactory::GetTypeHash() const
{
	uint64		hash = appMemFastHash( bFlipVertical, staticType.GetHash() );
	hash = appMemFastHash( bFlipHorizontal, hash );
	hash = appMemFastHash( textureRect, hash );
	return appMemFastHash( spriteSize, hash );
}

void FSpriteVertexFactory::SetupInstancing( class FBaseDeviceContextRHI* InDeviceContextRHI, const struct FMeshBatch& InMesh, const class FSceneView* InView, uint32 InNumInstances /* = 1 */, uint32 InStartInstanceID /* = 0 */ ) const
{
	check( InStartInstanceID < InMesh.transformationMatrices.size() && InNumInstances <= InMesh.transformationMatrices.size() - InStartInstanceID );
	GRHI->SetupInstancing( InDeviceContextRHI, SSS_Instance, &InMesh.transformationMatrices[ InStartInstanceID ], sizeof( FSpriteInstanceBuffer ), InNumInstances * sizeof( FSpriteInstanceBuffer ), InNumInstances );
}

void FSpriteVertexFactory::InitRHI()
{
	FVertexDeclarationElementList		vertexDeclElementList =
	{
		FVertexElement( SSS_Main,		sizeof( FSpriteVertexType ),		STRUCT_OFFSET( FSpriteVertexType, position ),							VET_Float4, VEU_Position,			0 ),
		FVertexElement( SSS_Main,		sizeof( FSpriteVertexType ),		STRUCT_OFFSET( FSpriteVertexType, texCoord ),							VET_Float2, VEU_TextureCoordinate,	0 ),
		FVertexElement( SSS_Main,		sizeof( FSpriteVertexType ),		STRUCT_OFFSET( FSpriteVertexType, normal ),								VET_Float4, VEU_Normal,				0 ),

#if USE_INSTANCING
		FVertexElement( SSS_Instance,	sizeof( FSpriteInstanceBuffer ),	STRUCT_OFFSET( FSpriteInstanceBuffer, instanceLocalToWorld ),			VET_Float4, VEU_Position,			1, true ),
		FVertexElement( SSS_Instance,	sizeof( FSpriteInstanceBuffer ),	STRUCT_OFFSET( FSpriteInstanceBuffer, instanceLocalToWorld ) + 16,		VET_Float4, VEU_Position,			2, true ),
		FVertexElement( SSS_Instance,	sizeof( FSpriteInstanceBuffer ),	STRUCT_OFFSET( FSpriteInstanceBuffer, instanceLocalToWorld ) + 32,		VET_Float4, VEU_Position,			3, true ),
		FVertexElement( SSS_Instance,	sizeof( FSpriteInstanceBuffer ),	STRUCT_OFFSET( FSpriteInstanceBuffer, instanceLocalToWorld ) + 48,		VET_Float4, VEU_Position,			4, true ),
#endif // USE_INSTANCING
	};

	InitDeclaration( vertexDeclElementList );
}

FVertexFactoryShaderParameters* FSpriteVertexFactory::ConstructShaderParameters( EShaderFrequency InShaderFrequency )
{
	return InShaderFrequency == SF_Vertex ? new FSpriteVertexShaderParameters() : nullptr;
}
