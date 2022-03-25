#include "Render/VertexFactory/GeneralVertexFactoryParams.h"
#include "Render/Scene.h"

FGeneralVertexShaderParameters::FGeneralVertexShaderParameters( bool InSupportsInstancing )
    : bSupportsInstancing( InSupportsInstancing )
{}

void FGeneralVertexShaderParameters::Bind( const class FShaderParameterMap& InParameterMap )
{
    if ( !bSupportsInstancing )
    {
        localToWorldMatrixParameter.Bind( InParameterMap, TEXT( "localToWorldMatrix" ) );
    }
}

void FGeneralVertexShaderParameters::Set( class FBaseDeviceContextRHI* InDeviceContextRHI, const class FVertexFactory* InVertexFactory ) const
{}

void FGeneralVertexShaderParameters::SetMesh( class FBaseDeviceContextRHI* InDeviceContextRHI, const struct FMeshBatch& InMesh, const class FVertexFactory* InVertexFactory, const class FSceneView* InView, uint32 InNumInstances /* = 1 */, uint32 InStartInstanceID /* = 0 */ ) const
{
    if ( !bSupportsInstancing )
    {
        check( InNumInstances == 1 );
        SetVertexShaderValue( InDeviceContextRHI, localToWorldMatrixParameter, InMesh.transformationMatrices[ InStartInstanceID ] );
    }
    else
    {
        check( InVertexFactory );
        InVertexFactory->SetupInstancing( InDeviceContextRHI, InMesh, InView, InNumInstances, InStartInstanceID );
    }
}
