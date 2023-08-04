#include "Actors/Actor.h"
#include "System/World.h"
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

/*
==================
CSpriteComponent::CSpriteComponent
==================
*/
CSpriteComponent::CSpriteComponent()
#if WITH_EDITOR
	: bGizmo( false ),
#else
	:
#endif // WITH_EDITOR
	  bFlipVertical( false )
	, bFlipHorizontal( false )
    , type( ST_Rotating )
	, sprite( new CSprite() )
{
	BeginInitResource( sprite );
}

/*
==================
CSpriteComponent::StaticInitializeClass
==================
*/
void CSpriteComponent::StaticInitializeClass()
{
	new CBoolProperty( staticClass, TEXT( "bFlipVertical" ), TEXT( "Sprite" ), TEXT( "Is need flip sprite by vertical" ), CPP_PROPERTY( bFlipVertical ), 0 );
	new CFloatProperty( staticClass, TEXT( "bFlipHorizontal" ), TEXT( "Sprite" ), TEXT( "Is need flip sprite by horizontal" ), CPP_PROPERTY( bFlipHorizontal ), 0 );
	// TODO BS yehor.pohuliaka - Need implement CNameProperty for enums
}

/*
==================
CSpriteComponent::Serialize
==================
*/
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

/*
==================
CSpriteComponent::CalcTransformationMatrix
==================
*/
void CSpriteComponent::CalcTransformationMatrix( const class CSceneView& InSceneView, Matrix& OutResult ) const
{
	CTransform      transform( NoInit );
	
	// If current sprite is gizmo then we must ignore rotate and scale
#if WITH_EDITOR
	if ( bGizmo )
	{
		transform = CTransform( GetComponentLocation() );
	}
	else
#endif // WITH_EDITOR
	{
		transform = GetComponentTransform();
	}

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
	OutResult *= SMath::QuaternionToMatrix( transform.GetRotation() );
}

/*
==================
CSpriteComponent::LinkDrawList
==================
*/
void CSpriteComponent::LinkDrawList()
{
    Assert( scene );

	// If the primitive already added to scene - remove all draw policy links
	if ( drawingPolicyLink
#if WITH_EDITOR
		 || gizmoDrawingPolicyLink
#endif // WITH_EDITOR
		 )
	{
		UnlinkDrawList();
	}

	// If sprite is valid - add to scene draw policy link
	if ( sprite )
	{
		SSceneDepthGroup&               SDG = scene->GetSDG( 
#if WITH_EDITOR
			bGizmo ? SDG_WorldEdForeground :
#endif // WITH_EDITOR
			SDG_World );
		
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
#if WITH_EDITOR
		if ( bGizmo )
		{
			gizmoDrawingPolicyLink		= ::MakeDrawingPolicyLink<GizmoDrawingPolicyLink_t>( sprite->GetVertexFactory(), sprite->GetMaterial(), meshBatch, meshBatchLink, SDG.gizmoDrawList, DEC_SPRITE );
		}
		else
#endif // WITH_EDITOR
		{
			drawingPolicyLink			= ::MakeDrawingPolicyLink<DrawingPolicyLink_t>( sprite->GetVertexFactory(), sprite->GetMaterial(), meshBatch, meshBatchLink, SDG.spriteDrawList, DEC_SPRITE );
		}
		meshBatchLinks.push_back( meshBatchLink );

		// Make and add to the scene new depth draw policy link
		depthDrawingPolicyLink			= ::MakeDrawingPolicyLink<DepthDrawingPolicyLink_t>( sprite->GetVertexFactory(), sprite->GetMaterial(), meshBatch, meshBatchLink, SDG.depthDrawList, DEC_SPRITE );
		meshBatchLinks.push_back( meshBatchLink );

		// Make and add to scene new hit proxy draw policy link
#if ENABLE_HITPROXY
		hitProxyDrawingPolicyLink		= ::MakeDrawingPolicyLink<HitProxyDrawingPolicyLink_t>( sprite->GetVertexFactory(), sprite->GetMaterial(), meshBatch, meshBatchLink, SDG.hitProxyLayers[ HPL_World ].hitProxyDrawList, DEC_SPRITE );
		meshBatchLinks.push_back( meshBatchLink );
#endif // ENABLE_HITPROXY
	}
}

/*
==================
CSpriteComponent::UnlinkDrawList
==================
*/
void CSpriteComponent::UnlinkDrawList()
{
    Assert( scene );
	SSceneDepthGroup&		SDGWorld = scene->GetSDG( SDG_World );

	// If the primitive already added to scene - remove all draw policy links
	if ( drawingPolicyLink )
	{		
		SDGWorld.spriteDrawList.RemoveItem( drawingPolicyLink );
	}
#if WITH_EDITOR
	else if ( gizmoDrawingPolicyLink )
	{
		SDGWorld.gizmoDrawList.RemoveItem( gizmoDrawingPolicyLink );
	}
#endif // WITH_EDITOR

	if ( depthDrawingPolicyLink )
	{
		SDGWorld.depthDrawList.RemoveItem( depthDrawingPolicyLink );
	}

#if ENABLE_HITPROXY
	if ( hitProxyDrawingPolicyLink )
	{
		SDGWorld.hitProxyLayers[HPL_World].hitProxyDrawList.RemoveItem( hitProxyDrawingPolicyLink );
	}
#endif // ENABLE_HITPROXY

	meshBatchLinks.clear();
}

/*
==================
CSpriteComponent::AddToDrawList
==================
*/
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
	{
		Vector			minLocation = Vector( -1.f, -1.f, 0.f ) * Vector( GetSpriteSize() / 2.f, 1.f );
		Vector			maxLocation = Vector( 1.f, 1.f, 0.f ) * Vector( GetSpriteSize() / 2.f, 1.f );
		Vector			verteces[8] =
		{
			Vector{ minLocation.x, minLocation.y, minLocation.z },
			Vector{ maxLocation.x, minLocation.y, minLocation.z },
			Vector{ maxLocation.x, maxLocation.y, minLocation.z },
			Vector{ minLocation.x, maxLocation.y, minLocation.z },
			Vector{ minLocation.x, minLocation.y, maxLocation.z },
			Vector{ maxLocation.x, minLocation.y, maxLocation.z },
			Vector{ maxLocation.x, maxLocation.y, maxLocation.z },
			Vector{ minLocation.x, maxLocation.y, maxLocation.z },
		};

		minLocation = maxLocation = GetComponentRotation() * ( GetComponentScale() * verteces[0] );
		for ( uint32 index = 0; index < 7; ++index )
		{
			Vector		vertex = GetComponentRotation() * ( GetComponentScale() * verteces[index] );
			if ( minLocation.x > vertex.x )
			{
				minLocation.x = vertex.x;
			}
			if ( minLocation.y > vertex.y )
			{
				minLocation.y = vertex.y;
			}
			if ( minLocation.z > vertex.z )
			{
				minLocation.z = vertex.z;
			}

			if ( maxLocation.x < vertex.x )
			{
				maxLocation.x = vertex.x;
			}
			if ( maxLocation.y < vertex.y )
			{
				maxLocation.y = vertex.y;
			}
			if ( maxLocation.z < vertex.z )
			{
				maxLocation.z = vertex.z;
			}
		}

		boundbox = CBox::BuildAABB( GetComponentLocation(), minLocation, maxLocation );
	}

	// Draw wireframe box if owner actor is selected (only for WorldEd)
#if WITH_EDITOR
	if ( !bGizmo && owner ? owner->IsSelected() : false )
	{
		DrawWireframeBox( ( ( CScene* )g_World->GetScene() )->GetSDG( SDG_WorldEdForeground ), boundbox, DEC_SPRITE );
	}
#endif // WITH_EDITOR
}
