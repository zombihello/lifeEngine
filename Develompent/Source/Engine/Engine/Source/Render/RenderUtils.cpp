#include "Math/Box.h"
#include "Misc/EngineGlobals.h"
#include "Render/VertexFactory/SimpleElementVertexFactory.h"
#include "Render/RenderUtils.h"
#include "Render/DynamicMeshBuilder.h"
#include "Render/Scene.h"
#include "RHI/BaseRHI.h"

TGlobalResource<CWhiteTexture>				g_WhiteTexture;
TGlobalResource<CBlackTexture>				g_BlackTexture;
TGlobalResource<CEmptyNormalTexture>		g_EmptyNormalTexture;

/* Maps members of EPixelFormat to a PixelFormatInfo describing the format */
PixelFormatInfo		g_PixelFormats[ PF_Max ] =
{
	// Name						BlockSizeX	BlockSizeY	BlockSizeZ	BlockBytes	NumComponents	PlatformFormat			Flags			Supported		EngineFormat

	{ TEXT( "Unknown" ),				0,			0,			0,			0,			0,				0,				0,				0,				PF_Unknown					},
	{ TEXT( "A8R8G8B8" ),				1,			1,			1,			4,			4,				0,				0,				0,				PF_A8R8G8B8					},
	{ TEXT( "DepthStencil" ),			1,			1,			1,			0,			1,				0,				0,				0,				PF_DepthStencil				},
	{ TEXT( "ShadowDepth" ),			1,			1,			1,			4,			1,				0,				0,				0,				PF_ShadowDepth				},
	{ TEXT( "FilteredShadowDepth" ),	1,			1,			1,			4,			1,				0,				0,				0,				PF_FilteredShadowDepth		},
	{ TEXT( "D32" ),					1,			1,			1,			4,			1,				0,				0,				0,				PF_D32						},
	{ TEXT( "FloatRGB" ),				1,			1,			1,			12,			3,				0,				0,				0,				PF_FloatRGB					},
	{ TEXT( "FloatRGBA" ),				1,			1,			1,			16,			4,				0,				0,				0,				PF_FloatRGBA				},
	{ TEXT( "R32F" ),					1,			1,			1,			4,			1,				0,				0,				0,				PF_R32F						},
	{ TEXT( "BC1" ),					4,			4,			1,			8,			3,				0,				0,				0,				PF_BC1						},
	{ TEXT( "BC2" ),					4,			4,			1,			16,			4,				0,				0,				0,				PF_BC2						},
	{ TEXT( "BC3" ),					4,			4,			1,			16,			4,				0,				0,				0,				PF_BC3						},
	{ TEXT( "BC5" ),					4,			4,			1,			16,			2,				0,				0,				0,				PF_BC5						},
	{ TEXT( "BC6H" ),					1,			1,			1,			16,			3,				0,				0,				0,				PF_BC6H						},
	{ TEXT( "BC7" ),					4,			4,			1,			16,			4,				0,				0,				0,				PF_BC7						}
};

/** Offset to center of the pixel */
float			g_PixelCenterOffset		= 0.5f;

/*
==================
DrawDenormalizedQuad
==================
*/
void DrawDenormalizedQuad( class CBaseDeviceContextRHI* InDeviceContextRHI, float InX, float InY, float InSizeX, float InSizeY, float InU, float InV, float InSizeU, float InSizeV, uint32 InTargetSizeX, uint32 InTargetSizeY, uint32 InTextureSizeX, uint32 InTextureSizeY, float InClipSpaceQuadZ )
{
	// Set up the vertices
	SimpleElementVertexType		vertices[ 4 ];

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
		vertices[ vertexIndex ].position.x		= -1.0f + 2.0f * ( vertices[ vertexIndex ].position.x - g_PixelCenterOffset ) / ( float )InTargetSizeX;
		vertices[ vertexIndex ].position.y		= +1.0f - 2.0f * ( vertices[ vertexIndex ].position.y - g_PixelCenterOffset ) / ( float )InTargetSizeY;

		vertices[ vertexIndex ].texCoord.x		= vertices[ vertexIndex ].texCoord.x / ( float )InTextureSizeX;
		vertices[ vertexIndex ].texCoord.y		= vertices[ vertexIndex ].texCoord.y / ( float )InTextureSizeY;
	}

	static uint32 indices[] = { 0, 1, 3, 0, 3, 2 };

	// Draw quad
	g_RHI->DrawIndexedPrimitiveUP( InDeviceContextRHI, PT_TriangleList, 0, 2, 4, indices, sizeof( indices[ 0 ] ), vertices, sizeof( vertices[ 0 ] ) );
}

/*
==================
DrawWireframeBox
==================
*/
void DrawWireframeBox( struct SceneDepthGroup& InSDG, const class CBox& InBox, const class CColor& InColor )
{
#if WITH_EDITOR
	Vector		bbox[ 2 ] = { InBox.GetMin(), InBox.GetMax() };
	Vector		start, end;

	for ( uint32 i = 0; i < 2; i++ )
	{
		for ( uint32 j = 0; j < 2; j++ )
		{
			start.x = bbox[ i ].x;		end.x = bbox[ i ].x;
			start.y = bbox[ j ].y;		end.y = bbox[ j ].y;
			start.z = bbox[ 0 ].z;		end.z = bbox[ 1 ].z;
			InSDG.simpleElements.AddLine( start, end, InColor );

			start.y = bbox[ i ].y;		end.y = bbox[ i ].y;
			start.z = bbox[ j ].z;		end.z = bbox[ j ].z;
			start.x = bbox[ 0 ].x;		end.x = bbox[ 1 ].x;
			InSDG.simpleElements.AddLine( start, end, InColor );

			start.z = bbox[ i ].z;		end.z = bbox[ i ].z;
			start.x = bbox[ j ].x;		end.x = bbox[ j ].x;
			start.y = bbox[ 0 ].y;		end.y = bbox[ 1 ].y;
			InSDG.simpleElements.AddLine( start, end, InColor );
		}
	}
#endif // WITH_EDITOR
}

/*
==================
DrawCircle
==================
*/
void DrawCircle( struct SceneDepthGroup& InSDG, const Vector& InLocation, const Vector& InX, const Vector& InY, const class CColor& InColor, float InRadius, uint32 InNumSides, float InThickness /*= 0.f*/ )
{
#if WITH_EDITOR
	const float		angleDelta = 2.f * PI / InNumSides;
	Vector			lastVertex = InLocation + InX * InRadius;

	for ( uint32 sideIndex = 0; sideIndex < InNumSides; ++sideIndex )
	{
		const Vector	vertex = InLocation + ( InX * Math::Cos( angleDelta * ( sideIndex + 1 ) ) + InY * Math::Sin( angleDelta * ( sideIndex + 1 ) ) ) * InRadius;
		InSDG.simpleElements.AddLine( lastVertex, vertex, InColor, InThickness );
		lastVertex = vertex;
	}
#endif // WITH_EDITOR
}

/*
==================
DrawWireSphere
==================
*/
void DrawWireSphere( struct SceneDepthGroup& InSDG, const Vector& InBase, CColor InColor, float InRadius, uint32 InNumSides, byte InDepthPriority /*= 0*/ )
{
#if WITH_EDITOR
	DrawCircle( InSDG, InBase, Vector( 1.f, 0.f, 0.f ), Vector( 0.f, 1.f, 0.f ), InColor, InRadius, InNumSides );
	DrawCircle( InSDG, InBase, Vector( 1.f, 0.f, 0.f ), Vector( 0.f, 0.f, 1.f ), InColor, InRadius, InNumSides );
	DrawCircle( InSDG, InBase, Vector( 0.f, 1.f, 0.f ), Vector( 0.f, 0.f, 1.f ), InColor, InRadius, InNumSides );
#endif // WITH_EDITOR
}

/*
==================
DrawWireCone
==================
*/
void DrawWireCone( struct SceneDepthGroup& InSDG, const Matrix& InTransform, float InConeRadius, float InConeHeight, uint32 InConeSides, CColor InColor, std::vector<Vector>& OutVerts, byte InDepthPriority /*= 0*/ )
{
#if WITH_EDITOR
	static const float		twoPI = 2.0f * PI;
	static const float		toRads = PI / 180.0f;
	static const float		maxRadius = 89.0f * toRads + 0.001f;
	const float				clampedConeRadius= Clamp( InConeRadius * toRads, 0.001f, maxRadius );
	const float				sinClampedConeRadius = Math::Sin( clampedConeRadius );
	const float				cosClampedConeRadius = Math::Cos( clampedConeRadius );
	const Vector coneDirection( 0.f, 0.f, 1.f );
	const Vector coneUpVector( 0.f, 1.f, 0.f );
	const Vector coneLeftVector( -1.f, 0.f, 0.f );

	OutVerts.resize( InConeSides );

	// Generate verteces in world space
	for ( uint32 index = 0, count = OutVerts.size(); index < count; ++index )
	{
		const float		theta = static_cast<float>( ( twoPI * index ) / count );
		OutVerts[index] = InTransform * Vector4D( ( coneDirection * ( InConeHeight * cosClampedConeRadius ) ) +
			( ( sinClampedConeRadius * InConeHeight * Math::Cos( theta ) ) * coneUpVector ) +
			( ( sinClampedConeRadius * InConeHeight * Math::Sin( theta ) ) * coneLeftVector ), 1.f );
	}

	// Draw spokes
	for ( uint32 index = 0, count = OutVerts.size(); index < count; ++index )
	{
		InSDG.simpleElements.AddLine( Math::GetOriginMatrix( InTransform ), OutVerts[index], InColor, 0.f );
	}

	// Draw rim
	for ( uint32 index = 0, count = OutVerts.size()-1; index < count; ++index )
	{
		InSDG.simpleElements.AddLine( OutVerts[index], OutVerts[index+1], InColor, 0.f );
	}
	InSDG.simpleElements.AddLine( OutVerts[OutVerts.size()-1], OutVerts[0], InColor, 0.f );
#endif // WITH_EDITOR
}

#if ENABLE_HITPROXY
/*
==================
DrawHitProxyCircle
==================
*/
void DrawHitProxyCircle( struct SceneDepthGroup& InSDG, EHitProxyLayer InHitProxyLayer, const Vector& InLocation, const Vector& InX, const Vector& InY, const CHitProxyId& InHitProxyId, float InRadius, uint32 InNumSides, float InThickness /*= 0.f*/ )
{
#if WITH_EDITOR
	const float		angleDelta = 2.f * PI / InNumSides;
	Vector			lastVertex = InLocation + InX * InRadius;

	for ( uint32 sideIndex = 0; sideIndex < InNumSides; ++sideIndex )
	{
		const Vector	vertex = InLocation + ( InX * Math::Cos( angleDelta * ( sideIndex + 1 ) ) + InY * Math::Sin( angleDelta * ( sideIndex + 1 ) ) ) * InRadius;
		InSDG.hitProxyLayers[ InHitProxyLayer ].simpleHitProxyElements.AddLine( lastVertex, vertex, InHitProxyId.GetColor(), InThickness );
		lastVertex = vertex;
	}
#endif // WITH_EDITOR
}
#endif // ENABLE_HITPROXY


/*
==================
CWhiteTexture::InitRHI
==================
*/
void CWhiteTexture::InitRHI()
{
	uint8		rgba[4] = { 0xFF, 0xFF, 0xFF, 0xFF };
	texture2DRHI = g_RHI->CreateTexture2D( TEXT( "WhiteTexture"), 1, 1, PF_A8R8G8B8, 1, 0, rgba );
}

/*
==================
CWhiteTexture::ReleaseRHI
==================
*/
void CWhiteTexture::ReleaseRHI()
{
	texture2DRHI.SafeRelease();
}


/*
==================
CBlackTexture::InitRHI
==================
*/
void CBlackTexture::InitRHI()
{
	texture2DRHI = g_RHI->CreateTexture2D( TEXT( "BlackTexture" ), 1, 1, PF_A8R8G8B8, 1, 0, nullptr );
}

/*
==================
CBlackTexture::ReleaseRHI
==================
*/
void CBlackTexture::ReleaseRHI()
{
	texture2DRHI.SafeRelease();
}


/*
==================
CEmptyNormalTexture::InitRHI
==================
*/
void CEmptyNormalTexture::InitRHI()
{
	uint8		rgba[4] = { 0x7F, 0x7F, 0xFF, 0xFF };
	texture2DRHI = g_RHI->CreateTexture2D( TEXT( "EmptyNormalTexture" ), 1, 1, PF_A8R8G8B8, 1, 0, rgba );
}

/*
==================
CEmptyNormalTexture::ReleaseRHI
==================
*/
void CEmptyNormalTexture::ReleaseRHI()
{
	texture2DRHI.SafeRelease();
}