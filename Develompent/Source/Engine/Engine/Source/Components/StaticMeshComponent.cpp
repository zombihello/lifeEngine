#include "Components/StaticMeshComponent.h"
#include "Render/Scene.h"

IMPLEMENT_CLASS( LStaticMeshComponent )

/**
 * @brief Struct of static mesh data for add to draw list
 */
struct FStaticMeshData
{
	/**
	 * @brief Constructor
	 * 
	 * @param InDrawPolicy Drawing policy
	 * @param InElements Array of mesh elements
	 * @param InPrimitiveType Primitive type
	 */
	FStaticMeshData( const FStaticMeshDrawPolicy& InDrawPolicy, const std::vector< FMeshBatchElement >& InElements, EPrimitiveType InPrimitiveType )
		: drawingPolicyLink( FMeshDrawList< FStaticMeshDrawPolicy >::FDrawingPolicyLink( InDrawPolicy ) )
		, elements( InElements )
	{
		drawingPolicyLink.meshBatch.primitiveType = InPrimitiveType;
	}

	FMeshDrawList< FStaticMeshDrawPolicy >::FDrawingPolicyLink			drawingPolicyLink;		/**< Drawing policy link */
	std::vector< FMeshBatchElement >									elements;				/**< Array of mesh elements */
};

LStaticMeshComponent::~LStaticMeshComponent()
{}

void LStaticMeshComponent::AddToDrawList( class FScene* InScene, const class FSceneView& InSceneView )
{
	if ( !staticMesh )
	{
		return;
	}

	// Generate mesh data for render
	std::vector< FStaticMeshData >			staticMeshDatas;
	{
		FVertexFactoryRef								vertexFactory = staticMesh->GetVertexFactory();
		FIndexBufferRHIRef								indexBuffer = staticMesh->GetIndexBufferRHI();
		const std::vector< FStaticMeshSurface >&		surfaces = staticMesh->GetSurfaces();
		const FMatrix                                   transformationMatrix = GetComponentTransform().ToMatrix();

		for ( uint32 indexSurface = 0, numSurfaces = ( uint32 )surfaces.size(); indexSurface < numSurfaces; ++indexSurface )
		{
			const FStaticMeshSurface&				surface = surfaces[ indexSurface ];

			std::vector< FMeshBatchElement >		meshElements;
            meshElements.push_back( FMeshBatchElement{ indexBuffer, surface.baseVertexIndex, surface.firstIndex, surface.numPrimitives, transformationMatrix } );
			staticMeshDatas.push_back( FStaticMeshData( FStaticMeshDrawPolicy( vertexFactory, materials[ surface.materialID ] ), meshElements, PT_TriangleList ) );
		}
	}

	// Add meshes to SDG_World (scene layer 'World')
	{
		FSceneDepthGroup&		SDG = InScene->GetSDG( SDG_World );
		for ( uint32 index = 0, count = staticMeshDatas.size(); index < count; ++index )
		{
			const FStaticMeshData&		data = staticMeshDatas[ index ];
			SDG.staticMeshDrawList.AddItem( data.drawingPolicyLink, data.elements );
		}
	}
}
