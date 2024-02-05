/**
 * @file
 * @addtogroup Engine Engine
 *
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

#ifndef GENERALVERTEXFACTORYPARAMS_H
#define GENERALVERTEXFACTORYPARAMS_H

#include "Engine/Render/VertexFactory/VertexFactory.h"
#include "Engine/Render/Shaders/ShaderManager.h"

/**
 * @ingroup Engine
 * @brief Vertex factory shader parameters for each meshes
 */
class CGeneralVertexShaderParameters : public CVertexFactoryShaderParameters
{
public:
    /**
     * Constructor
     * 
     * @param InSupportsInstancing Is supported instancing
     */
    CGeneralVertexShaderParameters( bool InSupportsInstancing );

    /**
     * @brief Bind shader parameters
     *
     * @param InParameterMap Shader parameter map
     */
    virtual void Bind( const class CShaderParameterMap& InParameterMap ) override;

	/**
	 * @brief Set any shader data specific to this vertex factory
	 *
	 * @param InDeviceContextRHI RHI device context
	 * @param InVertexFactory Vertex factory
	 */
	virtual void Set( class CBaseDeviceContextRHI* InDeviceContextRHI, const class CVertexFactory* InVertexFactory ) const override;

	/**
	 * @brief Set the l2w transform shader
	 *
	 * @param InDeviceContextRHI RHI device context
	 * @param InMesh Mesh data
	 * @param InVertexFactory Vertex factory
	 * @param InView Scene view
	 * @param InNumInstances Number instances
	 * @param InStartInstanceID ID of first instance
	 */
	virtual void SetMesh( class CBaseDeviceContextRHI* InDeviceContextRHI, const struct MeshBatch& InMesh, const class CVertexFactory* InVertexFactory, const class CSceneView* InView, uint32 InNumInstances = 1, uint32 InStartInstanceID = 0 ) const override;

protected:
    CShaderParameter		localToWorldMatrixParameter;	/**< Local to world parameter */
    bool					bSupportsInstancing;			/**< Is supported instancing */

#if WITH_EDITOR
	CShaderParameter		colorOverlayParameter;			/**< Color overlay parameter for selection actors */
#endif // WITH_EDITOR
};

#endif // !GENERALVERTEXFACTORYPARAMS_H
