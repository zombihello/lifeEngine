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

	vertices[ 0 ].color			= ÑColor( 255, 255, 255 );
	vertices[ 1 ].color			= ÑColor( 255, 255, 255 );
	vertices[ 2 ].color			= ÑColor( 255, 255, 255 );
	vertices[ 3 ].color			= ÑColor( 255, 255, 255 );

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

void DrawWireframeBox( struct SSceneDepthGroup& InSDG, const class ÑBox& InBox, const class ÑColor& InColor )
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

void DrawSphere( struct SSceneDepthGroup& InSDG, const Vector& InCenter, const Vector& InRadius, uint32 InNumSides, uint32 InNumRings, class CMaterial* InMaterial )
{
	CDynamicMeshBuilder		dynamicMeshBuilder;
	Matrix					transofmrationMatrix = SMath::ScaleMatrix( InRadius ) * SMath::TranslateMatrix( InCenter );

	// Use a mesh builder to draw the sphere
	{
		// The first/last arc are on top of each other
		uint32		numVerts = ( InNumSides + 1 ) * ( InNumRings + 1 );
		SDynamicMeshVertexType*		verts = ( SDynamicMeshVertexType* )malloc( numVerts * sizeof( SDynamicMeshVertexType ) );

		// Calculate verts for one arc
		SDynamicMeshVertexType*		arcVerts = ( SDynamicMeshVertexType* )malloc( ( InNumRings + 1 ) * sizeof( SDynamicMeshVertexType ) );

		for ( uint32 index = 0; index < InNumRings + 1; index++ )
		{
			SDynamicMeshVertexType*	arcVert = &arcVerts[ index ];
			float					angle	= ( ( float )index / InNumRings ) * PI;

			// Note: unit sphere, so position always has mag of one. We can just use it for normal!			
			arcVert->position		= Vector4D( 0.f, SMath::Cos( angle ), SMath::Sin( angle ), 1.f );
			arcVert->tangent		= Vector4D( 1.f, 0.f, 0.f, 0.f );
			arcVert->normal			= arcVert->position;
			arcVert->binormal		= Vector4D( 0.f, -arcVert->position.y, arcVert->position.z, 0.f );

			arcVert->texCoord.x		= 0.f;
			arcVert->texCoord.t		= ( float )index / InNumRings;
		}

		// Then rotate this arc InNumSides+1 times
		for ( uint32 indexSide = 0; indexSide < InNumSides + 1; indexSide++ )
		{
			CRotator	arcRotator( 0.f, SMath::Trunc( 65536.f * ( ( float )indexSide / InNumSides ) ), 0.f );
			Matrix		arcRot = arcRotator.ToMatrix();
			float		xTexCoord = ( float )indexSide / InNumSides;

			for ( uint32 indexRing = 0; indexRing < InNumRings + 1; indexRing++ )
			{
				uint32		VIx = ( InNumRings + 1 ) * indexSide + indexRing;

				verts[ VIx ].position	= arcRot * arcVerts[ indexRing ].position;
				verts[ VIx ].tangent	= arcRot * arcVerts[ indexRing ].tangent;
				verts[ VIx ].normal		= arcRot * arcVerts[ indexRing ].normal;
				verts[ VIx ].binormal	= arcRot * arcVerts[ indexRing ].binormal;

				verts[ VIx ].texCoord.x = xTexCoord;
				verts[ VIx ].texCoord.y = arcVerts[ indexRing ].texCoord.y;
			}
		}

		// Add all of the vertices we generated to the mesh builder
		for ( uint32 vertIdx = 0; vertIdx < numVerts; vertIdx++ )
		{
			dynamicMeshBuilder.AddVertex( verts[ vertIdx ] );
		}

		// Add all of the triangles we generated to the mesh builder
		for ( uint32 indexSide = 0; indexSide < InNumSides; indexSide++ )
		{
			uint32	a0start = ( indexSide + 0 ) * ( InNumRings + 1 );
			uint32	a1start = ( indexSide + 1 ) * ( InNumRings + 1 );
			for ( uint32 indexRing = 0; indexRing < InNumRings; indexRing++ )
			{
				dynamicMeshBuilder.AddTriangle( a0start + indexRing + 0, a1start + indexRing + 0, a0start + indexRing + 1 );
				dynamicMeshBuilder.AddTriangle( a1start + indexRing + 0, a1start + indexRing + 1, a0start + indexRing + 1 );
			}
		}

		// Free our local copy of verts and arc verts
		free( verts );
		free( arcVerts );

		// Build mesh for drawing
		dynamicMeshBuilder.Build();
	}
}