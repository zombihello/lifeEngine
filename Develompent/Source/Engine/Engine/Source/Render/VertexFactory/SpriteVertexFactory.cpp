#include "LEBuild.h"
#include "Misc/Template.h"
#include "Render/VertexFactory/SpriteVertexFactory.h"
#include "Render/Scene.h"

IMPLEMENT_VERTEX_FACTORY_TYPE( CSpriteVertexFactory, TEXT( "SpriteVertexFactory.hlsl" ), true, CSpriteVertexFactory::SSS_Instance )

// 
// GLOBALS
//
TGlobalResource< CSpriteVertexDeclaration >			GSpriteVertexDeclaration;

/**
 * Struct of instance buffer for sprite
 */
struct FSpriteInstanceBuffer
{
	Matrix		instanceLocalToWorld;		/**< Local to World matrix for each instance */
};

void CSpriteVertexDeclaration::InitRHI()
{
	VertexDeclarationElementList_t		vertexDeclElementList =
	{
		SVertexElement( CSpriteVertexFactory::SSS_Main,		sizeof( SSpriteVertexType ),		STRUCT_OFFSET( SSpriteVertexType, position ),							VET_Float4, VEU_Position,			0 ),
		SVertexElement( CSpriteVertexFactory::SSS_Main,		sizeof( SSpriteVertexType ),		STRUCT_OFFSET( SSpriteVertexType, texCoord ),							VET_Float2, VEU_TextureCoordinate,	0 ),
		SVertexElement( CSpriteVertexFactory::SSS_Main,		sizeof( SSpriteVertexType ),		STRUCT_OFFSET( SSpriteVertexType, normal ),								VET_Float4, VEU_Normal,				0 ),

#if USE_INSTANCING
		SVertexElement( CSpriteVertexFactory::SSS_Instance,	sizeof( FSpriteInstanceBuffer ),	STRUCT_OFFSET( FSpriteInstanceBuffer, instanceLocalToWorld ),			VET_Float4, VEU_Position,			1, true ),
		SVertexElement( CSpriteVertexFactory::SSS_Instance,	sizeof( FSpriteInstanceBuffer ),	STRUCT_OFFSET( FSpriteInstanceBuffer, instanceLocalToWorld ) + 16,		VET_Float4, VEU_Position,			2, true ),
		SVertexElement( CSpriteVertexFactory::SSS_Instance,	sizeof( FSpriteInstanceBuffer ),	STRUCT_OFFSET( FSpriteInstanceBuffer, instanceLocalToWorld ) + 32,		VET_Float4, VEU_Position,			3, true ),
		SVertexElement( CSpriteVertexFactory::SSS_Instance,	sizeof( FSpriteInstanceBuffer ),	STRUCT_OFFSET( FSpriteInstanceBuffer, instanceLocalToWorld ) + 48,		VET_Float4, VEU_Position,			4, true ),
#endif // USE_INSTANCING
	};
	vertexDeclarationRHI = GRHI->CreateVertexDeclaration( vertexDeclElementList );
}

void CSpriteVertexDeclaration::ReleaseRHI()
{
	vertexDeclarationRHI.SafeRelease();
}

CSpriteVertexShaderParameters::CSpriteVertexShaderParameters()
	: CGeneralVertexShaderParameters( CSpriteVertexFactory::staticType.SupportsInstancing() )
{}

void CSpriteVertexShaderParameters::Bind( const class CShaderParameterMap& InParameterMap )
{
	CGeneralVertexShaderParameters::Bind( InParameterMap );
	flipVerticalParameter.Bind( InParameterMap, TEXT( "bFlipVertical" ), true );
	flipHorizontalParameter.Bind( InParameterMap, TEXT( "bFlipHorizontal" ), true );
	textureRectParameter.Bind( InParameterMap, TEXT( "textureRect" ), true );
	spriteSizeParameter.Bind( InParameterMap, TEXT( "spriteSize" ) );
}

void CSpriteVertexShaderParameters::Set( class CBaseDeviceContextRHI* InDeviceContextRHI, const class CVertexFactory* InVertexFactory ) const
{
	CGeneralVertexShaderParameters::Set( InDeviceContextRHI, InVertexFactory );
	CSpriteVertexFactory*		vertexFactory = ( CSpriteVertexFactory* )InVertexFactory;
	check( InVertexFactory );

	SetVertexShaderValue( InDeviceContextRHI, flipVerticalParameter, vertexFactory->IsFlipedVertical() );
	SetVertexShaderValue( InDeviceContextRHI, flipHorizontalParameter, vertexFactory->IsFlipedHorizontal() );
	SetVertexShaderValue( InDeviceContextRHI, textureRectParameter, vertexFactory->GetTextureRect() );
	SetVertexShaderValue( InDeviceContextRHI, spriteSizeParameter, vertexFactory->GetSpriteSize() );
}

CSpriteVertexFactory::CSpriteVertexFactory()
	: bFlipVertical( false )
	, bFlipHorizontal( false )
	, textureRect( 0.f, 0.f, 1.f, 1.f )
	, spriteSize( 1.f, 1.f )
{}

uint64 CSpriteVertexFactory::GetTypeHash() const
{
	uint64		hash = appMemFastHash( bFlipVertical, staticType.GetHash() );
	hash = appMemFastHash( bFlipHorizontal, hash );
	hash = appMemFastHash( textureRect, hash );
	return appMemFastHash( spriteSize, hash );
}

void CSpriteVertexFactory::SetupInstancing( class CBaseDeviceContextRHI* InDeviceContextRHI, const struct SMeshBatch& InMesh, const class CSceneView* InView, uint32 InNumInstances /* = 1 */, uint32 InStartInstanceID /* = 0 */ ) const
{
	check( InStartInstanceID < InMesh.transformationMatrices.size() && InNumInstances <= InMesh.transformationMatrices.size() - InStartInstanceID );
	GRHI->SetupInstancing( InDeviceContextRHI, SSS_Instance, &InMesh.transformationMatrices[ InStartInstanceID ], sizeof( FSpriteInstanceBuffer ), InNumInstances * sizeof( FSpriteInstanceBuffer ), InNumInstances );
}

void CSpriteVertexFactory::InitRHI()
{
	InitDeclaration( GSpriteVertexDeclaration.GetVertexDeclarationRHI() );
}

CVertexFactoryShaderParameters* CSpriteVertexFactory::ConstructShaderParameters( EShaderFrequency InShaderFrequency )
{
	return InShaderFrequency == SF_Vertex ? new CSpriteVertexShaderParameters() : nullptr;
}
