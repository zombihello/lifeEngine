#include "Render/Sphere.h"
#include "Render/SceneUtils.h"
#include "Components/SphereComponent.h"

IMPLEMENT_CLASS( LSphereComponent )

LSphereComponent::LSphereComponent()
	: radius( 0.f )
	, SDGLevel( SDG_World )
	, pendingSDGLevel( SDG_World )
	, meshBatchLink( nullptr )
{}

void LSphereComponent::Serialize( class FArchive& InArchive )
{
	Super::Serialize( InArchive );
	InArchive << radius;
	InArchive << pendingSDGLevel;
	InArchive << material;
}

void LSphereComponent::UpdateBodySetup()
{}

void LSphereComponent::AddToDrawList( const class FSceneView& InSceneView )
{
	// If primitive is empty - exit from method
	if ( !bIsDirtyDrawingPolicyLink && !meshBatchLink )
	{
		return;
	}

	// If drawing policy link is dirty - we update it
	if ( bIsDirtyDrawingPolicyLink )
	{
		bIsDirtyDrawingPolicyLink = false;
		LinkDrawList();
	}

	// Add to mesh batch new instance
	FTransform				transform = GetComponentTransform();
	transform.SetScale( FVector( radius, radius, radius ) );

	++meshBatchLink->numInstances;
	meshBatchLink->transformationMatrices.push_back( transform.ToMatrix() );
}

void LSphereComponent::LinkDrawList()
{
	check( scene );

	// If the primitive already added to scene - remove all draw policy links
	if ( drawingPolicyLink )
	{
		UnlinkDrawList();
	}

	// If sprite is valid - add to scene draw policy link
	SDGLevel = pendingSDGLevel;
	FSceneDepthGroup&				SDG = scene->GetSDG( SDGLevel );
	FDrawingPolicyLinkRef           tmpDrawPolicyLink = new FDrawingPolicyLink( DEC_DYNAMICELEMENTS );
	tmpDrawPolicyLink->drawingPolicy.Init( GSphereMesh.GetVertexFactory(), material );

	// Generate mesh batch of sprite
	FMeshBatch			            meshBatch;
	meshBatch.baseVertexIndex		= 0;
	meshBatch.firstIndex			= 0;
	meshBatch.numPrimitives			= GSphereMesh.GetNumPrimitives();
	meshBatch.indexBufferRHI		= GSphereMesh.GetIndexBufferRHI();
	meshBatch.primitiveType			= PT_TriangleList;
	tmpDrawPolicyLink->meshBatchList.insert( meshBatch );

	// Add to new scene draw policy link
	drawingPolicyLink = SDG.dynamicMeshElements.AddItem( tmpDrawPolicyLink );
	check( drawingPolicyLink );

	// Get link to mesh batch. If not founded insert new
	FMeshBatchList::iterator        itMeshBatchLink = drawingPolicyLink->meshBatchList.find( meshBatch );
	if ( itMeshBatchLink != drawingPolicyLink->meshBatchList.end() )
	{
		meshBatchLink = &( *itMeshBatchLink );
	}
	else
	{
		meshBatchLink = &( *drawingPolicyLink->meshBatchList.insert( meshBatch ).first );
	}
}

void LSphereComponent::UnlinkDrawList()
{
	check( scene );

	// If the primitive already added to scene - remove all draw policy links
	if ( drawingPolicyLink )
	{
		FSceneDepthGroup&		SDG = scene->GetSDG( SDGLevel );
		SDG.dynamicMeshElements.RemoveItem( drawingPolicyLink );

		drawingPolicyLink	= nullptr;
		meshBatchLink		= nullptr;
	}
}