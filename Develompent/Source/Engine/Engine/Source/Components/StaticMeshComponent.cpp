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
	if ( !drawingPolicyLinks.empty() )
	{
		UnlinkDrawList();
	}

	// If static mesh is valid - add to scene draw policy link
	TSharedPtr<FStaticMesh>		staticMeshRef = staticMesh.Pin();
	if ( staticMeshRef )
	{
		FSceneDepthGroup&							SDGWorld = scene->GetSDG( SDG_World );
		FVertexFactoryRef							vertexFactory = staticMeshRef->GetVertexFactory();
		FIndexBufferRHIRef							indexBuffer = staticMeshRef->GetIndexBufferRHI();
		const std::vector< FStaticMeshSurface >&	surfaces = staticMeshRef->GetSurfaces();

		// Generate mesh batch for surface and add to new scene draw policy link
		for ( uint32 indexSurface = 0, numSurfaces = ( uint32 )surfaces.size(); indexSurface < numSurfaces; ++indexSurface )
		{
			const FStaticMeshSurface& surface = surfaces[ indexSurface ];
			FDrawingPolicyLinkRef           tmpDrawPolicyLink = new FDrawingPolicyLink( DEC_STATIC_MESH );
			tmpDrawPolicyLink->drawingPolicy.Init( vertexFactory, materials[ surface.materialID ] );

			// Generate mesh batch of surface
			FMeshBatch					meshBatch;
			meshBatch.baseVertexIndex	= surface.baseVertexIndex;
			meshBatch.firstIndex		= surface.firstIndex;
			meshBatch.numPrimitives		= surface.numPrimitives;
			meshBatch.indexBufferRHI	= indexBuffer;
			meshBatch.primitiveType		= PT_TriangleList;
			tmpDrawPolicyLink->meshBatchList.insert( meshBatch );

			// Add to new scene draw policy link
			FDrawingPolicyLinkRef		drawingPolicyLink = SDGWorld.staticMeshDrawList.AddItem( tmpDrawPolicyLink );
			check( drawingPolicyLink );
			drawingPolicyLinks.push_back( drawingPolicyLink );

			// Get link to mesh batch. If not founded insert new
			FMeshBatchList::iterator        itMeshBatchLink = drawingPolicyLink->meshBatchList.find( meshBatch );
			if ( itMeshBatchLink != drawingPolicyLink->meshBatchList.end() )
			{
				meshBatchLinks.push_back( &( *itMeshBatchLink ) );
			}
			else
			{
				const FMeshBatch*		meshBatchLink = &( *drawingPolicyLink->meshBatchList.insert( meshBatch ).first );
				meshBatchLinks.push_back( meshBatchLink );
			}
		}
	}
}

void LStaticMeshComponent::UnlinkDrawList()
{
	check( scene );

	// If the primitive already added to scene - remove all draw policy links
	if ( !drawingPolicyLinks.empty() )
	{
		FSceneDepthGroup&		SDGWorld = scene->GetSDG( SDG_World );
		for ( uint32 index = 0, count = drawingPolicyLinks.size(); index < count; ++index )
		{
			SDGWorld.staticMeshDrawList.RemoveItem( drawingPolicyLinks[ index ] );
		}

		drawingPolicyLinks.clear();
		meshBatchLinks.clear();
	}
}

void LStaticMeshComponent::AddToDrawList( const class FSceneView& InSceneView )
{
	// If primitive is empty - exit from method
	if ( !bIsDirtyDrawingPolicyLink && meshBatchLinks.empty() )
	{
		return;
	}

	// If drawing policy link is dirty - we update it
	if ( bIsDirtyDrawingPolicyLink )
	{
		bIsDirtyDrawingPolicyLink = false;

		if ( staticMesh )
		{
			LinkDrawList();
		}
		else
		{
			UnlinkDrawList();
			return;
		}
	}

	// Add to mesh batch new instance
	const FMatrix          transformationMatrix = GetComponentTransform().ToMatrix();
	for ( uint32 index = 0, count = meshBatchLinks.size(); index < count; ++index )
	{
		const FMeshBatch*		meshBatch = meshBatchLinks[ index ];
		++meshBatch->numInstances;
		meshBatch->transformationMatrices.push_back( transformationMatrix );
	}
}
