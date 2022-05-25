#include "Components/StaticMeshComponent.h"
#include "Render/Scene.h"
#include "Render/SceneUtils.h"

IMPLEMENT_CLASS( LStaticMeshComponent )

LStaticMeshComponent::LStaticMeshComponent()
{}

LStaticMeshComponent::~LStaticMeshComponent()
{}

void LStaticMeshComponent::LinkDrawList()
{
	check( scene );

	// If the primitive already added to scene - remove all draw policy links
	if ( elementDrawingPolicyLink )
	{
		UnlinkDrawList();
	}

	// If static mesh is valid - add to scene draw policy link
	TSharedPtr<FStaticMesh>		staticMeshRef = staticMesh.ToSharedPtr();
	if ( staticMeshRef )
	{
		elementDrawingPolicyLink = staticMeshRef->LinkDrawList( scene->GetSDG( SDG_World ), overrideMaterials );
	}
}

void LStaticMeshComponent::UnlinkDrawList()
{
	check( scene );

	// If the primitive already added to scene - remove all draw policy links
	if ( elementDrawingPolicyLink )
	{
		TSharedPtr<FStaticMesh>		staticMeshRef = staticMesh.ToSharedPtr();
		if ( staticMeshRef )
		{
			staticMeshRef->UnlinkDrawList( scene->GetSDG( SDG_World ), elementDrawingPolicyLink );
		}
		else
		{
			elementDrawingPolicyLink.Reset();
		}
	}
}

void LStaticMeshComponent::AddToDrawList( const class FSceneView& InSceneView )
{
	// If primitive is empty - exit from method
	if ( !bIsDirtyDrawingPolicyLink && !elementDrawingPolicyLink )
	{
		return;
	}

	// If drawing policy link is dirty - we update it
	if ( bIsDirtyDrawingPolicyLink || elementDrawingPolicyLink->bDirty )
	{
		bIsDirtyDrawingPolicyLink = false;
		
		LinkDrawList();
		if ( !staticMesh.IsAssetValid() )
		{
			return;
		}
	}

	// Add to mesh batch new instance
	const FMatrix				transformationMatrix = GetComponentTransform().ToMatrix();
	for ( uint32 index = 0, count = elementDrawingPolicyLink->meshBatchLinks.size(); index < count; ++index )
	{
		const FMeshBatch*		meshBatch = elementDrawingPolicyLink->meshBatchLinks[ index ];
		++meshBatch->numInstances;
		meshBatch->transformationMatrices.push_back( transformationMatrix );
	}
}
