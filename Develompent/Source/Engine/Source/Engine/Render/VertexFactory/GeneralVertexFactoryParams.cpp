/**
 * ************************************************************
 *                  This file is part of:
 *                      LIFEENGINE
 *          https://github.com/zombihello/lifeEngine
 * ************************************************************
 * Copyright (C) 2024 Yehor Pohuliaka.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "Engine/Render/VertexFactory/GeneralVertexFactoryParams.h"
#include "Engine/Render/Scene.h"

#if WITH_EDITOR
    #include "Editor/Misc/EditorGlobals.h"
    #include "Editor/System/EditorEngine.h"
#endif // WITH_EDITOR

/*
==================
CGeneralVertexShaderParameters::CGeneralVertexShaderParameters
==================
*/
CGeneralVertexShaderParameters::CGeneralVertexShaderParameters( bool InSupportsInstancing )
    : bSupportsInstancing( InSupportsInstancing )
{}

/*
==================
CGeneralVertexShaderParameters::Bind
==================
*/
void CGeneralVertexShaderParameters::Bind( const class CShaderParameterMap& InParameterMap )
{
    if ( !bSupportsInstancing )
    {
        localToWorldMatrixParameter.Bind( InParameterMap, TEXT( "localToWorldMatrix" ), true );

#if WITH_EDITOR
        colorOverlayParameter.Bind( InParameterMap, TEXT( "colorOverlay" ), true );
#endif // WITH_EDITOR
    }
}

/*
==================
CGeneralVertexShaderParameters::Set
==================
*/
void CGeneralVertexShaderParameters::Set( class CBaseDeviceContextRHI* InDeviceContextRHI, const class CVertexFactory* InVertexFactory ) const
{}

/*
==================
CGeneralVertexShaderParameters::SetMesh
==================
*/
void CGeneralVertexShaderParameters::SetMesh( class CBaseDeviceContextRHI* InDeviceContextRHI, const struct MeshBatch& InMesh, const class CVertexFactory* InVertexFactory, const class CSceneView* InView, uint32 InNumInstances /* = 1 */, uint32 InStartInstanceID /* = 0 */ ) const
{
    if ( !bSupportsInstancing )
    {
        Assert( InNumInstances == 1 );
        const MeshInstance&        meshInstance = InMesh.instances[ InStartInstanceID ];
        SetVertexShaderValue( InDeviceContextRHI, localToWorldMatrixParameter, meshInstance.transformMatrix );
       
#if WITH_EDITOR
        if ( meshInstance.bSelected )
        {
            SetVertexShaderValue( InDeviceContextRHI, colorOverlayParameter, g_EditorEngine->GetSelectionColor().ToNormalizedVector4D() );
        }
        else
        {
			SetVertexShaderValue( InDeviceContextRHI, colorOverlayParameter, Vector4D( 0.f, 0.f, 0.f, 0.f ) );
        }
#endif // WITH_EDITOR
    }
    else
    {
        Assert( InVertexFactory );
        InVertexFactory->SetupInstancing( InDeviceContextRHI, InMesh, InView, InNumInstances, InStartInstanceID );
    }
}
