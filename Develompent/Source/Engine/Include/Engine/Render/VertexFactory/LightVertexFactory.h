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

#ifndef LIGHTVERTEXFACTORY_H
#define LIGHTVERTEXFACTORY_H

#include <vector>

#include "Core/Math/Math.h"
#include "Engine/Render/VertexFactory/VertexFactory.h"
#include "Engine/Render/VertexFactory/GeneralVertexFactoryParams.h"
#include "Engine/Render/RenderUtils.h"

#include "Engine/Components/LightComponent.h"
#include "Engine/Components/PointLightComponent.h"
#include "Engine/Components/SpotLightComponent.h"
#include "Engine/Components/DirectionalLightComponent.h"

/**
 * @ingroup Engine
 * @brief Vertex type for light render
 */
struct LightVertexType
{
	Vector4D		position;		/**< Position vertex */

	/**
	 * @brief Overload operator ==
	 */
	bool FORCEINLINE operator==( const LightVertexType& InOther ) const
	{
		return position == InOther.position;
	}
};

/**
 * @ingroup Engine
 * @brief The light vertex declaration resource type
 */
class CLightVertexDeclaration : public CRenderResource
{
public:
	/**
	 * @brief Get vertex declaration RHI
	 * 
	 * @param InLightType	Light type
	 * @return Return vertex declaration RHI
	 */
	FORCEINLINE VertexDeclarationRHIRef_t GetVertexDeclarationRHI( ELightType InLightType )
	{
		Assert( InLightType != LT_Unknown );
		if ( !vertexDeclarationRHI[InLightType] )
		{
			InitRHI();
		}
		return vertexDeclarationRHI[InLightType];
	}

protected:
	/**
	 * @brief Initializes the RHI resources used by this resource.
	 * Called when the resource is initialized.
	 * This is only called by the rendering thread.
	 */
	virtual void InitRHI() override;

	/**
	 * @brief Releases the RHI resources used by this resource.
	 * Called when the resource is released.
	 * This is only called by the rendering thread.
	 */
	virtual void ReleaseRHI() override;

private:
	VertexDeclarationRHIRef_t		vertexDeclarationRHI[LT_Num];		/**< Vertex declaration RHI */
};

/**
 * @ingroup Engine
 * @brief Global resource of light vertex declaration
 */
extern TGlobalResource<CLightVertexDeclaration>			g_LightVertexDeclaration;

/**
 * @ingroup Engine
 * @brief Vertex factory shader parameters for light
 */
class CLightVertexShaderParameters : public CGeneralVertexShaderParameters
{
public:
	/**
	 * Constructor
	 */
	CLightVertexShaderParameters();

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

	/**
	 * @brief Set the l2w transform shader
	 *
	 * @param InDeviceContextRHI	RHI device context
	 * @param InLights				List of point lights
	 * @param InVertexFactory		Vertex factory
	 * @param InView				Scene view
	 * @param InNumInstances		Number instances
	 * @param InStartInstanceID		ID of first instance
	 */
	void SetMesh( class CBaseDeviceContextRHI* InDeviceContextRHI, const std::list<TRefCountPtr<CPointLightComponent>>& InLights, const class CLightVertexFactory* InVertexFactory, const class CSceneView* InView, uint32 InNumInstances = 1, uint32 InStartInstanceID = 0 ) const;

	/**
	 * @brief Set the l2w transform shader
	 *
	 * @param InDeviceContextRHI	RHI device context
	 * @param InLights				List of spot lights
	 * @param InVertexFactory		Vertex factory
	 * @param InView				Scene view
	 * @param InNumInstances		Number instances
	 * @param InStartInstanceID		ID of first instance
	 */
	void SetMesh( class CBaseDeviceContextRHI* InDeviceContextRHI, const std::list<TRefCountPtr<CSpotLightComponent>>& InLights, const class CLightVertexFactory* InVertexFactory, const class CSceneView* InView, uint32 InNumInstances = 1, uint32 InStartInstanceID = 0 ) const;

	/**
	 * @brief Set the l2w transform shader
	 *
	 * @param InDeviceContextRHI	RHI device context
	 * @param InLights				List of directional lights
	 * @param InVertexFactory		Vertex factory
	 * @param InView				Scene view
	 * @param InNumInstances		Number instances
	 * @param InStartInstanceID		ID of first instance
	 */
	void SetMesh( class CBaseDeviceContextRHI* InDeviceContextRHI, const std::list<TRefCountPtr<CDirectionalLightComponent>>& InLights, const class CLightVertexFactory* InVertexFactory, const class CSceneView* InView, uint32 InNumInstances = 1, uint32 InStartInstanceID = 0 ) const;
};

/**
 * @ingroup Engine
 * @brief Vertex factory for render lights
 */
class CLightVertexFactory : public CVertexFactory
{
	DECLARE_VERTEX_FACTORY_TYPE( CLightVertexFactory )

public:
	enum EStreamSourceSlot
	{
		SSS_Main		= 0,	/**< Main vertex buffer */
		SSS_Instance	= 1		/**< Instance buffer */
	};

	/**
	 * @brief Constructor
	 * @param InLightType	Light type
	 */
	CLightVertexFactory( ELightType InLightType );

	/**
	 * @brief Initializes the RHI resources used by this resource.
	 * Called when the resource is initialized.
	 * This is only called by the rendering thread.
	 */
	virtual void InitRHI() override;

	/**
	 * @brief Setup instancing
	 *
	 * @param InDeviceContextRHI RHI device context
	 * @param InMesh Mesh data
	 * @param InView Scene view
	 * @param InNumInstances Number instances
	 * @param InStartInstanceID ID of first instance
	 */
	virtual void SetupInstancing( class CBaseDeviceContextRHI* InDeviceContextRHI, const struct MeshBatch& InMesh, const class CSceneView* InView, uint32 InNumInstances = 1, uint32 InStartInstanceID = 0 ) const override;

	/**
	 * @brief Setup instancing for point lights
	 *
	 * @param InDeviceContextRHI	RHI device context
	 * @param InLights				List of point lights
	 * @param InView				Scene view
	 * @param InNumInstances		Number instances
	 * @param InStartInstanceID		ID of first instance
	 */
	void SetupInstancing( class CBaseDeviceContextRHI* InDeviceContextRHI, const std::list<TRefCountPtr<CPointLightComponent>>& InLights, const class CSceneView* InView, uint32 InNumInstances = 1, uint32 InStartInstanceID = 0 ) const;

	/**
	 * @brief Setup instancing for spot lights
	 *
	 * @param InDeviceContextRHI	RHI device context
	 * @param InLights				List of spot lights
	 * @param InView				Scene view
	 * @param InNumInstances		Number instances
	 * @param InStartInstanceID		ID of first instance
	 */
	void SetupInstancing( class CBaseDeviceContextRHI* InDeviceContextRHI, const std::list<TRefCountPtr<CSpotLightComponent>>& InLights, const class CSceneView* InView, uint32 InNumInstances = 1, uint32 InStartInstanceID = 0 ) const;

	/**
	 * @brief Setup instancing for directional lights
	 *
	 * @param InDeviceContextRHI	RHI device context
	 * @param InLights				List of spot lights
	 * @param InView				Scene view
	 * @param InNumInstances		Number instances
	 * @param InStartInstanceID		ID of first instance
	 */
	void SetupInstancing( class CBaseDeviceContextRHI* InDeviceContextRHI, const std::list<TRefCountPtr<CDirectionalLightComponent>>& InLights, const class CSceneView* InView, uint32 InNumInstances = 1, uint32 InStartInstanceID = 0 ) const;

	/**
	 * @brief Get type hash
	 * @return Return hash of vertex factory
	 */
	virtual uint64 GetTypeHash() const override;

	/**
	 * @brief Construct vertex factory shader parameters
	 *
	 * @param InShaderFrequency Shader frequency
	 * @return Return instance of vertex factory shader parameters
	 */
	static CVertexFactoryShaderParameters* ConstructShaderParameters( EShaderFrequency InShaderFrequency );

private:
	ELightType		lightType;		/**< Light type */
};

#endif // !LIGHTVERTEXFACTORY_H