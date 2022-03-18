#include "Components/SpriteComponent.h"
#include "Misc/CoreGlobals.h"
#include "System/Package.h"
#include "Render/Scene.h"
#include "Math/Math.h"
#include "Render/Shaders/BasePassShader.h"
#include "Render/Texture.h"

IMPLEMENT_CLASS( LSpriteComponent )

LSpriteComponent::LSpriteComponent()
    : type( ST_Rotating )
{
    SetRelativeRotation( FRotator( 90.f, 0.f, 0.f ) );
}

void LSpriteComponent::BeginPlay()
{
    Super::BeginPlay();

    // Find static mesh of quad for render sprite
    SetStaticMesh( ( FStaticMeshRef )GPackageManager->FindAsset( TEXT( "EngineMeshes:Plane_SM" ), AT_StaticMesh ) );
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
