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
    : type( ST_Rotating )
	, sprite( new FSprite() )
{
	BeginInitResource( sprite );
}

void LSpriteComponent::BeginPlay()
{
    Super::BeginPlay();
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

    if ( InArchive.IsLoading() )
    {
        SetTextureRect( textureRect );
        SetSpriteSize( spriteSize );
        SetMaterial( material );
    }
}

FMatrix LSpriteComponent::CalcTransformationMatrix( const class FSceneView& InSceneView ) const
{
    FTransform      transform = GetComponentTransform();
    if ( type == ST_Static )
    {
        return transform.ToMatrix();
    }

    FMatrix             resultMatrix = FMath::CreateTranslateMatrix( transform.GetLocation() );
    const FMatrix&      viewMatrix = InSceneView.GetViewMatrix();

    resultMatrix[ 0 ].x = viewMatrix[ 0 ].x;
    resultMatrix[ 0 ].y = viewMatrix[ 1 ].x;
    resultMatrix[ 0 ].z = viewMatrix[ 2 ].x;

    if ( type == ST_RotatingOnlyVertical )
    {
        resultMatrix[ 1 ].x = 0;
        resultMatrix[ 1 ].y = 1;
        resultMatrix[ 1 ].z = 0;
    }
    else
    {
        resultMatrix[ 1 ].x = viewMatrix[ 0 ].y;
        resultMatrix[ 1 ].y = viewMatrix[ 1 ].y;
        resultMatrix[ 1 ].z = viewMatrix[ 2 ].y;
    }

    resultMatrix[ 2 ].x = viewMatrix[ 0 ].z;
    resultMatrix[ 2 ].y = viewMatrix[ 1 ].z;
    resultMatrix[ 2 ].z = viewMatrix[ 2 ].z;

    resultMatrix *= FMath::CreateScaleMatrix( transform.GetScale() );
    resultMatrix *= transform.GetRotation().ToMatrix();
    return resultMatrix;
}

void LSpriteComponent::AddToDrawList( class FScene* InScene, const class FSceneView& InSceneView )
{
    if ( !sprite )
    {
        return;
    }

    FSceneDepthGroup&                                               SDGWorld = InScene->GetSDG( SDG_World );
    FSpriteSurface													surface = sprite->GetSurface();
	FMeshDrawList< FStaticMeshDrawPolicy >::FDrawingPolicyLink		drawPolicyLink( FStaticMeshDrawPolicy( sprite->GetVertexFactory(), sprite->GetMaterial() ) );

	FMeshBatch			            meshBatch;
	meshBatch.baseVertexIndex       = surface.baseVertexIndex;
	meshBatch.firstIndex            = surface.firstIndex;
	meshBatch.numPrimitives         = surface.numPrimitives;
	meshBatch.indexBufferRHI        = sprite->GetIndexBufferRHI();
	meshBatch.numInstances          = 1;
	meshBatch.primitiveType         = PT_TriangleList;
	meshBatch.transformationMatrices.push_back( CalcTransformationMatrix( InSceneView ) );
    drawPolicyLink.meshBatchList.insert( meshBatch );

    // Add meshes to SDG_World (scene layer 'World')
    SDGWorld.spriteDrawList.AddItem( drawPolicyLink );

    // Update AABB
    boundbox = FBox::BuildAABB( GetComponentLocation(), FVector( GetSpriteSize(), 1.f ) );
}
