#include "Render/Scene.h"

FSceneView::FSceneView( const FMatrix& InProjectionMatrix, const FMatrix& InViewMatrix, const FColor& InBackgroundColor, EShowFlags InShowFlags )
	: viewMatrix( InViewMatrix )
	, projectionMatrix( InProjectionMatrix )
	, viewProjectionMatrix( InProjectionMatrix * InViewMatrix )
	, backgroundColor( InBackgroundColor )
	, showFlags( InShowFlags )
{
	frustum.Update( viewProjectionMatrix );
}


void FScene::AddPrimitive( class LPrimitiveComponent* InPrimitive )
{
	check( InPrimitive );

	InPrimitive->LinkDrawList( this );
	primitives.push_back( InPrimitive );
}

void FScene::RemovePrimitive( class LPrimitiveComponent* InPrimitive )
{
	for ( auto it = primitives.begin(), itEnd = primitives.end(); it != itEnd; ++it )
	{
		if ( *it == InPrimitive )
		{
			InPrimitive->UnlinkDrawList();
			primitives.erase( it );
			return;
		}
	}
}

void FScene::BuildSDGs( const FSceneView& InSceneView )
{
	// Clear scene depth groups
	for ( uint32 index = 0; index < SDG_Max; ++index )
	{
		SDGs[ index ].Clear();
	}

	// Add to SDGs primitives
	for ( auto it = primitives.begin(), itEnd = primitives.end(); it != itEnd; ++it )
	{
		if ( InSceneView.GetFrustum().IsIn( ( *it )->GetBoundBox() ) )
		{
			( *it )->AddToDrawList( InSceneView );
		}
	}
}
