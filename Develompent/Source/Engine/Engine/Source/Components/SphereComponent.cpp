#include "Render/Sphere.h"
#include "Render/SceneUtils.h"
#include "Components/SphereComponent.h"

IMPLEMENT_CLASS( CSphereComponent )

CSphereComponent::CSphereComponent()
	: radius( 0.f )
	, SDGLevel( SDG_World )
	, pendingSDGLevel( SDG_World )
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
	if ( !bIsDirtyDrawingPolicyLink && meshBatchLinks.empty() )
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

	for ( uint32 index = 0, count = meshBatchLinks.size(); index < count; ++index )
	{
		const SMeshBatch*	meshBatchLink = meshBatchLinks[index];
		++meshBatchLink->numInstances;
		meshBatchLink->instances.push_back( SMeshInstance{ transform.ToMatrix() } );
	}
}

void CSphereComponent::LinkDrawList()
{
	check( scene );

	// If the primitive already added to scene - remove all draw policy links
	if ( drawingPolicyLink )
	{
		UnlinkDrawList();
	}

	SDGLevel = pendingSDGLevel;
	SSceneDepthGroup&				SDG = scene->GetSDG( SDGLevel );

	// Generate mesh batch of sprite
	SMeshBatch			            meshBatch;
	meshBatch.baseVertexIndex		= 0;
	meshBatch.firstIndex			= 0;
	meshBatch.numPrimitives			= GSphereMesh.GetNumPrimitives();
	meshBatch.indexBufferRHI		= GSphereMesh.GetIndexBufferRHI();
	meshBatch.primitiveType			= PT_TriangleList;
	
	// Make and add to scene new draw policy link	
	const SMeshBatch*				meshBatchLink = nullptr;
	drawingPolicyLink				= ::MakeDrawingPolicyLink<DrawingPolicyLink_t>( GSphereMesh.GetVertexFactory(), material, meshBatch, meshBatchLink, SDG.dynamicMeshElements, DEC_DYNAMICELEMENTS );
	meshBatchLinks.push_back( meshBatchLink );

	// Make and add to scene new depth draw policy link
	depthDrawingPolicyLink			= ::MakeDrawingPolicyLink<DepthDrawingPolicyLink_t>( GSphereMesh.GetVertexFactory(), material, meshBatch, meshBatchLink, SDG.depthDrawList, DEC_DYNAMICELEMENTS );
	meshBatchLinks.push_back( meshBatchLink );
}

void CSphereComponent::UnlinkDrawList()
{
	check( scene );
	SSceneDepthGroup&	SDG = scene->GetSDG( SDGLevel );

	// If the primitive already added to scene - remove all draw policy links
	if ( drawingPolicyLink  )
	{
		SDG.dynamicMeshElements.RemoveItem( drawingPolicyLink );		
	}

	if ( depthDrawingPolicyLink )
	{
		SDG.depthDrawList.RemoveItem( depthDrawingPolicyLink );
	}

	meshBatchLinks.clear();
}