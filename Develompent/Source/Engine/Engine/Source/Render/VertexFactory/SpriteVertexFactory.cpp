#include "LEBuild.h"
#include "Misc/Template.h"
#include "Render/VertexFactory/SpriteVertexFactory.h"
#include "Render/Scene.h"

#if WITH_EDITOR
#include "Misc/WorldEdGlobals.h"
#include "System/EditorEngine.h"
#endif // WITH_EDITOR

IMPLEMENT_VERTEX_FACTORY_TYPE( CSpriteVertexFactory, TEXT( "SpriteVertexFactory.hlsl" ), true, CSpriteVertexFactory::SSS_Instance )

// 
// GLOBALS
//
TGlobalResource< CSpriteVertexDeclaration >			g_SpriteVertexDeclaration;

/**
 * @ingroup Engine
 * @brief Struct of instance buffer for sprite
 */
struct SpriteInstanceBuffer
{
	Matrix		instanceLocalToWorld;		/**< Local to World matrix for each instance */

#if ENABLE_HITPROXY
	CColor		hitProxyId;					/**< Hit proxy id */
#endif // ENABLE_HITPROXY

#if WITH_EDITOR
	CColor		colorOverlay;				/**< Color overlay */
#endif // WITH_EDITOR
};


/*
==================
CSpriteVertexDeclaration::InitRHI
==================
*/
void CSpriteVertexDeclaration::InitRHI()
{
	VertexDeclarationElementList_t		vertexDeclElementList =
	{
		VertexElement( CSpriteVertexFactory::SSS_Main,		sizeof( SpriteVertexType ),		STRUCT_OFFSET( SpriteVertexType, position ),							VET_Float4, VEU_Position,			0 ),
		VertexElement( CSpriteVertexFactory::SSS_Main,		sizeof( SpriteVertexType ),		STRUCT_OFFSET( SpriteVertexType, texCoord ),							VET_Float2, VEU_TextureCoordinate,	0 ),
		VertexElement( CSpriteVertexFactory::SSS_Main,		sizeof( SpriteVertexType ),		STRUCT_OFFSET( SpriteVertexType, normal ),								VET_Float4, VEU_Normal,				0 ),

#if USE_INSTANCING
		VertexElement( CSpriteVertexFactory::SSS_Instance,	sizeof( SpriteInstanceBuffer ),	STRUCT_OFFSET( SpriteInstanceBuffer, instanceLocalToWorld ),			VET_Float4, VEU_Position,			1, true ),
		VertexElement( CSpriteVertexFactory::SSS_Instance,	sizeof( SpriteInstanceBuffer ),	STRUCT_OFFSET( SpriteInstanceBuffer, instanceLocalToWorld ) + 16,		VET_Float4, VEU_Position,			2, true ),
		VertexElement( CSpriteVertexFactory::SSS_Instance,	sizeof( SpriteInstanceBuffer ),	STRUCT_OFFSET( SpriteInstanceBuffer, instanceLocalToWorld ) + 32,		VET_Float4, VEU_Position,			3, true ),
		VertexElement( CSpriteVertexFactory::SSS_Instance,	sizeof( SpriteInstanceBuffer ),	STRUCT_OFFSET( SpriteInstanceBuffer, instanceLocalToWorld ) + 48,		VET_Float4, VEU_Position,			4, true ),
		
#if ENABLE_HITPROXY
		VertexElement( CSpriteVertexFactory::SSS_Instance,	sizeof( SpriteInstanceBuffer ),	STRUCT_OFFSET( SpriteInstanceBuffer, hitProxyId ),						VET_Color,	VEU_Color,				0, true ),
#endif // ENABLE_HITPROXY

#if WITH_EDITOR
		VertexElement( CSpriteVertexFactory::SSS_Instance,	sizeof( SpriteInstanceBuffer ),	STRUCT_OFFSET( SpriteInstanceBuffer, colorOverlay ),					VET_Color,	VEU_Color,				1, true ),
#endif // WITH_EDITOR
#endif // USE_INSTANCING
	};
	vertexDeclarationRHI = g_RHI->CreateVertexDeclaration( vertexDeclElementList );
}

/*
==================
CSpriteVertexDeclaration::ReleaseRHI
==================
*/
void CSpriteVertexDeclaration::ReleaseRHI()
{
	vertexDeclarationRHI.SafeRelease();
}


/*
==================
CSpriteVertexShaderParameters::CSpriteVertexShaderParameters
==================
*/
CSpriteVertexShaderParameters::CSpriteVertexShaderParameters()
	: CGeneralVertexShaderParameters( CSpriteVertexFactory::staticType.SupportsInstancing() )
{}

/*
==================
CSpriteVertexShaderParameters::Bind
==================
*/
void CSpriteVertexShaderParameters::Bind( const class CShaderParameterMap& InParameterMap )
{
	CGeneralVertexShaderParameters::Bind( InParameterMap );
	flipVerticalParameter.Bind( InParameterMap, TEXT( "bFlipVertical" ), true );
	flipHorizontalParameter.Bind( InParameterMap, TEXT( "bFlipHorizontal" ), true );
	textureRectParameter.Bind( InParameterMap, TEXT( "textureRect" ), true );
	spriteSizeParameter.Bind( InParameterMap, TEXT( "spriteSize" ) );
}

/*
==================
CSpriteVertexShaderParameters::Set
==================
*/
void CSpriteVertexShaderParameters::Set( class CBaseDeviceContextRHI* InDeviceContextRHI, const class CVertexFactory* InVertexFactory ) const
{
	CGeneralVertexShaderParameters::Set( InDeviceContextRHI, InVertexFactory );
	CSpriteVertexFactory*		vertexFactory = ( CSpriteVertexFactory* )InVertexFactory;
	Assert( InVertexFactory );

	SetVertexShaderValue( InDeviceContextRHI, flipVerticalParameter, vertexFactory->IsFlipedVertical() );
	SetVertexShaderValue( InDeviceContextRHI, flipHorizontalParameter, vertexFactory->IsFlipedHorizontal() );
	SetVertexShaderValue( InDeviceContextRHI, textureRectParameter, vertexFactory->GetTextureRect() );
	SetVertexShaderValue( InDeviceContextRHI, spriteSizeParameter, vertexFactory->GetSpriteSize() );
}

/*
==================
CSpriteVertexFactory::CSpriteVertexFactory
==================
*/
CSpriteVertexFactory::CSpriteVertexFactory()
	: bFlipVertical( false )
	, bFlipHorizontal( false )
	, textureRect( 0.f, 0.f, 1.f, 1.f )
	, spriteSize( 1.f, 1.f )
{}

/*
==================
CSpriteVertexFactory::GetTypeHash
==================
*/
uint64 CSpriteVertexFactory::GetTypeHash() const
{
	uint64		hash = FastHash( bFlipVertical, CVertexFactory::GetTypeHash() );
	hash = FastHash( bFlipHorizontal, hash );
	hash = FastHash( textureRect, hash );
	return FastHash( spriteSize, hash );
}

/*
==================
CSpriteVertexFactory::SetupInstancing
==================
*/
void CSpriteVertexFactory::SetupInstancing( class CBaseDeviceContextRHI* InDeviceContextRHI, const struct MeshBatch& InMesh, const class CSceneView* InView, uint32 InNumInstances /* = 1 */, uint32 InStartInstanceID /* = 0 */ ) const
{
	Assert( InStartInstanceID < InMesh.instances.size() && InNumInstances <= InMesh.instances.size() - InStartInstanceID );
	
	std::vector<SpriteInstanceBuffer>		instanceBuffers;
	instanceBuffers.resize( InNumInstances );
	for ( uint32 index = 0; index < InNumInstances; ++index )
	{
		SpriteInstanceBuffer&					instanceBuffer = instanceBuffers[ index ];
		const MeshInstance&					meshInstance = InMesh.instances[ InStartInstanceID + index ];
		instanceBuffer.instanceLocalToWorld		= meshInstance.transformMatrix;

#if ENABLE_HITPROXY
		instanceBuffer.hitProxyId				= meshInstance.hitProxyId.GetColor().ToNormalizedVector4D();
#endif // ENABLE_HITPROXY

#if WITH_EDITOR
		instanceBuffer.colorOverlay				= meshInstance.bSelected ? g_EditorEngine->GetSelectionColor().ToNormalizedVector4D() : Vector4D( 0.f, 0.f, 0.f, 0.f );
#endif // WITH_EDITOR
	}

	g_RHI->SetupInstancing( InDeviceContextRHI, SSS_Instance, instanceBuffers.data(), sizeof( SpriteInstanceBuffer ), InNumInstances * sizeof( SpriteInstanceBuffer ), InNumInstances );
}

/*
==================
CSpriteVertexFactory::InitRHI
==================
*/
void CSpriteVertexFactory::InitRHI()
{
	InitDeclaration( g_SpriteVertexDeclaration.GetVertexDeclarationRHI() );
}

/*
==================
CSpriteVertexFactory::ConstructShaderParameters
==================
*/
CVertexFactoryShaderParameters* CSpriteVertexFactory::ConstructShaderParameters( EShaderFrequency InShaderFrequency )
{
	return InShaderFrequency == SF_Vertex ? new CSpriteVertexShaderParameters() : nullptr;
}
