#include "Math/Math.h"
#include "Render/Scene.h"

CSceneView::CSceneView( const Matrix& InProjectionMatrix, const Matrix& InViewMatrix, float InSizeX, float InSizeY, const ÑColor& InBackgroundColor, ShowFlags_t InShowFlags )
	: viewMatrix( InViewMatrix )
	, projectionMatrix( InProjectionMatrix )
	, viewProjectionMatrix( InProjectionMatrix * InViewMatrix )
	, backgroundColor( InBackgroundColor )
	, showFlags( InShowFlags )
	, sizeX( InSizeX )
	, sizeY( InSizeY )
{
	frustum.Update( viewProjectionMatrix );
}

void CSceneView::ScreenToWorld( const Vector2D& InScreenPoint, Vector& OutWorldOrigin, Vector& OutWorldDirection ) const
{
	int32	x = SMath::Trunc( InScreenPoint.x ),
			y = SMath::Trunc( InScreenPoint.y );

	// Get the eye position and direction of the mouse cursor in two stages (inverse transform projection, then inverse transform view).
	// This avoids the numerical instability that occurs when a view matrix with large translation is composed with a projection matrix
	Matrix		inverseView			= SMath::InverseMatrix( viewMatrix );
	Matrix		inverseProjection	= SMath::InverseMatrix( projectionMatrix );

	// The start of the raytrace is defined to be at mousex,mousey,0 in projection space
	// The end of the raytrace is at mousex, mousey, 0.5 in projection space
	float		screenSpaceX				= ( x - sizeX / 2.f ) / ( sizeX / 2.f );
	float		screenSpaceY				= ( y - sizeY / 2.f ) / -( sizeY / 2.f );
	Vector4D	rayStartProjectionSpace		= Vector4D( screenSpaceX, screenSpaceY, 0, 1.0f );
	Vector4D	rayEndProjectionSpace		= Vector4D( screenSpaceX, screenSpaceY, 0.5f, 1.0f );

	// Projection (changing the W coordinate) is not handled by the Matrix transforms that work with vectors, so multiplications
	// by the projection matrix should use homogenous coordinates.
	Vector4D	hgRayStartViewSpace = inverseProjection * rayStartProjectionSpace;
	Vector4D	hgRayEndViewSpace	= inverseProjection * rayEndProjectionSpace;
	Vector		rayStartViewSpace( hgRayStartViewSpace.x, hgRayStartViewSpace.y, hgRayStartViewSpace.z );
	Vector		rayEndViewSpace( hgRayEndViewSpace.x, hgRayEndViewSpace.y, hgRayEndViewSpace.z );
	
	// Divide vectors by W to undo any projection and get the 3-space coordinate 
	if ( hgRayStartViewSpace.w != 0.0f )
	{
		rayStartViewSpace /= hgRayStartViewSpace.w;
	}
	if ( hgRayEndViewSpace.w != 0.0f )
	{
		rayEndViewSpace /= hgRayEndViewSpace.w;
	}

	Vector		rayDirViewSpace = rayEndViewSpace - rayStartViewSpace;
	rayDirViewSpace = SMath::NormalizeVector( rayDirViewSpace );

	// The view transform does not have projection, so we can use the standard functions that deal with vectors and normals (normals
	// are vectors that do not use the translational part of a rotation/translation)
	Vector		rayStartWorldSpace	= inverseView * Vector4D( rayStartViewSpace, 1.f );
	Vector		rayDirWorldSpace	= inverseView * Vector4D( rayDirViewSpace, 0.f );

	// Finally, store the results in the hitcheck inputs.  The start position is the eye, and the end position
	// is the eye plus a long distance in the direction the mouse is pointing.
	OutWorldOrigin		= rayStartWorldSpace;
	OutWorldDirection	= SMath::NormalizeVector( rayDirWorldSpace );
}


CScene::~CScene()
{
	Clear();
}

void CScene::AddPrimitive( class CPrimitiveComponent* InPrimitive )
{
	check( InPrimitive );

	// If primitive already on scene
	if ( InPrimitive->scene == this )
	{
		return;
	}
	// Else if primitive on other scene - remove from old
	if ( InPrimitive->scene )
	{
		InPrimitive->scene->RemovePrimitive( InPrimitive );
	}

	InPrimitive->scene = this;
	InPrimitive->LinkDrawList();
	primitives.push_back( InPrimitive );
}

void CScene::RemovePrimitive( class CPrimitiveComponent* InPrimitive )
{
	for ( auto it = primitives.begin(), itEnd = primitives.end(); it != itEnd; ++it )
	{
		if ( *it == InPrimitive )
		{
			InPrimitive->UnlinkDrawList();
			InPrimitive->scene = nullptr;
			primitives.erase( it );
			return;
		}
	}
}

void CScene::Clear()
{
	for ( auto it = primitives.begin(), itEnd = primitives.end(); it != itEnd; ++it )
	{
		CPrimitiveComponent*		primitiveComponent = *it;
		primitiveComponent->UnlinkDrawList();
		primitiveComponent->scene = nullptr;
	}
	primitives.clear();
}

void CScene::BuildSDGs( const CSceneView& InSceneView )
{
	// Add to SDGs visible primitives
	for ( auto it = primitives.begin(), itEnd = primitives.end(); it != itEnd; ++it )
	{
		CPrimitiveComponent*		primitiveComponent = *it;
		if ( primitiveComponent->IsVisibility() && InSceneView.GetFrustum().IsIn( primitiveComponent->GetBoundBox() ) )
		{
			primitiveComponent->AddToDrawList( InSceneView );
		}
	}
}

void CScene::ClearSDGs()
{
	// Clear all instances in scene depth groups
	for ( uint32 index = 0; index < SDG_Max; ++index )
	{
		SDGs[ index ].Clear();
	}
}