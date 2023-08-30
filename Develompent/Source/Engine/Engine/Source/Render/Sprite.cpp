#include "Render/Sprite.h"
#include "Misc/EngineGlobals.h"
#include "System/BaseEngine.h"

// -------------
// GLOBALS
// -------------
TGlobalResource< CSpriteMesh >				g_SpriteMesh;

/*
==================
CSpriteMesh::InitRHI
==================
*/
void CSpriteMesh::InitRHI()
{
	float	x0 = -1.f,	y0 = -1.f;
	float	x1 = 1.f,	y1 = 1.f;

	SpriteVertexType		verteces[] =
	{
		//			POSITION					TEXCOORD				NORMAL
		{ Vector4D( x0, y0, 0.f, 1.f ), Vector2D( 0.f, 1.f ),		Vector4D( 0.f, 1.f, 0.f, 0.f ) },		// 0
		{ Vector4D( x0, y1, 0.f, 1.f ), Vector2D( 0.f, 0.f ),		Vector4D( 0.f, 1.f, 0.f, 0.f ) },		// 1
		{ Vector4D( x1, y1, 0.f, 1.f ), Vector2D( 1.f, 0.f ),		Vector4D( 0.f, 1.f, 0.f, 0.f ) },		// 2
		{ Vector4D( x1, y0, 0.f, 1.f ), Vector2D( 1.f, 1.f ),		Vector4D( 0.f, 1.f, 0.f, 0.f ) }		// 3
	};
	uint32					indeces[] = { 0, 1, 2, 0, 2, 3 };

	vertexBufferRHI = g_RHI->CreateVertexBuffer( TEXT( "SpriteMesh" ), sizeof( SpriteVertexType ) * ARRAY_COUNT( verteces ), ( byte* ) &verteces[ 0 ], RUF_Static );
	indexBufferRHI = g_RHI->CreateIndexBuffer( TEXT( "SpriteMesh" ), sizeof( uint32 ), sizeof( uint32 ) * ARRAY_COUNT( indeces ), ( byte* ) &indeces[ 0 ], RUF_Static );
}

/*
==================
CSpriteMesh::ReleaseRHI
==================
*/
void CSpriteMesh::ReleaseRHI()
{
	vertexBufferRHI.SafeRelease();
	indexBufferRHI.SafeRelease();
}


/*
==================
CSprite::CSprite
==================
*/
CSprite::CSprite()
	: vertexFactory( new CSpriteVertexFactory() )
	, material( g_Engine->GetDefaultMaterial() )
{}

/*
==================
CSprite::InitRHI
==================
*/
void CSprite::InitRHI()
{
	// Initialize vertex factory
	vertexFactory->AddVertexStream( VertexStream{ GetVertexBufferRHI(), sizeof( SpriteVertexType ) } );		// 0 stream slot
	vertexFactory->Init();
}

/*
==================
CSprite::ReleaseRHI
==================
*/
void CSprite::ReleaseRHI()
{
	vertexFactory->ReleaseResource();
}
