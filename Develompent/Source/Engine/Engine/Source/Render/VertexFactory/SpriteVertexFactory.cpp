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
struct SSpriteInstanceBuffer
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
		SVertexElement( CSpriteVertexFactory::SSS_Main,		sizeof( SSpriteVertexType ),		STRUCT_OFFSET( SSpriteVertexType, position ),							VET_Float4, VEU_Position,			0 ),
		SVertexElement( CSpriteVertexFactory::SSS_Main,		sizeof( SSpriteVertexType ),		STRUCT_OFFSET( SSpriteVertexType, texCoord ),							VET_Float2, VEU_TextureCoordinate,	0 ),
		SVertexElement( CSpriteVertexFactory::SSS_Main,		sizeof( SSpriteVertexType ),		STRUCT_OFFSET( SSpriteVertexType, normal ),								VET_Float4, VEU_Normal,				0 ),

#if USE_INSTANCING
		SVertexElement( CSpriteVertexFactory::SSS_Instance,	sizeof( SSpriteInstanceBuffer ),	STRUCT_OFFSET( SSpriteInstanceBuffer, instanceLocalToWorld ),			VET_Float4, VEU_Position,			1, true ),
		SVertexElement( CSpriteVertexFactory::SSS_Instance,	sizeof( SSpriteInstanceBuffer ),	STRUCT_OFFSET( SSpriteInstanceBuffer, instanceLocalToWorld ) + 16,		VET_Float4, VEU_Position,			2, true ),
		SVertexElement( CSpriteVertexFactory::SSS_Instance,	sizeof( SSpriteInstanceBuffer ),	STRUCT_OFFSET( SSpriteInstanceBuffer, instanceLocalToWorld ) + 32,		VET_Float4, VEU_Position,			3, true ),
		SVertexElement( CSpriteVertexFactory::SSS_Instance,	sizeof( SSpriteInstanceBuffer ),	STRUCT_OFFSET( SSpriteInstanceBuffer, instanceLocalToWorld ) + 48,		VET_Float4, VEU_Position,			4, true ),
		
#if ENABLE_HITPROXY
		SVertexElement( CSpriteVertexFactory::SSS_Instance,	sizeof( SSpriteInstanceBuffer ),	STRUCT_OFFSET( SSpriteInstanceBuffer, hitProxyId ),						VET_Color,	VEU_Color,				0, true ),
#endif // ENABLE_HITPROXY

#if WITH_EDITOR
		SVertexElement( CSpriteVertexFactory::SSS_Instance,	sizeof( SSpriteInstanceBuffer ),	STRUCT_OFFSET( SSpriteInstanceBuffer, colorOverlay ),					VET_Color,	VEU_Color,				1, true ),
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
	uint64		hash = Sys_MemFastHash( bFlipVertical, CVertexFactory::GetTypeHash() );
	hash = Sys_MemFastHash( bFlipHorizontal, hash );
	hash = Sys_MemFastHash( textureRect, hash );
	return Sys_MemFastHash( spriteSize, hash );
}

/*
==================
CSpriteVertexFactory::SetupInstancing
==================
*/
void CSpriteVertexFactory::SetupInstancing( class CBaseDeviceContextRHI* InDeviceContextRHI, const struct SMeshBatch& InMesh, const class CSceneView* InView, uint32 InNumInstances /* = 1 */, uint32 InStartInstanceID /* = 0 */ ) const
{
	Assert( InStartInstanceID < InMesh.instances.size() && InNumInstances <= InMesh.instances.size() - InStartInstanceID );
	
	std::vector<SSpriteInstanceBuffer>		instanceBuffers;
	instanceBuffers.resize( InNumInstances );
	for ( uint32 index = 0; index < InNumInstances; ++index )
	{
		SSpriteInstanceBuffer&					instanceBuffer = instanceBuffers[ index ];
		const SMeshInstance&					meshInstance = InMesh.instances[ InStartInstanceID + index ];
		instanceBuffer.instanceLocalToWorld		= meshInstance.transformMatrix;

#if ENABLE_HITPROXY
		instanceBuffer.hitProxyId				= meshInstance.hitProxyId.GetColor().ToNormalizedVector4D();
#endif // ENABLE_HITPROXY

#if WITH_EDITOR
		instanceBuffer.colorOverlay				= meshInstance.bSelected ? g_EditorEngine->GetSelectionColor().ToNormalizedVector4D() : Vector4D( 0.f, 0.f, 0.f, 0.f );
#endif // WITH_EDITOR
	}

	g_RHI->SetupInstancing( InDeviceContextRHI, SSS_Instance, instanceBuffers.data(), sizeof( SSpriteInstanceBuffer ), InNumInstances * sizeof( SSpriteInstanceBuffer ), InNumInstances );
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
