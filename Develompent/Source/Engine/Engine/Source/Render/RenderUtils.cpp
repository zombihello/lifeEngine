#include "Misc/EngineGlobals.h"
#include "Render/VertexFactory/LocalVertexFactory.h"
#include "Render/RenderUtils.h"
#include "RHI/BaseRHI.h"

/* Maps members of EPixelFormat to a FPixelFormatInfo describing the format */
FPixelFormatInfo		GPixelFormats[ PF_Max ] =
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

void DrawDenormalizedQuad( class FBaseDeviceContextRHI* InDeviceContextRHI, float InX, float InY, float InSizeX, float InSizeY, float InU, float InV, float InSizeU, float InSizeV, uint32 InTargetSizeX, uint32 InTargetSizeY, uint32 InTextureSizeX, uint32 InTextureSizeY, float InClipSpaceQuadZ )
{
	// Set up the vertices
	FLocalVertexType		vertices[ 4 ];

	vertices[ 0 ].position = FVector4D( InX,			InY,			InClipSpaceQuadZ, 1.f );
	vertices[ 1 ].position = FVector4D( InX + InSizeX,	InY,			InClipSpaceQuadZ, 1.f );
	vertices[ 2 ].position = FVector4D( InX,			InY + InSizeY,	InClipSpaceQuadZ, 1.f );
	vertices[ 3 ].position = FVector4D( InX + InSizeX,	InY + InSizeY,	InClipSpaceQuadZ, 1.f );

	vertices[ 0 ].texCoord = FVector2D( InU,			InV );
	vertices[ 1 ].texCoord = FVector2D( InU + InSizeU,	InV );
	vertices[ 2 ].texCoord = FVector2D( InU,			InV + InSizeV );
	vertices[ 3 ].texCoord = FVector2D( InU + InSizeU,	InV + InSizeV );

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