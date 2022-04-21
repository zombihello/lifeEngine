#include "Components/SpriteComponent.h"
#include "Misc/CoreGlobals.h"
#include "System/Package.h"
#include "Render/Scene.h"
#include "Math/Math.h"
#include "Math/Rect.h"
#include "Render/Shaders/BasePassShader.h"
#include "Render/Texture.h"

IMPLEMENT_CLASS( LSpriteComponent )

LSpriteComponent::LSpriteComponent()
    : bIsDirtyDrawingPolicyLink( false )
	, bFlipVertical( false )
	, bFlipHorizontal( false )
    , scene( nullptr )
    , type( ST_Rotating )
	, sprite( new FSprite() )
    , meshBatchLink( nullptr )
{
	BeginInitResource( sprite );
}

void LSpriteComponent::Serialize( class FArchive& InArchive )
{
    Super::Serialize( InArchive );

    FRectFloat      textureRect     = GetTextureRect();
    FVector2D       spriteSize      = GetSpriteSize();
    FMaterialRef    material        = GetMaterial();

    InArchive << type;
    InArchive << textureRect;
    InArchive << spriteSize;
    InArchive << material;
	InArchive << bFlipVertical;
	InArchive << bFlipHorizontal;

    if ( InArchive.IsLoading() )
    {
        SetTextureRect( textureRect );
        SetSpriteSize( spriteSize );
        SetMaterial( material );
		SetFlipVertical( bFlipVertical );
		SetFlipHorizontal( bFlipHorizontal );
    }
}

void LSpriteComponent::CalcTransformationMatrix( const class FSceneView& InSceneView, FMatrix& OutResult ) const
{
    FTransform      transform = GetComponentTransform();
    if ( type == ST_Static )
    {
        transform.ToMatrix( OutResult );
		return;
    }

	FMath::IdentityMatrix( OutResult );
	FMath::TranslateMatrix( transform.GetLocation(), OutResult );
    
	const FMatrix&      viewMatrix = InSceneView.GetViewMatrix();
	OutResult[ 0 ].x = viewMatrix[ 0 ].x;
	OutResult[ 0 ].y = viewMatrix[ 1 ].x;
	OutResult[ 0 ].z = viewMatrix[ 2 ].x;

    if ( type == ST_RotatingOnlyVertical )
    {
		OutResult[ 1 ].x = 0;
		OutResult[ 1 ].y = 1;
		OutResult[ 1 ].z = 0;
    }
    else
    {
		OutResult[ 1 ].x = viewMatrix[ 0 ].y;
		OutResult[ 1 ].y = viewMatrix[ 1 ].y;
		OutResult[ 1 ].z = viewMatrix[ 2 ].y;
    }

	OutResult[ 2 ].x = viewMatrix[ 0 ].z;
	OutResult[ 2 ].y = viewMatrix[ 1 ].z;
	OutResult[ 2 ].z = viewMatrix[ 2 ].z;

	OutResult *= FMath::ScaleMatrix( transform.GetScale() );
	OutResult *= transform.GetRotation().ToMatrix();
}

void LSpriteComponent::LinkDrawList( class FScene* InScene )
{
    check( InScene );

	// If the primitive is already connected to another scene - remove the link 
	if ( scene && scene != InScene )
	{
		UnlinkDrawList();
	}

	// Memorize a new scene 
	scene = InScene;

    // If sprite is valid - add to scene draw policy link
    AddDrawingPolicyLink();
    bIsDirtyDrawingPolicyLink = false;
}

void LSpriteComponent::UnlinkDrawList()
{
	// If the primitive not connected to scene - exit from method
	if ( !scene )
	{
		return;
	}

	// If the primitive already added to scene - remove all draw policy links
	RemoveDrawingPolicyLink();
	bIsDirtyDrawingPolicyLink = false;

	// Forget the scene 
	scene = nullptr;
}

void LSpriteComponent::AddDrawingPolicyLink()
{
    check( scene );

	// If the primitive already added to scene - remove all draw policy links
	if ( drawingPolicyLink )
	{
		RemoveDrawingPolicyLink();
	}

	// If sprite is valid - add to scene draw policy link
	if ( sprite )
	{
		FSceneDepthGroup&               SDGWorld = scene->GetSDG( SDG_World );
		FSpriteSurface					surface = sprite->GetSurface();
		FDrawingPolicyLinkRef           tmpDrawPolicyLink = new FDrawingPolicyLink( FStaticMeshDrawPolicy( sprite->GetVertexFactory(), sprite->GetMaterial() ) );

		// Generate mesh batch of sprite
		FMeshBatch			            meshBatch;
		meshBatch.baseVertexIndex       = surface.baseVertexIndex;
		meshBatch.firstIndex            = surface.firstIndex;
		meshBatch.numPrimitives         = surface.numPrimitives;
		meshBatch.indexBufferRHI        = sprite->GetIndexBufferRHI();
		meshBatch.primitiveType         = PT_TriangleList;
		tmpDrawPolicyLink->meshBatchList.insert( meshBatch );

		// Add to new scene draw policy link
		drawingPolicyLink = SDGWorld.spriteDrawList.AddItem( tmpDrawPolicyLink );
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
}

void LSpriteComponent::RemoveDrawingPolicyLink()
{
    check( scene );

	// If the primitive already added to scene - remove all draw policy links
	if ( drawingPolicyLink )
	{
		FSceneDepthGroup&		SDGWorld = scene->GetSDG( SDG_World );
		SDGWorld.spriteDrawList.RemoveItem( drawingPolicyLink );

		drawingPolicyLink = nullptr;
		meshBatchLink = nullptr;
	}
}

void LSpriteComponent::AddToDrawList( const class FSceneView& InSceneView )
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

		if ( sprite )
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
    ++meshBatchLink->numInstances;
	meshBatchLink->transformationMatrices.resize( meshBatchLink->numInstances );
	CalcTransformationMatrix( InSceneView, meshBatchLink->transformationMatrices[ meshBatchLink->numInstances-1 ] );

    // Update AABB
    boundbox = FBox::BuildAABB( GetComponentLocation(), FVector( GetSpriteSize(), 1.f ) );
}
