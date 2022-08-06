#include "Misc/EngineGlobals.h"
#include "RHI/BaseRHI.h"
#include "RHI/BaseViewportRHI.h"
#include "Render/Scene.h"
#include "Render/DynamicMeshBuilder.h"
#include "System/BaseEngine.h"
#include "System/Gizmo.h"

/**
 * @ingroup WorldEd
 * @brief Macro of count segments in arrow axis
 */
#define AXIS_ARROW_SEGMENTS		6

/**
 * @ingroup WorldEd
 * @brief Macro of axis arrow radius
 */
#define AXIS_ARROW_RADIUS		5

/**
 * @ingroup WorldEd
 * @brief Macro of axis cube scale
 */
#define CUBE_SCALE				4.0f

CGizmo::CGizmo()
	: bEnabled( false )
	, type( GT_Translate )
	, location( SMath::vectorZero )
	, axisColorX( CColor::red )
	, axisColorY( 0, 0, 255 )
	, axisColorZ( 0, 255, 0 )
{}

CGizmo::~CGizmo()
{}

void CGizmo::Init()
{
	axisMaterialX	= ( TAssetHandle<CMaterial> )GPackageManager->FindAsset( TEXT( "Material'EditorMaterials:AxisX_Mat" ), AT_Material );
	axisMaterialY	= ( TAssetHandle<CMaterial> )GPackageManager->FindAsset( TEXT( "Material'EditorMaterials:AxisY_Mat" ), AT_Material );
	axisMaterialZ	= ( TAssetHandle<CMaterial> )GPackageManager->FindAsset( TEXT( "Material'EditorMaterials:AxisZ_Mat" ), AT_Material );
}

void CGizmo::Draw_RenderThread( ViewportRHIRef_t InViewportRHI, class CSceneView* InSceneView, class CScene* InScene )
{
	switch ( type )
	{
	case GT_Translate:		Render_Translate( InViewportRHI, InSceneView, InScene );	break;
	case GT_Scale:			Render_Scale( InViewportRHI, InSceneView, InScene );		break;
	}
}

#define CUBE_VERT( x, y, z )		meshBuilder->AddVertex( SDynamicMeshVertexType{ Vector4D( (x) * CUBE_SCALE + 52.f, (y) * CUBE_SCALE, (z) * CUBE_SCALE, 1.f ), Vector2D( 0.f, 0.f ), Vector4D( 0.f, 0.f, 0.f, 0.f ), Vector4D( 0.f, 0.f, 0.f, 0.f ), Vector4D( 0.f, 0.f, 0.f, 0.f ), Vector4D( 1.f, 1.f, 1.f, 1.f ) } )
#define CUBE_FACE( i, j, k )		meshBuilder->AddTriangle( cubeVerts[ (i) ], cubeVerts[ (j) ], cubeVerts[ (k) ] )

void CGizmo::Render_Axis( const class CSceneView* InSceneView, struct SSceneDepthGroup& InSDG, EAxis InAxis, const Matrix& InMatrix, const TAssetHandle<CMaterial>& InMaterial, const CColor& InColor, float InScale, bool InIsCubeHead /*= false*/ )
{
	Matrix						arrowToWorld	= InMatrix * SMath::ScaleMatrix( Vector( InScale, InScale, InScale ) );
	DynamicMeshBuilderRef_t		meshBuilder		= new CDynamicMeshBuilder();

	// Draw axis line
	InSDG.simpleElements.AddLine( InMatrix * Vector4D( 8.f * InScale, 0.f, 0.f, 1.f ), InMatrix * Vector4D( 48.f * InScale, 0.f, 0.f, 1.f ), InColor );

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
	InSDG.dynamicMeshBuilders.push_back( SDynamicMeshBuilderElement{ meshBuilder, arrowToWorld, InMaterial } );
}

void CGizmo::Render_Translate( ViewportRHIRef_t InViewportRHI, class CSceneView* InSceneView, class CScene* InScene )
{
	SSceneDepthGroup&	SDG		= InScene->GetSDG( SDG_WorldEdForeground );
	float				scale	= InSceneView->WorldToScreen( location ).w * ( 4.f / InSceneView->GetSizeX() / InSceneView->GetProjectionMatrix()[0][0] );

	// Figure out axis matrices
	Matrix		xMatrix = SMath::TranslateMatrix( location );
	Matrix		yMatrix = SMath::TranslateMatrix( location ) * CRotator( 0, 0, 90 ).ToMatrix();
	Matrix		zMatrix = SMath::TranslateMatrix( location ) * CRotator( 0, -90, 0 ).ToMatrix();

	// Draw axis X
	Render_Axis( InSceneView, SDG, A_X, xMatrix, axisMaterialX, axisColorX, scale );
	SDG.simpleElements.AddLine( xMatrix * Vector4D( Vector( 16.f, 0.f, 0.f ) * scale, 1.f ), xMatrix * Vector4D( Vector( 16.f, 16.f, 0.f ) * scale, 1.f ), axisColorX );
	SDG.simpleElements.AddLine( xMatrix * Vector4D( Vector( 16.f, 16.f, 0.f ) * scale, 1.f ), xMatrix * Vector4D( Vector( 0, 16.f, 0.f ) * scale, 1.f ), axisColorY );

	// Draw axis Y
	Render_Axis( InSceneView, SDG, A_Y, yMatrix, axisMaterialY, axisColorY, scale );
	SDG.simpleElements.AddLine( xMatrix * Vector4D( Vector( 16.f, 0.f, 0.f ) * scale, 1.f ), xMatrix * Vector4D( Vector( 16.f, 0.f, 16.f ) * scale, 1.f ), axisColorX );
	SDG.simpleElements.AddLine( xMatrix * Vector4D( Vector( 16.f, 0.f, 16.f ) * scale, 1.f ), xMatrix * Vector4D( Vector( 0, 0.f, 16.f ) * scale, 1.f ), axisColorZ );

	// Draw axis Z
	Render_Axis( InSceneView, SDG, A_Z, zMatrix, axisMaterialZ, axisColorZ, scale );
	SDG.simpleElements.AddLine( xMatrix * Vector4D( Vector( 0.f, 16.f, 0.f ) * scale, 1.f ), xMatrix * Vector4D( Vector( 0.f, 16.f, 16.f ) * scale, 1.f ), axisColorY );
	SDG.simpleElements.AddLine( xMatrix * Vector4D( Vector( 0.f, 16.f, 16.f ) * scale, 1.f ), xMatrix * Vector4D( Vector( 0, 0.f, 16.f ) * scale, 1.f ), axisColorZ );
}

void CGizmo::Render_Scale( ViewportRHIRef_t InViewportRHI, class CSceneView* InSceneView, class CScene* InScene )
{
	SSceneDepthGroup&	SDG		= InScene->GetSDG( SDG_WorldEdForeground );
	float				scale	= InSceneView->WorldToScreen( location ).w * ( 4.f / InSceneView->GetSizeX() / InSceneView->GetProjectionMatrix()[ 0 ][ 0 ] );

	// Figure out axis matrices
	Matrix		xMatrix = SMath::TranslateMatrix( location );
	Matrix		yMatrix = SMath::TranslateMatrix( location ) * CRotator( 0, 0, 90 ).ToMatrix();
	Matrix		zMatrix = SMath::TranslateMatrix( location ) * CRotator( 0, -90, 0 ).ToMatrix();

	// Draw axis X
	Render_Axis( InSceneView, SDG, A_X, xMatrix, axisMaterialX, axisColorX, scale, true );
	SDG.simpleElements.AddLine( xMatrix * Vector4D( Vector( 16.f, 0.f, 0.f ) * scale, 1.f ), xMatrix * Vector4D( Vector( 8.f, 8.f, 0.f ) * scale, 1.f ), axisColorX );
	SDG.simpleElements.AddLine( xMatrix * Vector4D( Vector( 8.f, 8.f, 0.f ) * scale, 1.f ), xMatrix * Vector4D( Vector( 0, 16.f, 0.f ) * scale, 1.f ), axisColorY );

	// Draw axis Y
	Render_Axis( InSceneView, SDG, A_Y, yMatrix, axisMaterialY, axisColorY, scale, true );
	SDG.simpleElements.AddLine( xMatrix * Vector4D( Vector( 16.f, 0.f, 0.f ) * scale, 1.f ), xMatrix * Vector4D( Vector( 8.f, 0.f, 8.f ) * scale, 1.f ), axisColorX );
	SDG.simpleElements.AddLine( xMatrix * Vector4D( Vector( 8.f, 0.f, 8.f ) * scale, 1.f ), xMatrix * Vector4D( Vector( 0, 0.f, 16.f ) * scale, 1.f ), axisColorZ );

	// Draw axis Z
	Render_Axis( InSceneView, SDG, A_Z, zMatrix, axisMaterialZ, axisColorZ, scale, true );
	SDG.simpleElements.AddLine( xMatrix * Vector4D( Vector( 0.f, 16.f, 0.f ) * scale, 1.f ), xMatrix * Vector4D( Vector( 0.f, 8.f, 8.f ) * scale, 1.f ), axisColorY );
	SDG.simpleElements.AddLine( xMatrix * Vector4D( Vector( 0.f, 8.f, 8.f ) * scale, 1.f ), xMatrix * Vector4D( Vector( 0, 0.f, 16.f ) * scale, 1.f ), axisColorZ );
}