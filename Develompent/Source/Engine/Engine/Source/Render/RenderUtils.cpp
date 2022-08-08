#include "Math/Box.h"
#include "Misc/EngineGlobals.h"
#include "Render/VertexFactory/SimpleElementVertexFactory.h"
#include "Render/RenderUtils.h"
#include "Render/DynamicMeshBuilder.h"
#include "Render/Scene.h"
#include "RHI/BaseRHI.h"

/* Maps members of EPixelFormat to a SPixelFormatInfo describing the format */
SPixelFormatInfo		GPixelFormats[ PF_Max ] =
{
	// Name						BlockSizeX	BlockSizeY	BlockSizeZ	BlockBytes	NumComponents	PlatformFormat	Flags			Supported		EngineFormat

	{ TEXT( "Unknown" ),				0,			0,			0,			0,			0,				0,				0,				0,				PF_Unknown					},
	{ TEXT( "A8R8G8B8" ),				1,			1,			1,			4,			4,				0,				0,				1,				PF_A8R8G8B8					},
	{ TEXT( "DepthStencil" ),			1,			1,			1,			0,			1,				0,				0,				0,				PF_DepthStencil				},
	{ TEXT( "ShadowDepth" ),			1,			1,			1,			4,			1,				0,				0,				0,				PF_ShadowDepth				},
	{ TEXT( "FilteredShadowDepth" ),	1,			1,			1,			4,			1,				0,				0,				0,				PF_FilteredShadowDepth		},
	{ TEXT( "D32" ),					1,			1,			1,			4,			1,				0,				0,				1,				PF_D32						}
};

/** Offset to center of the pixel */
float			GPixelCenterOffset		= 0.5f;

void DrawDenormalizedQuad( class CBaseDeviceContextRHI* InDeviceContextRHI, float InX, float InY, float InSizeX, float InSizeY, float InU, float InV, float InSizeU, float InSizeV, uint32 InTargetSizeX, uint32 InTargetSizeY, uint32 InTextureSizeX, uint32 InTextureSizeY, float InClipSpaceQuadZ )
{
	// Set up the vertices
	SSimpleElementVertexType		vertices[ 4 ];

	vertices[ 0 ].position		= Vector4D( InX,			InY,			InClipSpaceQuadZ, 1.f );
	vertices[ 1 ].position		= Vector4D( InX + InSizeX,	InY,			InClipSpaceQuadZ, 1.f );
	vertices[ 2 ].position		= Vector4D( InX,			InY + InSizeY,	InClipSpaceQuadZ, 1.f );
	vertices[ 3 ].position		= Vector4D( InX + InSizeX,	InY + InSizeY,	InClipSpaceQuadZ, 1.f );

	vertices[ 0 ].texCoord		= Vector2D( InU,			InV );
	vertices[ 1 ].texCoord		= Vector2D( InU + InSizeU,	InV );
	vertices[ 2 ].texCoord		= Vector2D( InU,			InV + InSizeV );
	vertices[ 3 ].texCoord		= Vector2D( InU + InSizeU,	InV + InSizeV );

	vertices[ 0 ].color			= CColor( 255, 255, 255 );
	vertices[ 1 ].color			= CColor( 255, 255, 255 );
	vertices[ 2 ].color			= CColor( 255, 255, 255 );
	vertices[ 3 ].color			= CColor( 255, 255, 255 );

	for ( uint32 vertexIndex = 0; vertexIndex < 4; ++vertexIndex )
	{
		vertices[ vertexIndex ].position.x		= -1.0f + 2.0f * ( vertices[ vertexIndex ].position.x - GPixelCenterOffset ) / ( float )InTargetSizeX;
		vertices[ vertexIndex ].position.y		= +1.0f - 2.0f * ( vertices[ vertexIndex ].position.y - GPixelCenterOffset ) / ( float )InTargetSizeY;

		vertices[ vertexIndex ].texCoord.x		= vertices[ vertexIndex ].texCoord.x / ( float )InTextureSizeX;
		vertices[ vertexIndex ].texCoord.y		= vertices[ vertexIndex ].texCoord.y / ( float )InTextureSizeY;
	}

	static uint32 indices[] = { 0, 1, 3, 0, 3, 2 };

	// Draw quad
	GRHI->DrawIndexedPrimitiveUP( InDeviceContextRHI, PT_TriangleList, 0, 2, 4, indices, sizeof( indices[ 0 ] ), vertices, sizeof( vertices[ 0 ] ) );
}

void DrawWireframeBox( struct SSceneDepthGroup& InSDG, const class ÑBox& InBox, const class CColor& InColor )
{
#if WITH_EDITOR
	Vector		bbox[ 2 ] = { InBox.GetMin(), InBox.GetMax() };
	Vector		start, end;

	for ( uint32 i = 0; i < 2; i++ )
	{
		for ( uint32 j = 0; j < 2; j++ )
		{
			start.x = bbox[ i ].z;		end.x = bbox[ i ].x;
			start.y = bbox[ j ].y;		end.y = bbox[ j ].y;
			start.z = bbox[ 0 ].z;		end.z = bbox[ 1 ].z;
			InSDG.simpleElements.AddLine( start, end, InColor );

			start.y = bbox[ i ].y; end.y = bbox[ i ].y;
			start.z = bbox[ j ].z; end.z = bbox[ j ].z;
			start.x = bbox[ 0 ].x; end.x = bbox[ 1 ].x;
			InSDG.simpleElements.AddLine( start, end, InColor );

			start.z = bbox[ i ].z;		end.z = bbox[ i ].z;
			start.x = bbox[ j ].x;		end.x = bbox[ j ].x;
			start.y = bbox[ 0 ].y;		end.y = bbox[ 1 ].y;
			InSDG.simpleElements.AddLine( start, end, InColor );
		}
	}
#endif // WITH_EDITOR
}

void DrawCircle( struct SSceneDepthGroup& InSDG, const Vector& InLocation, const Vector& InX, const Vector& InY, const class CColor& InColor, float InRadius, uint32 InNumSides, float InThickness /*= 0.f*/ )
{
#if WITH_EDITOR
	const float		angleDelta = 2.f * PI / InNumSides;
	Vector			lastVertex = InLocation + InX * InRadius;

	for ( uint32 sideIndex = 0; sideIndex < InNumSides; ++sideIndex )
	{
		const Vector	vertex = InLocation + ( InX * SMath::Cos( angleDelta * ( sideIndex + 1 ) ) + InY * SMath::Sin( angleDelta * ( sideIndex + 1 ) ) ) * InRadius;
		InSDG.simpleElements.AddLine( lastVertex, vertex, InColor, InThickness );
		lastVertex = vertex;
	}
#endif // WITH_EDITOR
}

#if ENABLE_HITPROXY
void DrawHitProxyCircle( struct SSceneDepthGroup& InSDG, EHitProxyLayer InHitProxyLayer, const Vector& InLocation, const Vector& InX, const Vector& InY, const CHitProxyId& InHitProxyId, float InRadius, uint32 InNumSides, float InThickness /*= 0.f*/ )
{
#if WITH_EDITOR
	const float		angleDelta = 2.f * PI / InNumSides;
	Vector			lastVertex = InLocation + InX * InRadius;

	for ( uint32 sideIndex = 0; sideIndex < InNumSides; ++sideIndex )
	{
		const Vector	vertex = InLocation + ( InX * SMath::Cos( angleDelta * ( sideIndex + 1 ) ) + InY * SMath::Sin( angleDelta * ( sideIndex + 1 ) ) ) * InRadius;
		InSDG.hitProxyLayers[ InHitProxyLayer ].simpleHitProxyElements.AddLine( lastVertex, vertex, InHitProxyId.GetColor(), InThickness );
		lastVertex = vertex;
	}
#endif // WITH_EDITOR
}
#endif // ENABLE_HITPROXY