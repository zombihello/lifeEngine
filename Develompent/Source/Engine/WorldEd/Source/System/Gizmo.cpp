#include "Misc/EngineGlobals.h"
#include "Misc/WorldEdGlobals.h"
#include "RHI/BaseRHI.h"
#include "RHI/BaseViewportRHI.h"
#include "Render/Scene.h"
#include "Render/DynamicMeshBuilder.h"
#include "Render/RenderUtils.h"
#include "System/BaseEngine.h"
#include "System/Gizmo.h"
#include "System/EditorEngine.h"

/**
 * @ingroup WorldEd
 * @brief Macro of count segments in arrow axis
 */
#define AXIS_ARROW_SEGMENTS			6

/**
 * @ingroup WorldEd
 * @brief Macro of axis arrow radius
 */
#define AXIS_ARROW_RADIUS			5

/**
 * @ingroup WorldEd
 * @brief Macro of axis cube scale
 */
#define CUBE_SCALE					4.0f

/**
 * @ingroup WorldEd
 * @brief Macro of circle radius
 */
#define CIRCLE_RADIUS				32.f

/**
 * @ingroup WorldEd
 * @brief Macro of circle segments
 */
#define CIRCLE_SEGMENTS				32

/**
 * @ingroup WorldEd
 * @brief Macro of thickness lines for hit proxy
 */
#define HITPROXY_THICKNESS_LINE		10

CGizmo::COnUpdateAllGizmo		CGizmo::onUpdateAllGizmo;

CGizmo::CGizmo()
	: bEnabled( false )
	, type( GT_None )
	, location( SMath::vectorZero )
	, screenLocation( 0.f, 0.f )
	, axisColorX( CColor::red )
	, axisColorY( 0, 0, 255 )
	, axisColorZ( 0, 255, 0 )
	, currentAxisColor( 255, 255, 0 )
	, currentAxis( A_None )
	, axisXEnd( 0.f, 0.f )
	, axisYEnd( 0.f, 0.f )
	, axisZEnd( 0.f, 0.f )
	, editorModeChangedDelegate( nullptr )
	, updateAllGizmoDelegate( nullptr )
{}

CGizmo::~CGizmo()
{
	if ( editorModeChangedDelegate )
	{
		SEditorDelegates::onEditorModeChanged.Remove( editorModeChangedDelegate );
	}

	if ( updateAllGizmoDelegate )
	{
		onUpdateAllGizmo.Remove( updateAllGizmoDelegate );
	}
}

void CGizmo::Init()
{
	// Find materials for axis
	axisMaterialX		= ( TAssetHandle<CMaterial> )GPackageManager->FindAsset( TEXT( "Material'EditorMaterials:AxisX_Mat" ), AT_Material );
	axisMaterialY		= ( TAssetHandle<CMaterial> )GPackageManager->FindAsset( TEXT( "Material'EditorMaterials:AxisY_Mat" ), AT_Material );
	axisMaterialZ		= ( TAssetHandle<CMaterial> )GPackageManager->FindAsset( TEXT( "Material'EditorMaterials:AxisZ_Mat" ), AT_Material );

	// Init gizmo type
	OnEditorModeChanged( GEditorEngine->GetEditorMode() );

	editorModeChangedDelegate	= SEditorDelegates::onEditorModeChanged.Add(	std::bind( &CGizmo::OnEditorModeChanged, this, std::placeholders::_1 )						);
	updateAllGizmoDelegate		= onUpdateAllGizmo.Add(							std::bind( &CGizmo::OnUpdateGizmo, this, std::placeholders::_1, std::placeholders::_2 )		);
}

void CGizmo::Draw_RenderThread( ViewportRHIRef_t InViewportRHI, class CSceneView* InSceneView, class CScene* InScene, ELevelViewportType InViewportType )
{
	switch ( type )
	{
	case GT_Translate:			Render_Translate( InViewportRHI, InSceneView, InScene, InViewportType );		break;
	case GT_Rotate:				Render_Rotate( InViewportRHI, InSceneView, InScene, InViewportType );			break;
	case GT_Scale:				Render_Scale( InViewportRHI, InSceneView, InScene, InViewportType );			break;
	case GT_ScaleNonUniform:	Render_ScaleNonUniform( InViewportRHI, InSceneView, InScene, InViewportType );	break;
	}

	screenLocation = InSceneView->WorldToScreen( location );
}

#define CUBE_VERT( x, y, z )		meshBuilder->AddVertex( SDynamicMeshVertexType{ Vector4D( (x) * CUBE_SCALE + 52.f, (y) * CUBE_SCALE, (z) * CUBE_SCALE, 1.f ), Vector2D( 0.f, 0.f ), Vector4D( 0.f, 0.f, 0.f, 0.f ), Vector4D( 0.f, 0.f, 0.f, 0.f ), Vector4D( 0.f, 0.f, 0.f, 0.f ), Vector4D( 1.f, 1.f, 1.f, 1.f ) } )
#define CUBE_FACE( i, j, k )		meshBuilder->AddTriangle( cubeVerts[ (i) ], cubeVerts[ (j) ], cubeVerts[ (k) ] )

void CGizmo::Render_Axis( const class CSceneView* InSceneView, struct SSceneDepthGroup& InSDG, uint8 InAxis, const Matrix& InMatrix, const TAssetHandle<CMaterial>& InMaterial, const CColor& InColor, Vector2D* OutAxisEnd, float InScale, bool InIsCubeHead /*= false*/, bool InDrawAxisLine /*= true*/ )
{
	Matrix						arrowToWorld	= InMatrix * SMath::ScaleMatrix( Vector( InScale, InScale, InScale ) );
	DynamicMeshBuilderRef_t		meshBuilder		= new CDynamicMeshBuilder();

	// Draw axis line
	if ( InDrawAxisLine )
	{
		InSDG.simpleElements.AddLine( InMatrix * Vector4D( 8.f * InScale, 0.f, 0.f, 1.f ), InMatrix * Vector4D( 48.f * InScale, 0.f, 0.f, 1.f ), InColor );

#if ENABLE_HITPROXY
		InSDG.hitProxyLayers[HPL_UI].simpleHitProxyElements.AddLine( InMatrix * Vector4D( 8.f * InScale, 0.f, 0.f, 1.f ), InMatrix * Vector4D( 48.f * InScale, 0.f, 0.f, 1.f ), CHitProxyId( InAxis ), HITPROXY_THICKNESS_LINE * InScale );
#endif // ENABLE_HITPROXY
	}

	// Draw cube on head if need
	if ( InIsCubeHead )
	{
		// Compute cube vertices
		int32		cubeVerts[ 8 ];
		cubeVerts[ 0 ] = CUBE_VERT( -1, -1, 1 );
		cubeVerts[ 1 ] = CUBE_VERT( 1, -1, 1 );
		cubeVerts[ 2 ] = CUBE_VERT( 1, 1, 1 );
		cubeVerts[ 3 ] = CUBE_VERT( -1, 1, 1 );
		cubeVerts[ 4 ] = CUBE_VERT( -1, -1, -1 );
		cubeVerts[ 5 ] = CUBE_VERT( 1, -1, -1 );
		cubeVerts[ 6 ] = CUBE_VERT( 1, 1, -1 );
		cubeVerts[ 7 ] = CUBE_VERT( -1, 1, -1 );
		CUBE_FACE( 0, 1, 2 );
		CUBE_FACE( 2, 3, 0 );
		CUBE_FACE( 1, 5, 6 );
		CUBE_FACE( 6, 2, 1 );

		CUBE_FACE( 7, 6, 5 );
		CUBE_FACE( 5, 4, 7 );
		CUBE_FACE( 4, 0, 3 );
		CUBE_FACE( 3, 7, 4 );

		CUBE_FACE( 4, 5, 1 );
		CUBE_FACE( 1, 0, 4 );
		CUBE_FACE( 3, 2, 6 );
		CUBE_FACE( 6, 7, 3 );
	}
	// Else we draw cones on head
	else
	{
		SDynamicMeshVertexType		vertex;
		appMemzero( &vertex, sizeof( SDynamicMeshVertexType ) );
		vertex.color				= InColor.ToNormalizedVector4D();

		// Compute the arrow cone vertices
		int32		arrowVertices[ AXIS_ARROW_SEGMENTS ];
		for ( uint32 vertexIndex = 0; vertexIndex < AXIS_ARROW_SEGMENTS; ++vertexIndex )
		{
			float						theta = PI * 2.f * vertexIndex / AXIS_ARROW_SEGMENTS;		

			vertex.position	= Vector4D( 40.f, AXIS_ARROW_RADIUS * SMath::Cos( theta ) * 0.5f, AXIS_ARROW_RADIUS * SMath::Sin( theta ) * 0.5f, 1.f );
			arrowVertices[ vertexIndex ] = meshBuilder->AddVertex( vertex );
		}
		
		// Add root arrow vertex
		vertex.position = Vector4D( 54.f, 0.f, 0.f, 1.f );
		int32		rootArrowVertex = meshBuilder->AddVertex( vertex );

		// Build the arrow mesh
		for ( uint32 segment = 0; segment < AXIS_ARROW_SEGMENTS; ++segment )
		{
			meshBuilder->AddTriangle( rootArrowVertex, arrowVertices[ segment ], arrowVertices[ ( segment + 1 ) % AXIS_ARROW_SEGMENTS ] );
		}
	}

	// Draw the arrow mesh
	meshBuilder->Build();

#if ENABLE_HITPROXY
	meshBuilder->SetHitProxyId( CHitProxyId( InAxis ) );
	InSDG.hitProxyLayers[ HPL_UI ].dynamicHitProxyMeshBuilders.push_back( SDynamicMeshBuilderElement{ meshBuilder, arrowToWorld, InMaterial } );
#endif // ENABLE_HITPROXY

	InSDG.dynamicMeshBuilders.push_back( SDynamicMeshBuilderElement{ meshBuilder, arrowToWorld, InMaterial } );
	if ( OutAxisEnd )
	{
		*OutAxisEnd = InSceneView->WorldToScreen( arrowToWorld * Vector4D( 64.f, 0.f, 0.f, 1.f ) );
	}
}

void CGizmo::Render_Translate( ViewportRHIRef_t InViewportRHI, class CSceneView* InSceneView, class CScene* InScene, ELevelViewportType InViewportType )
{
	SSceneDepthGroup&	SDG		= InScene->GetSDG( SDG_WorldEdForeground );
	float				scale	= InSceneView->WorldToScreen( location ).w * ( 4.f / InSceneView->GetSizeX() / InSceneView->GetProjectionMatrix()[0][0] );

	bool				bDrawAxisX = InViewportType == LVT_Perspective || InViewportType == LVT_OrthoXY || InViewportType == LVT_OrthoXZ;
	bool				bDrawAxisY = InViewportType == LVT_Perspective || InViewportType == LVT_OrthoXY || InViewportType == LVT_OrthoYZ;
	bool				bDrawAxisZ = InViewportType == LVT_Perspective || InViewportType == LVT_OrthoXZ || InViewportType == LVT_OrthoYZ;

	// Figure out axis colors
	CColor*		xColor = &( currentAxis & A_X ? currentAxisColor : axisColorX );
	CColor*		yColor = &( currentAxis & A_Y ? currentAxisColor : axisColorY );
	CColor*		zColor = &( currentAxis & A_Z ? currentAxisColor : axisColorZ );

	// Figure out axis matrices
	Matrix		xMatrix = SMath::TranslateMatrix( location );
	Matrix		yMatrix = SMath::TranslateMatrix( location ) * SMath::QuaternionToMatrix( SMath::AnglesToQuaternionXYZ( 0.f, 0.f, 90.f ) );
	Matrix		zMatrix = SMath::TranslateMatrix( location ) * SMath::QuaternionToMatrix( SMath::AnglesToQuaternionXYZ( 0.f, -90.f, 0.f ) );

	// Draw axis X
	if ( bDrawAxisX )
	{
		Render_Axis( InSceneView, SDG, A_X, xMatrix, axisMaterialX, *xColor, &axisXEnd, scale );
		SDG.simpleElements.AddLine( xMatrix * Vector4D( Vector( 16.f, 0.f, 0.f ) * scale, 1.f ), xMatrix * Vector4D( Vector( 16.f, 16.f, 0.f ) * scale, 1.f ), *xColor );
		SDG.simpleElements.AddLine( xMatrix * Vector4D( Vector( 16.f, 16.f, 0.f ) * scale, 1.f ), xMatrix * Vector4D( Vector( 0, 16.f, 0.f ) * scale, 1.f ), *yColor );

#if ENABLE_HITPROXY
		SDG.hitProxyLayers[HPL_UI].simpleHitProxyElements.AddLine( xMatrix * Vector4D( Vector( 16.f, 0.f, 0.f ) * scale, 1.f ), xMatrix * Vector4D( Vector( 16.f, 16.f, 0.f ) * scale, 1.f ), CHitProxyId( A_X | A_Y ), HITPROXY_THICKNESS_LINE * scale );
		SDG.hitProxyLayers[HPL_UI].simpleHitProxyElements.AddLine( xMatrix * Vector4D( Vector( 16.f, 16.f, 0.f ) * scale, 1.f ), xMatrix * Vector4D( Vector( 0, 16.f, 0.f ) * scale, 1.f ), CHitProxyId( A_X | A_Y ) ), HITPROXY_THICKNESS_LINE* scale;
#endif // ENABLE_HITPROXY
	}

	// Draw axis Y
	if ( bDrawAxisY )
	{
		Render_Axis( InSceneView, SDG, A_Y, yMatrix, axisMaterialY, *yColor, &axisYEnd, scale );
		SDG.simpleElements.AddLine( xMatrix * Vector4D( Vector( 16.f, 0.f, 0.f ) * scale, 1.f ), xMatrix * Vector4D( Vector( 16.f, 0.f, 16.f ) * scale, 1.f ), *xColor );
		SDG.simpleElements.AddLine( xMatrix * Vector4D( Vector( 16.f, 0.f, 16.f ) * scale, 1.f ), xMatrix * Vector4D( Vector( 0, 0.f, 16.f ) * scale, 1.f ), *zColor );

#if ENABLE_HITPROXY
		SDG.hitProxyLayers[HPL_UI].simpleHitProxyElements.AddLine( xMatrix * Vector4D( Vector( 16.f, 0.f, 0.f ) * scale, 1.f ), xMatrix * Vector4D( Vector( 16.f, 0.f, 16.f ) * scale, 1.f ), CHitProxyId( A_X | A_Z ), HITPROXY_THICKNESS_LINE * scale );
		SDG.hitProxyLayers[HPL_UI].simpleHitProxyElements.AddLine( xMatrix * Vector4D( Vector( 16.f, 0.f, 16.f ) * scale, 1.f ), xMatrix * Vector4D( Vector( 0, 0.f, 16.f ) * scale, 1.f ), CHitProxyId( A_X | A_Z ), HITPROXY_THICKNESS_LINE * scale );
#endif // ENABLE_HITPROXY
	}

	// Draw axis Z
	if ( bDrawAxisZ )
	{
		Render_Axis( InSceneView, SDG, A_Z, zMatrix, axisMaterialZ, *zColor, &axisZEnd, scale );
		SDG.simpleElements.AddLine( xMatrix * Vector4D( Vector( 0.f, 16.f, 0.f ) * scale, 1.f ), xMatrix * Vector4D( Vector( 0.f, 16.f, 16.f ) * scale, 1.f ), *yColor );
		SDG.simpleElements.AddLine( xMatrix * Vector4D( Vector( 0.f, 16.f, 16.f ) * scale, 1.f ), xMatrix * Vector4D( Vector( 0, 0.f, 16.f ) * scale, 1.f ), *zColor );

#if ENABLE_HITPROXY
		SDG.hitProxyLayers[HPL_UI].simpleHitProxyElements.AddLine( xMatrix * Vector4D( Vector( 0.f, 16.f, 0.f ) * scale, 1.f ), xMatrix * Vector4D( Vector( 0.f, 16.f, 16.f ) * scale, 1.f ), CHitProxyId( A_Y | A_Z ), HITPROXY_THICKNESS_LINE * scale );
		SDG.hitProxyLayers[HPL_UI].simpleHitProxyElements.AddLine( xMatrix * Vector4D( Vector( 0.f, 16.f, 16.f ) * scale, 1.f ), xMatrix * Vector4D( Vector( 0, 0.f, 16.f ) * scale, 1.f ), CHitProxyId( A_Y | A_Z ), HITPROXY_THICKNESS_LINE * scale );
#endif // ENABLE_HITPROXY
	}
}

void CGizmo::Render_Rotate( ViewportRHIRef_t InViewportRHI, class CSceneView* InSceneView, class CScene* InScene, ELevelViewportType InViewportType )
{
	SSceneDepthGroup&	SDG		= InScene->GetSDG( SDG_WorldEdForeground );
	float				scale	= InSceneView->WorldToScreen( location ).w * ( 4.f / InSceneView->GetSizeX() / InSceneView->GetProjectionMatrix()[ 0 ][ 0 ] );

	bool				bDrawAxisX = InViewportType == LVT_Perspective || InViewportType == LVT_OrthoYZ;
	bool				bDrawAxisY = InViewportType == LVT_Perspective || InViewportType == LVT_OrthoXZ;
	bool				bDrawAxisZ = InViewportType == LVT_Perspective || InViewportType == LVT_OrthoXY;

	// Figure out axis colors
	CColor*				xColor = &( currentAxis & A_X ? currentAxisColor : axisColorX );
	CColor*				yColor = &( currentAxis & A_Y ? currentAxisColor : axisColorY );
	CColor*				zColor = &( currentAxis & A_Z ? currentAxisColor : axisColorZ );

	// Draw circle for axis
	if ( bDrawAxisZ )
	{
		DrawCircle( SDG, location, Vector( 1, 0, 0 ), Vector( 0, 1, 0 ), *zColor, CIRCLE_RADIUS * scale, CIRCLE_SEGMENTS );
#if ENABLE_HITPROXY
		DrawHitProxyCircle( SDG, HPL_UI, location, Vector( 1, 0, 0 ), Vector( 0, 1, 0 ), CHitProxyId( A_Z ), CIRCLE_RADIUS * scale, CIRCLE_SEGMENTS, HITPROXY_THICKNESS_LINE * scale );
#endif // ENABLE_HITPROXY
	}

	if ( bDrawAxisY )
	{
		DrawCircle( SDG, location, Vector( 1, 0, 0 ), Vector( 0, 0, 1 ), *yColor, CIRCLE_RADIUS * scale, CIRCLE_SEGMENTS );
#if ENABLE_HITPROXY
		DrawHitProxyCircle( SDG, HPL_UI, location, Vector( 1, 0, 0 ), Vector( 0, 0, 1 ), CHitProxyId( A_Y ), CIRCLE_RADIUS * scale, CIRCLE_SEGMENTS, HITPROXY_THICKNESS_LINE * scale );
#endif // ENABLE_HITPROXY
	}

	if ( bDrawAxisX )
	{
		DrawCircle( SDG, location, Vector( 0, 1, 0 ), Vector( 0, 0, 1 ), *xColor, CIRCLE_RADIUS * scale, CIRCLE_SEGMENTS );
#if ENABLE_HITPROXY
		DrawHitProxyCircle( SDG, HPL_UI, location, Vector( 0, 1, 0 ), Vector( 0, 0, 1 ), CHitProxyId( A_X ), CIRCLE_RADIUS * scale, CIRCLE_SEGMENTS, HITPROXY_THICKNESS_LINE * scale );
#endif // ENABLE_HITPROXY
	}
}

void CGizmo::Render_Scale( ViewportRHIRef_t InViewportRHI, class CSceneView* InSceneView, class CScene* InScene, ELevelViewportType InViewportType )
{
	SSceneDepthGroup&	SDG		= InScene->GetSDG( SDG_WorldEdForeground );
	float				scale	= InSceneView->WorldToScreen( location ).w * ( 4.f / InSceneView->GetSizeX() / InSceneView->GetProjectionMatrix()[0][0] );

	bool				bDrawAxisX = InViewportType == LVT_Perspective || InViewportType == LVT_OrthoXY || InViewportType == LVT_OrthoXZ;
	bool				bDrawAxisY = InViewportType == LVT_Perspective || InViewportType == LVT_OrthoXY || InViewportType == LVT_OrthoYZ;
	bool				bDrawAxisZ = InViewportType == LVT_Perspective || InViewportType == LVT_OrthoXZ || InViewportType == LVT_OrthoYZ;

	// Figure out axis colors
	CColor*		xColor	= &( currentAxis & ( A_X | A_Y | A_Z ) ? currentAxisColor : axisColorX );

	// Figure out axis matrices
	Matrix		xMatrix = SMath::TranslateMatrix( location );
	Matrix		yMatrix = SMath::TranslateMatrix( location ) * SMath::QuaternionToMatrix( SMath::AnglesToQuaternionXYZ( Vector( 0.f, 0.f, 90.f ) ) );
	Matrix		zMatrix = SMath::TranslateMatrix( location ) * SMath::QuaternionToMatrix( SMath::AnglesToQuaternionXYZ( Vector( 0.f, -90.f, 0.f ) ) );

	// Draw axis X
	if ( bDrawAxisX )
	{
		Render_Axis( InSceneView, SDG, A_X | A_Y | A_Z, xMatrix, axisMaterialX, *xColor, &axisXEnd, scale, true );
		SDG.simpleElements.AddLine( xMatrix * Vector4D( Vector( 16.f, 0.f, 0.f ) * scale, 1.f ), xMatrix * Vector4D( Vector( 8.f, 8.f, 0.f ) * scale, 1.f ), *xColor );
		SDG.simpleElements.AddLine( xMatrix * Vector4D( Vector( 8.f, 8.f, 0.f ) * scale, 1.f ), xMatrix * Vector4D( Vector( 0, 16.f, 0.f ) * scale, 1.f ), *xColor );

#if ENABLE_HITPROXY
		SDG.hitProxyLayers[HPL_UI].simpleHitProxyElements.AddLine( xMatrix * Vector4D( Vector( 16.f, 0.f, 0.f ) * scale, 1.f ), xMatrix * Vector4D( Vector( 8.f, 8.f, 0.f ) * scale, 1.f ), CHitProxyId( A_X | A_Y | A_Z ), HITPROXY_THICKNESS_LINE * scale );
		SDG.hitProxyLayers[HPL_UI].simpleHitProxyElements.AddLine( xMatrix * Vector4D( Vector( 8.f, 8.f, 0.f ) * scale, 1.f ), xMatrix * Vector4D( Vector( 0, 16.f, 0.f ) * scale, 1.f ), CHitProxyId( A_X | A_Y | A_Z ), HITPROXY_THICKNESS_LINE * scale );
#endif // ENABLE_HITPROXY
	}

	// Draw axis Y
	if ( bDrawAxisY )
	{
		Render_Axis( InSceneView, SDG, A_X | A_Y | A_Z, yMatrix, axisMaterialX, *xColor, &axisYEnd, scale, true );
		SDG.simpleElements.AddLine( xMatrix * Vector4D( Vector( 16.f, 0.f, 0.f ) * scale, 1.f ), xMatrix * Vector4D( Vector( 8.f, 0.f, 8.f ) * scale, 1.f ), *xColor );
		SDG.simpleElements.AddLine( xMatrix * Vector4D( Vector( 8.f, 0.f, 8.f ) * scale, 1.f ), xMatrix * Vector4D( Vector( 0, 0.f, 16.f ) * scale, 1.f ), *xColor );

#if ENABLE_HITPROXY
		SDG.hitProxyLayers[HPL_UI].simpleHitProxyElements.AddLine( xMatrix * Vector4D( Vector( 16.f, 0.f, 0.f ) * scale, 1.f ), xMatrix * Vector4D( Vector( 8.f, 0.f, 8.f ) * scale, 1.f ), CHitProxyId( A_X | A_Y | A_Z ), HITPROXY_THICKNESS_LINE * scale );
		SDG.hitProxyLayers[HPL_UI].simpleHitProxyElements.AddLine( xMatrix * Vector4D( Vector( 8.f, 0.f, 8.f ) * scale, 1.f ), xMatrix * Vector4D( Vector( 0, 0.f, 16.f ) * scale, 1.f ), CHitProxyId( A_X | A_Y | A_Z ), HITPROXY_THICKNESS_LINE * scale );
#endif // ENABLE_HITPROXY
	}

	// Draw axis Z
	if ( bDrawAxisZ )
	{
		Render_Axis( InSceneView, SDG, A_X | A_Y | A_Z, zMatrix, axisMaterialX, *xColor, &axisZEnd, scale, true );
		SDG.simpleElements.AddLine( xMatrix * Vector4D( Vector( 0.f, 16.f, 0.f ) * scale, 1.f ), xMatrix * Vector4D( Vector( 0.f, 8.f, 8.f ) * scale, 1.f ), *xColor );
		SDG.simpleElements.AddLine( xMatrix * Vector4D( Vector( 0.f, 8.f, 8.f ) * scale, 1.f ), xMatrix * Vector4D( Vector( 0, 0.f, 16.f ) * scale, 1.f ), *xColor );

#if ENABLE_HITPROXY
		SDG.hitProxyLayers[HPL_UI].simpleHitProxyElements.AddLine( xMatrix * Vector4D( Vector( 0.f, 16.f, 0.f ) * scale, 1.f ), xMatrix * Vector4D( Vector( 0.f, 8.f, 8.f ) * scale, 1.f ), CHitProxyId( A_X | A_Y | A_Z ), HITPROXY_THICKNESS_LINE * scale );
		SDG.hitProxyLayers[HPL_UI].simpleHitProxyElements.AddLine( xMatrix * Vector4D( Vector( 0.f, 8.f, 8.f ) * scale, 1.f ), xMatrix * Vector4D( Vector( 0, 0.f, 16.f ) * scale, 1.f ), CHitProxyId( A_X | A_Y | A_Z ), HITPROXY_THICKNESS_LINE * scale );
#endif // ENABLE_HITPROXY
	}
}

void CGizmo::Render_ScaleNonUniform( ViewportRHIRef_t InViewportRHI, class CSceneView* InSceneView, class CScene* InScene, ELevelViewportType InViewportType )
{
	SSceneDepthGroup&	SDG		= InScene->GetSDG( SDG_WorldEdForeground );
	float				scale	= InSceneView->WorldToScreen( location ).w * ( 4.f / InSceneView->GetSizeX() / InSceneView->GetProjectionMatrix()[ 0 ][ 0 ] );

	bool				bDrawAxisX	= InViewportType == LVT_Perspective || InViewportType == LVT_OrthoXY || InViewportType == LVT_OrthoXZ;
	bool				bDrawAxisY	= InViewportType == LVT_Perspective || InViewportType == LVT_OrthoXY || InViewportType == LVT_OrthoYZ;
	bool				bDrawAxisZ	= InViewportType == LVT_Perspective || InViewportType == LVT_OrthoXZ || InViewportType == LVT_OrthoYZ;

	// Figure out axis colors
	CColor*		xColor		= &( currentAxis & A_X ? currentAxisColor : axisColorX );
	CColor*		yColor		= &( currentAxis & A_Y ? currentAxisColor : axisColorY );
	CColor*		zColor		= &( currentAxis & A_Z ? currentAxisColor : axisColorZ );

	// Figure out axis matrices
	Matrix		xMatrix			= SMath::TranslateMatrix( location );
	Matrix		yMatrix			= SMath::TranslateMatrix( location ) * SMath::QuaternionToMatrix( SMath::AnglesToQuaternionXYZ( Vector( 0.f, 0.f, 90.f ) ) );
	Matrix		zMatrix			= SMath::TranslateMatrix( location ) * SMath::QuaternionToMatrix( SMath::AnglesToQuaternionXYZ( Vector( 0.f, -90.f, 0.f ) ) );

	// Draw axis X
	if ( bDrawAxisX )
	{
		Render_Axis( InSceneView, SDG, A_X, xMatrix, axisMaterialX, *xColor, &axisXEnd, scale, true );
		SDG.simpleElements.AddLine( xMatrix * Vector4D( Vector( 16.f, 0.f, 0.f ) * scale, 1.f ), xMatrix * Vector4D( Vector( 8.f, 8.f, 0.f ) * scale, 1.f ), *xColor );
		SDG.simpleElements.AddLine( xMatrix * Vector4D( Vector( 8.f, 8.f, 0.f ) * scale, 1.f ), xMatrix * Vector4D( Vector( 0, 16.f, 0.f ) * scale, 1.f ), *yColor );

#if ENABLE_HITPROXY
		SDG.hitProxyLayers[HPL_UI].simpleHitProxyElements.AddLine( xMatrix * Vector4D( Vector( 16.f, 0.f, 0.f ) * scale, 1.f ), xMatrix * Vector4D( Vector( 8.f, 8.f, 0.f ) * scale, 1.f ), CHitProxyId( A_X | A_Y ), HITPROXY_THICKNESS_LINE * scale );
		SDG.hitProxyLayers[HPL_UI].simpleHitProxyElements.AddLine( xMatrix * Vector4D( Vector( 8.f, 8.f, 0.f ) * scale, 1.f ), xMatrix * Vector4D( Vector( 0, 16.f, 0.f ) * scale, 1.f ), CHitProxyId( A_X | A_Y ), HITPROXY_THICKNESS_LINE * scale );
#endif // ENABLE_HITPROXY
	}

	// Draw axis Y
	if ( bDrawAxisY )
	{
		Render_Axis( InSceneView, SDG, A_Y, yMatrix, axisMaterialY, *yColor, &axisYEnd, scale, true );
		SDG.simpleElements.AddLine( xMatrix * Vector4D( Vector( 16.f, 0.f, 0.f ) * scale, 1.f ), xMatrix * Vector4D( Vector( 8.f, 0.f, 8.f ) * scale, 1.f ), *xColor );
		SDG.simpleElements.AddLine( xMatrix * Vector4D( Vector( 8.f, 0.f, 8.f ) * scale, 1.f ), xMatrix * Vector4D( Vector( 0, 0.f, 16.f ) * scale, 1.f ), *zColor );

#if ENABLE_HITPROXY
		SDG.hitProxyLayers[HPL_UI].simpleHitProxyElements.AddLine( xMatrix * Vector4D( Vector( 16.f, 0.f, 0.f ) * scale, 1.f ), xMatrix * Vector4D( Vector( 8.f, 0.f, 8.f ) * scale, 1.f ), CHitProxyId( A_X | A_Z ), HITPROXY_THICKNESS_LINE * scale );
		SDG.hitProxyLayers[HPL_UI].simpleHitProxyElements.AddLine( xMatrix * Vector4D( Vector( 8.f, 0.f, 8.f ) * scale, 1.f ), xMatrix * Vector4D( Vector( 0, 0.f, 16.f ) * scale, 1.f ), CHitProxyId( A_X | A_Z ), HITPROXY_THICKNESS_LINE * scale );
#endif // ENABLE_HITPROXY
	}

	// Draw axis Z
	if ( bDrawAxisZ )
	{
		Render_Axis( InSceneView, SDG, A_Z, zMatrix, axisMaterialZ, *zColor, &axisZEnd, scale, true );
		SDG.simpleElements.AddLine( xMatrix * Vector4D( Vector( 0.f, 16.f, 0.f ) * scale, 1.f ), xMatrix * Vector4D( Vector( 0.f, 8.f, 8.f ) * scale, 1.f ), *yColor );
		SDG.simpleElements.AddLine( xMatrix * Vector4D( Vector( 0.f, 8.f, 8.f ) * scale, 1.f ), xMatrix * Vector4D( Vector( 0, 0.f, 16.f ) * scale, 1.f ), *zColor );

#if ENABLE_HITPROXY
		SDG.hitProxyLayers[HPL_UI].simpleHitProxyElements.AddLine( xMatrix * Vector4D( Vector( 0.f, 16.f, 0.f ) * scale, 1.f ), xMatrix * Vector4D( Vector( 0.f, 8.f, 8.f ) * scale, 1.f ), CHitProxyId( A_Y | A_Z ), HITPROXY_THICKNESS_LINE * scale );
		SDG.hitProxyLayers[HPL_UI].simpleHitProxyElements.AddLine( xMatrix * Vector4D( Vector( 0.f, 8.f, 8.f ) * scale, 1.f ), xMatrix * Vector4D( Vector( 0, 0.f, 16.f ) * scale, 1.f ), CHitProxyId( A_Y | A_Z ), HITPROXY_THICKNESS_LINE * scale );
#endif // ENABLE_HITPROXY
	}
}

void CGizmo::OnEditorModeChanged( EEditorMode InEditorMode )
{
	switch ( InEditorMode )
	{
	case EM_Translate:			type = GT_Translate;		break;
	case EM_Rotate:				type = GT_Rotate;			break;
	case EM_Scale:				type = GT_Scale;			break;
	case EM_ScaleNonUniform:	type = GT_ScaleNonUniform;	break;
	default:					type = GT_None;				break;
	}
}

void CGizmo::OnUpdateGizmo( bool InEnable, const Vector& InLocation )
{
	SetEnable( InEnable );
	SetLocation( InLocation );
}