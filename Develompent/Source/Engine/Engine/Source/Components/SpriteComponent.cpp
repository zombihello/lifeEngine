#include "Components/SpriteComponent.h"
#include "Misc/CoreGlobals.h"
#include "System/Package.h"
#include "Render/Scene.h"
#include "Render/SceneUtils.h"
#include "Math/Math.h"
#include "Math/Rect.h"
#include "Render/Shaders/BasePassShader.h"
#include "Render/Texture.h"

IMPLEMENT_CLASS( CSpriteComponent )

CSpriteComponent::CSpriteComponent()
    : bFlipVertical( false )
	, bFlipHorizontal( false )
    , type( ST_Rotating )
	, sprite( new CSprite() )
    , meshBatchLink( nullptr )
{
	BeginInitResource( sprite );
}

void CSpriteComponent::Serialize( class CArchive& InArchive )
{
    Super::Serialize( InArchive );

    RectFloat_t				textureRect     = GetTextureRect();
    Vector2D				spriteSize      = GetSpriteSize();
	TAssetHandle<CMaterial>	material        = GetMaterial();

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

void CSpriteComponent::CalcTransformationMatrix( const class CSceneView& InSceneView, Matrix& OutResult ) const
{
    CTransform      transform = GetComponentTransform();
    if ( type == ST_Static )
    {
        transform.ToMatrix( OutResult );
		return;
    }

	SMath::IdentityMatrix( OutResult );
	SMath::TranslateMatrix( transform.GetLocation(), OutResult );
    
	const Matrix&      viewMatrix = InSceneView.GetViewMatrix();
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

	OutResult *= SMath::ScaleMatrix( transform.GetScale() );
	OutResult *= transform.GetRotation().ToMatrix();
}

void CSpriteComponent::LinkDrawList()
{
    check( scene );

	// If the primitive already added to scene - remove all draw policy links
	if ( drawingPolicyLink )
	{
		UnlinkDrawList();
	}

	// If sprite is valid - add to scene draw policy link
	if ( sprite )
	{
		SSceneDepthGroup&               SDGWorld = scene->GetSDG( SDG_World );
		SSpriteSurface					surface = sprite->GetSurface();
		DrawingPolicyLinkRef_t           tmpDrawPolicyLink = new DrawingPolicyLink_t( DEC_SPRITE );
		tmpDrawPolicyLink->drawingPolicy.Init( sprite->GetVertexFactory(), sprite->GetMaterial() );

		// Generate mesh batch of sprite
		SMeshBatch			            meshBatch;
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
}

void CSpriteComponent::UnlinkDrawList()
{
    check( scene );

	// If the primitive already added to scene - remove all draw policy links
	if ( drawingPolicyLink )
	{
		SSceneDepthGroup&		SDGWorld = scene->GetSDG( SDG_World );
		SDGWorld.spriteDrawList.RemoveItem( drawingPolicyLink );

		drawingPolicyLink = nullptr;
		meshBatchLink = nullptr;
	}
}

void CSpriteComponent::AddToDrawList( const class CSceneView& InSceneView )
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
			LinkDrawList();
		}
		else
		{
			UnlinkDrawList();
			return;
		}	
	}

    // Add to mesh batch new instance
    ++meshBatchLink->numInstances;
	meshBatchLink->transformationMatrices.resize( meshBatchLink->numInstances );
	CalcTransformationMatrix( InSceneView, meshBatchLink->transformationMatrices[ meshBatchLink->numInstances-1 ] );

    // Update AABB
    boundbox = ÑBox::BuildAABB( GetComponentLocation(), Vector( GetSpriteSize(), 1.f ) );
}
