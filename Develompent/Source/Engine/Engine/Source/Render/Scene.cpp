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