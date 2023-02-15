#include "Actors/Actor.h"
#include "System/World.h"
#include "Render/Scene.h"
#include "Components/StaticMeshComponent.h"
#include "Render/Scene.h"
#include "Render/SceneUtils.h"

IMPLEMENT_CLASS( CStaticMeshComponent )

CStaticMeshComponent::CStaticMeshComponent()
{}

CStaticMeshComponent::~CStaticMeshComponent()
{}

void CStaticMeshComponent::Serialize( class CArchive& InArchive )
{
	Super::Serialize( InArchive );
	InArchive << staticMesh;
	InArchive << overrideMaterials;
}

void CStaticMeshComponent::LinkDrawList()
{
	check( scene );

	// If the primitive already added to scene - remove all draw policy links
	if ( elementDrawingPolicyLink )
	{
		UnlinkDrawList();
	}

	// If static mesh is valid - add to scene draw policy link
	TSharedPtr<CStaticMesh>		staticMeshRef = staticMesh.ToSharedPtr();
	if ( staticMeshRef )
	{
		elementDrawingPolicyLink = staticMeshRef->LinkDrawList( scene->GetSDG( SDG_World ), overrideMaterials );
	}
}

void CStaticMeshComponent::UnlinkDrawList()
{
	check( scene );

	// If the primitive already added to scene - remove all draw policy links
	if ( elementDrawingPolicyLink )
	{
		TSharedPtr<CStaticMesh>		staticMeshRef = staticMesh.ToSharedPtr();
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

void CStaticMeshComponent::AddToDrawList( const class CSceneView& InSceneView )
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

	AActor*		owner = GetOwner();

	// Add to mesh batch new instance
	const Matrix				transformationMatrix = GetComponentTransform().ToMatrix();
	for ( uint32 index = 0, count = elementDrawingPolicyLink->meshBatchLinks.size(); index < count; ++index )
	{
		const SMeshBatch*		meshBatch = elementDrawingPolicyLink->meshBatchLinks[ index ];
		++meshBatch->numInstances;
		meshBatch->instances.push_back( SMeshInstance{ transformationMatrix 
#if ENABLE_HITPROXY
										, owner ? owner->GetHitProxyId() : CHitProxyId()
#endif // ENABLE_HITPROXY

#if WITH_EDITOR
										, owner? owner->IsSelected() : false
#endif // WITH_EDITOR
										} );
	}

	// Build AABB
	TSharedPtr<CStaticMesh>		staticMeshRef = staticMesh.ToSharedPtr();
	if ( staticMeshRef )
	{
		Vector			minLocation = staticMeshRef->GetBoundingBox().GetMin();
		Vector			maxLocation = staticMeshRef->GetBoundingBox().GetMax();
		Vector			verteces[8] =
		{
			Vector{ minLocation.x, minLocation.y, minLocation.z },
			Vector{ maxLocation.x, minLocation.y, minLocation.z },
			Vector{ maxLocation.x, maxLocation.y, minLocation.z },
			Vector{ minLocation.x, maxLocation.y, minLocation.z },
			Vector{ minLocation.x, minLocation.y, maxLocation.z },
			Vector{ maxLocation.x, minLocation.y, maxLocation.z },
			Vector{ maxLocation.x, maxLocation.y, maxLocation.z },
			Vector{ minLocation.x, maxLocation.y, maxLocation.z },
		};

		minLocation = maxLocation	= GetComponentRotation() * ( GetComponentScale() * verteces[0] );	
		for ( uint32 index = 0; index < 7; ++index )
		{
			Vector		vertex		= GetComponentRotation() * ( GetComponentScale() * verteces[index] );
			if ( minLocation.x > vertex.x )
			{
				minLocation.x = vertex.x;
			}
			if ( minLocation.y > vertex.y )
			{
				minLocation.y = vertex.y;
			}
			if ( minLocation.z > vertex.z )
			{
				minLocation.z = vertex.z;
			}

			if ( maxLocation.x < vertex.x )
			{
				maxLocation.x = vertex.x;
			}
			if ( maxLocation.y < vertex.y )
			{
				maxLocation.y = vertex.y;
			}
			if ( maxLocation.z < vertex.z )
			{
				maxLocation.z = vertex.z;
			}
		}

		boundbox = CBox::BuildAABB( GetComponentLocation(), minLocation, maxLocation );

		// Draw wireframe box if owner actor is selected (only for WorldEd)
#if WITH_EDITOR
		if ( owner ? owner->IsSelected() : false )
		{
			DrawWireframeBox( ( ( CScene* )GWorld->GetScene() )->GetSDG( SDG_Highlight ), boundbox, DEC_STATIC_MESH );
		}
#endif // WITH_EDITOR
	}
}
