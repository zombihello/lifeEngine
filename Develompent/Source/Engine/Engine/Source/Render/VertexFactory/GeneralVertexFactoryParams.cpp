#include "Render/VertexFactory/GeneralVertexFactoryParams.h"
#include "Render/Scene.h"

void FGeneralVertexShaderParameters::Bind( const class FShaderParameterMap& InParameterMap )
{
    localToWorldMatrixParameter.Bind( InParameterMap, TEXT( "localToWorldMatrix" ) );
}

void FGeneralVertexShaderParameters::Set( class FBaseDeviceContextRHI* InDeviceContextRHI, const class FVertexFactory* InVertexFactory, const class FSceneView* InView ) const
{}

void FGeneralVertexShaderParameters::SetMesh( class FBaseDeviceContextRHI* InDeviceContextRHI, const struct FMeshBatch& InMesh, uint32 InBatchElementIndex, const class FSceneView* InView ) const
{
    const FMeshBatchElement&		batchElement = InMesh.elements[ InBatchElementIndex ];
    SetVertexShaderValue( InDeviceContextRHI, localToWorldMatrixParameter, batchElement.transformationMatrix );
}
