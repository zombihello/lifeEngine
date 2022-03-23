#include "Render/Sprite.h"
#include "Misc/EngineGlobals.h"
#include "System/BaseEngine.h"

// -------------
// GLOBALS
// -------------
TGlobalResource< FSpriteMesh >				GSpriteMesh;

void FSpriteMesh::InitRHI()
{
	float	x0 = 0.f,	y0 = 0.f;
	float	x1 = 1.f,	y1 = 1.f;

	FSpriteVertexType		verteces[] =
	{
		//			POSITION					TEXCOORD				NORMAL
		{ FVector4D( x0, y0, 0.f, 1.f ), FVector2D( 1.f, 1.f ),		FVector4D( x0, y0, 0.f, 0.f ) },		// 0
		{ FVector4D( x0, y1, 0.f, 1.f ), FVector2D( 1.f, 0.f ),		FVector4D( x0, y1, 0.f, 0.f ) },		// 1
		{ FVector4D( x1, y1, 0.f, 1.f ), FVector2D( 0.f, 0.f ),		FVector4D( x1, y1, 0.f, 0.f ) },		// 2
		{ FVector4D( x1, y0, 0.f, 1.f ), FVector2D( 0.f, 1.f ),		FVector4D( x1, y0, 0.f, 0.f ) }			// 3
	};
	uint32					indeces[] = { 2, 1, 0, 2, 0, 3 };

	vertexBufferRHI = GRHI->CreateVertexBuffer( TEXT( "SpriteMesh" ), sizeof( FSpriteVertexType ) * ARRAY_COUNT( verteces ), ( byte* ) &verteces[ 0 ], RUF_Static );
	indexBufferRHI = GRHI->CreateIndexBuffer( TEXT( "SpriteMesh" ), sizeof( uint32 ), sizeof( uint32 ) * ARRAY_COUNT( indeces ), ( byte* ) &indeces[ 0 ], RUF_Static );
}

void FSpriteMesh::ReleaseRHI()
{
	vertexBufferRHI.SafeRelease();
	indexBufferRHI.SafeRelease();
}

FSprite::FSprite()
	: material( GEngine->GetDefaultMaterial() )
{}

void FSprite::InitRHI()
{
	// Initialize vertex factory
	vertexFactory = new FSpriteVertexFactory();
	vertexFactory->AddVertexStream( FVertexStream{ GetVertexBufferRHI(), sizeof( FSpriteVertexType ) } );		// 0 stream slot
	vertexFactory->Init();
}

void FSprite::ReleaseRHI()
{
	vertexFactory.SafeRelease();
}
