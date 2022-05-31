#include "Render/VertexFactory/GeneralVertexFactoryParams.h"
#include "Render/Scene.h"

CGeneralVertexShaderParameters::CGeneralVertexShaderParameters( bool InSupportsInstancing )
    : bSupportsInstancing( InSupportsInstancing )
{}

void CGeneralVertexShaderParameters::Bind( const class CShaderParameterMap& InParameterMap )
{
    if ( !bSupportsInstancing )
    {
        localToWorldMatrixParameter.Bind( InParameterMap, TEXT( "localToWorldMatrix" ) );
    }
}

void CGeneralVertexShaderParameters::Set( class CBaseDeviceContextRHI* InDeviceContextRHI, const class CVertexFactory* InVertexFactory ) const
{}

void CGeneralVertexShaderParameters::SetMesh( class CBaseDeviceContextRHI* InDeviceContextRHI, const struct SMeshBatch& InMesh, const class CVertexFactory* InVertexFactory, const class CSceneView* InView, uint32 InNumInstances /* = 1 */, uint32 InStartInstanceID /* = 0 */ ) const
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
