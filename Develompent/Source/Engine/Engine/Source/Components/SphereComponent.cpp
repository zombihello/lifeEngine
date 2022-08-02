#include "Render/Sphere.h"
#include "Render/SceneUtils.h"
#include "Components/SphereComponent.h"

IMPLEMENT_CLASS( CSphereComponent )

CSphereComponent::CSphereComponent()
	: radius( 0.f )
	, SDGLevel( SDG_World )
	, pendingSDGLevel( SDG_World )
	, meshBatchLink( nullptr )
{}

void CSphereComponent::Serialize( class CArchive& InArchive )
{
	Super::Serialize( InArchive );
	InArchive << radius;
	InArchive << pendingSDGLevel;
	InArchive << material;
}

void CSphereComponent::UpdateBodySetup()
{}

void CSphereComponent::AddToDrawList( const class CSceneView& InSceneView )
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
	CTransform				transform = GetComponentTransform();
	transform.SetScale( Vector( radius, radius, radius ) );

	++meshBatchLink->numInstances;
	meshBatchLink->instances.push_back( SMeshInstance{ transform.ToMatrix() } );
}

void CSphereComponent::LinkDrawList()
{
	check( scene );

	// If the primitive already added to scene - remove all draw policy links
	if ( drawingPolicyLink )
	{
		UnlinkDrawList();
	}

	// If sprite is valid - add to scene draw policy link
	SDGLevel = pendingSDGLevel;
	SSceneDepthGroup&				SDG = scene->GetSDG( SDGLevel );
	DrawingPolicyLinkRef_t           tmpDrawPolicyLink = new DrawingPolicyLink_t( DEC_DYNAMICELEMENTS );
	tmpDrawPolicyLink->drawingPolicy.Init( GSphereMesh.GetVertexFactory(), material );

	// Generate mesh batch of sprite
	SMeshBatch			            meshBatch;
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
	MeshBatchList_t::iterator        itMeshBatchLink = drawingPolicyLink->meshBatchList.find( meshBatch );
	if ( itMeshBatchLink != drawingPolicyLink->meshBatchList.end() )
	{
		meshBatchLink = &( *itMeshBatchLink );
	}
	else
	{
		meshBatchLink = &( *drawingPolicyLink->meshBatchList.insert( meshBatch ).first );
	}
}

void CSphereComponent::UnlinkDrawList()
{
	check( scene );

	// If the primitive already added to scene - remove all draw policy links
	if ( drawingPolicyLink )
	{
		SSceneDepthGroup&		SDG = scene->GetSDG( SDGLevel );
		SDG.dynamicMeshElements.RemoveItem( drawingPolicyLink );

		drawingPolicyLink	= nullptr;
		meshBatchLink		= nullptr;
	}
}