#include "Render/Sprite.h"
#include "Misc/EngineGlobals.h"
#include "System/BaseEngine.h"

// -------------
// GLOBALS
// -------------
TGlobalResource< CSpriteMesh >				GSpriteMesh;

void CSpriteMesh::InitRHI()
{
	float	x0 = 0.f,	y0 = 0.f;
	float	x1 = 1.f,	y1 = 1.f;

	SSpriteVertexType		verteces[] =
	{
		//			POSITION					TEXCOORD				NORMAL
		{ Vector4D( x0, y0, 0.f, 1.f ), Vector2D( 0.f, 1.f ),		Vector4D( x0, y0, 0.f, 0.f ) },		// 0
		{ Vector4D( x0, y1, 0.f, 1.f ), Vector2D( 0.f, 0.f ),		Vector4D( x0, y1, 0.f, 0.f ) },		// 1
		{ Vector4D( x1, y1, 0.f, 1.f ), Vector2D( 1.f, 0.f ),		Vector4D( x1, y1, 0.f, 0.f ) },		// 2
		{ Vector4D( x1, y0, 0.f, 1.f ), Vector2D( 1.f, 1.f ),		Vector4D( x1, y0, 0.f, 0.f ) }			// 3
	};
	uint32					indeces[] = { 0, 1, 2, 0, 2, 3 };

	vertexBufferRHI = GRHI->CreateVertexBuffer( TEXT( "SpriteMesh" ), sizeof( SSpriteVertexType ) * ARRAY_COUNT( verteces ), ( byte* ) &verteces[ 0 ], RUF_Static );
	indexBufferRHI = GRHI->CreateIndexBuffer( TEXT( "SpriteMesh" ), sizeof( uint32 ), sizeof( uint32 ) * ARRAY_COUNT( indeces ), ( byte* ) &indeces[ 0 ], RUF_Static );
}

void CSpriteMesh::ReleaseRHI()
{
	vertexBufferRHI.SafeRelease();
	indexBufferRHI.SafeRelease();
}

CSprite::CSprite()
	: material( GEngine->GetDefaultMaterial() )
{}

void CSprite::InitRHI()
{
	// Initialize vertex factory
	vertexFactory = new CSpriteVertexFactory();
	vertexFactory->AddVertexStream( SVertexStream{ GetVertexBufferRHI(), sizeof( SSpriteVertexType ) } );		// 0 stream slot
	vertexFactory->Init();
}

void CSprite::ReleaseRHI()
{
	vertexFactory.SafeRelease();
}
