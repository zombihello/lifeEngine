#include "Render/Scene.h"

FSceneView::FSceneView( const FMatrix& InProjectionMatrix, const FMatrix& InViewMatrix, float InSizeX, float InSizeY, const FColor& InBackgroundColor, EShowFlags InShowFlags )
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


void FScene::AddPrimitive( class LPrimitiveComponent* InPrimitive )
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

void FScene::RemovePrimitive( class LPrimitiveComponent* InPrimitive )
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

void FScene::Clear()
{
	for ( auto it = primitives.begin(), itEnd = primitives.end(); it != itEnd; ++it )
	{
		LPrimitiveComponent*		primitiveComponent = *it;
		primitiveComponent->UnlinkDrawList();
		primitiveComponent->scene = nullptr;
	}
	primitives.clear();
}

void FScene::BuildSDGs( const FSceneView& InSceneView )
{
	// Add to SDGs visible primitives
	for ( auto it = primitives.begin(), itEnd = primitives.end(); it != itEnd; ++it )
	{
		LPrimitiveComponent*		primitiveComponent = *it;
		if ( primitiveComponent->IsVisibility() && InSceneView.GetFrustum().IsIn( primitiveComponent->GetBoundBox() ) )
		{
			primitiveComponent->AddToDrawList( InSceneView );
		}
	}
}

void FScene::ClearSDGs()
{
	// Clear all instances in scene depth groups
	for ( uint32 index = 0; index < SDG_Max; ++index )
	{
		SDGs[ index ].Clear();
	}
}