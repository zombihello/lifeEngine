#include "Render/VertexFactory/GeneralVertexFactoryParams.h"
#include "Render/Scene.h"

#if WITH_EDITOR
    #include "Misc/WorldEdGlobals.h"
    #include "System/EditorEngine.h"
#endif // WITH_EDITOR

CGeneralVertexShaderParameters::CGeneralVertexShaderParameters( bool InSupportsInstancing )
    : bSupportsInstancing( InSupportsInstancing )
{}

void CGeneralVertexShaderParameters::Bind( const class CShaderParameterMap& InParameterMap )
{
    if ( !bSupportsInstancing )
    {
        localToWorldMatrixParameter.Bind( InParameterMap, TEXT( "localToWorldMatrix" ) );

#if WITH_EDITOR
        colorOverlayParameter.Bind( InParameterMap, TEXT( "colorOverlay" ), true );
#endif // WITH_EDITOR
    }
}

void CGeneralVertexShaderParameters::Set( class CBaseDeviceContextRHI* InDeviceContextRHI, const class CVertexFactory* InVertexFactory ) const
{}

void CGeneralVertexShaderParameters::SetMesh( class CBaseDeviceContextRHI* InDeviceContextRHI, const struct SMeshBatch& InMesh, const class CVertexFactory* InVertexFactory, const class CSceneView* InView, uint32 InNumInstances /* = 1 */, uint32 InStartInstanceID /* = 0 */ ) const
{
    if ( !bSupportsInstancing )
    {
        check( InNumInstances == 1 );
        const SMeshInstance&        meshInstance = InMesh.instances[ InStartInstanceID ];
        SetVertexShaderValue( InDeviceContextRHI, localToWorldMatrixParameter, meshInstance.transformMatrix );
       
#if WITH_EDITOR
        if ( meshInstance.bSelected )
        {
            SetVertexShaderValue( InDeviceContextRHI, colorOverlayParameter, GEditorEngine->GetSelectionColor().ToNormalizedVector4D() );
        }
        else
        {
			SetVertexShaderValue( InDeviceContextRHI, colorOverlayParameter, Vector4D( 0.f, 0.f, 0.f, 0.f ) );
        }
#endif // WITH_EDITOR
    }
    else
    {
        check( InVertexFactory );
        InVertexFactory->SetupInstancing( InDeviceContextRHI, InMesh, InView, InNumInstances, InStartInstanceID );
    }
}
