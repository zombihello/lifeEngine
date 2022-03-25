#include "Components/StaticMeshComponent.h"
#include "Render/Scene.h"

IMPLEMENT_CLASS( LStaticMeshComponent )

LStaticMeshComponent::LStaticMeshComponent()
	: bIsDirtyDrawingPolicyLink( false )
	, scene( nullptr )
{}

LStaticMeshComponent::~LStaticMeshComponent()
{}

void LStaticMeshComponent::LinkDrawList( class FScene* InScene )
{
	check( InScene );

	// If the primitive is already connected to another scene - remove the link 
	if ( scene && scene != InScene )
	{
		UnlinkDrawList();
	}

	// Memorize a new scene 
	scene = InScene;

	// Add to scene draw policy link
	AddDrawingPolicyLink();
	bIsDirtyDrawingPolicyLink = false;
}

void LStaticMeshComponent::UnlinkDrawList()
{
	// If the primitive not connected to scene - exit from method
	if ( !scene )
	{
		return;
	}

	// Remove all draw policy links
	RemoveDrawingPolicyLink();
	bIsDirtyDrawingPolicyLink = false;

	// Forget the scene 
	scene = nullptr;
}

void LStaticMeshComponent::AddDrawingPolicyLink()
{
	check( scene );

	// If the primitive already added to scene - remove all draw policy links
	if ( !drawingPolicyLinks.empty() )
	{
		RemoveDrawingPolicyLink();
	}

	// If static mesh is valid - add to scene draw policy link
	if ( staticMesh )
	{
		FSceneDepthGroup&							SDGWorld = scene->GetSDG( SDG_World );
		FVertexFactoryRef							vertexFactory = staticMesh->GetVertexFactory();
		FIndexBufferRHIRef							indexBuffer = staticMesh->GetIndexBufferRHI();
		const std::vector< FStaticMeshSurface >&	surfaces = staticMesh->GetSurfaces();

		// Generate mesh batch for surface and add to new scene draw policy link
		for ( uint32 indexSurface = 0, numSurfaces = ( uint32 )surfaces.size(); indexSurface < numSurfaces; ++indexSurface )
		{
			const FStaticMeshSurface& surface = surfaces[ indexSurface ];
			FDrawingPolicyLinkRef           tmpDrawPolicyLink = new FDrawingPolicyLink( FStaticMeshDrawPolicy( vertexFactory, materials[ surface.materialID ] ) );

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

void LStaticMeshComponent::RemoveDrawingPolicyLink()
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
			AddDrawingPolicyLink();
		}
		else
		{
			RemoveDrawingPolicyLink();
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
