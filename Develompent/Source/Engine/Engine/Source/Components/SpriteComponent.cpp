#include "Actors/Actor.h"
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

		// Generate mesh batch of sprite
		SMeshBatch			            meshBatch;
		meshBatch.baseVertexIndex       = surface.baseVertexIndex;
		meshBatch.firstIndex            = surface.firstIndex;
		meshBatch.numPrimitives         = surface.numPrimitives;
		meshBatch.indexBufferRHI        = sprite->GetIndexBufferRHI();
		meshBatch.primitiveType         = PT_TriangleList;

		// Make and add to scene new draw policy link
		const SMeshBatch*				meshBatchLink = nullptr;
		drawingPolicyLink				= ::MakeDrawingPolicyLink<DrawingPolicyLink_t>( sprite->GetVertexFactory(), sprite->GetMaterial(), meshBatch, meshBatchLink, SDGWorld.spriteDrawList, DEC_SPRITE );
		meshBatchLinks.push_back( meshBatchLink );

		// Make and add to scene new hit proxy draw policy link
#if ENABLE_HITPROXY
		hitProxyDrawingPolicyLink		= ::MakeDrawingPolicyLink<HitProxyDrawingPolicyLink_t>( sprite->GetVertexFactory(), sprite->GetMaterial(), meshBatch, meshBatchLink, SDGWorld.hitProxyLayers[ HPL_World ].hitProxyDrawList, DEC_SPRITE );
		meshBatchLinks.push_back( meshBatchLink );
#endif // ENABLE_HITPROXY
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

#if ENABLE_HITPROXY
		SDGWorld.hitProxyLayers[ HPL_World ].hitProxyDrawList.RemoveItem( hitProxyDrawingPolicyLink );
#endif // ENABLE_HITPROXY

		drawingPolicyLink = nullptr;
		meshBatchLinks.clear();
	}
}

void CSpriteComponent::AddToDrawList( const class CSceneView& InSceneView )
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

	// Calculate transform matrix
	ActorRef_t	owner = GetOwner();
	Matrix		transformMatrix;
	CalcTransformationMatrix( InSceneView, transformMatrix );

    // Add to mesh batch new instance
	for ( uint32 index = 0, count = meshBatchLinks.size(); index < count; ++index )
	{
		const SMeshBatch*	meshBatchLink = meshBatchLinks[ index ];	
		++meshBatchLink->numInstances;
		meshBatchLink->instances.resize( meshBatchLink->numInstances );

		SMeshInstance&		instanceMesh = meshBatchLink->instances[ meshBatchLink->numInstances - 1 ];
		instanceMesh.transformMatrix	 = transformMatrix;

#if ENABLE_HITPROXY
		instanceMesh.hitProxyId		= owner ? owner->GetHitProxyId() : CHitProxyId();
#endif // ENABLE_HITPROXY

#if WITH_EDITOR
		instanceMesh.bSelected		= owner ? owner->IsSelected() : false;
#endif // WITH_EDITOR
	}

    // Update AABB
    boundbox = ÑBox::BuildAABB( GetComponentLocation(), Vector( GetSpriteSize(), 1.f ) );
}
