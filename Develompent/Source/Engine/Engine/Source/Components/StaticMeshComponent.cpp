#include "Components/StaticMeshComponent.h"
#include "Render/Scene.h"

IMPLEMENT_CLASS( LStaticMeshComponent )

LStaticMeshComponent::~LStaticMeshComponent()
{}

void LStaticMeshComponent::AddToDrawList( class FScene* InScene, const class FSceneView& InSceneView )
{
	if ( !staticMesh )
	{
		return;
	}

	FSceneDepthGroup&								SDGWorld = InScene->GetSDG( SDG_World );
	FVertexFactoryRef								vertexFactory = staticMesh->GetVertexFactory();
	FIndexBufferRHIRef								indexBuffer = staticMesh->GetIndexBufferRHI();
	const std::vector< FStaticMeshSurface >&		surfaces = staticMesh->GetSurfaces();
	const FMatrix                                   transformationMatrix = GetComponentTransform().ToMatrix();

	for ( uint32 indexSurface = 0, numSurfaces = ( uint32 )surfaces.size(); indexSurface < numSurfaces; ++indexSurface )
	{
		const FStaticMeshSurface&											surface = surfaces[ indexSurface ];
		FStaticMeshDrawPolicy												meshDrawPolicy( vertexFactory, materials[ surface.materialID ] );
		FMeshDrawList< FStaticMeshDrawPolicy >::FDrawingPolicyLink			drawingPolicyLink( meshDrawPolicy );
		
		FMeshBatch			meshBatch;
		meshBatch.baseVertexIndex	= surface.baseVertexIndex;
		meshBatch.firstIndex		= surface.firstIndex;
		meshBatch.numPrimitives		= surface.numPrimitives;
		meshBatch.indexBufferRHI	= indexBuffer;
		meshBatch.numInstances		= 1;
		meshBatch.primitiveType		= PT_TriangleList;
		meshBatch.transformationMatrices.push_back( transformationMatrix );
		drawingPolicyLink.meshBatchList.insert( meshBatch );

		// Add meshes to SDG_World (scene layer 'World')
		SDGWorld.staticMeshDrawList.AddItem( drawingPolicyLink );
	}
}
