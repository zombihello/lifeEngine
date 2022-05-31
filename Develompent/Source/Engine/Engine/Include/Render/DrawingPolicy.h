/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef DRAWINGPOLICY_H
#define DRAWINGPOLICY_H

#include "Misc/RefCountPtr.h"
#include "Render/Material.h"
#include "Render/VertexFactory/VertexFactory.h"
#include "Core.h"

/**
 * @ingroup Engine
 * The base mesh drawing policy.  Subclasses are used to draw meshes with type-specific context variables.
 * May be used either simply as a helper to render a dynamic mesh, or as a static instance shared between
 * similar meshs
 */
class CMeshDrawingPolicy
{
public:
	/**
	 * Constructor
	 */
	CMeshDrawingPolicy();

	/**
	 * Destructor
	 */
	virtual ~CMeshDrawingPolicy();

	/**
	 * Initialize mesh drawing policy
	 *
	 * @param InVertexFactory	Vertex factory
	 * @param InMaterial		Material
	 * @param InDepthBias		Depth bias
	 */
	FORCEINLINE void Init( class CVertexFactory* InVertexFactory, const TAssetHandle<CMaterial>& InMaterial, float InDepthBias = 0.f )
	{
		InitInternal( InVertexFactory, InMaterial, InDepthBias );
	}

	/**
	 * Set render state for drawing
	 * 
	 * @param[in] InDeviceContextRHI RHI device context
	 */
	virtual void SetRenderState( class CBaseDeviceContextRHI* InDeviceContextRHI );

	/**
	 * Set shader parameters
	 * 
	 * @param[in] InDeviceContextRHI RHI device context
	 */
	virtual void SetShaderParameters( class CBaseDeviceContextRHI* InDeviceContextRHI );

	/**
	 * @brief Get bound shader state
	 * @return Return bound shader state of current drawing policy
	 */
	virtual BoundShaderStateRHIRef_t GetBoundShaderState() const;

	/**
	 * @brief Get rasterizer state
	 * @return Return rasterizer state of current drawing policy 
	 */
	virtual RasterizerStateRHIRef_t GetRasterizerState() const;

	/**
	 * Draw mesh
	 * 
	 * @param[in] InDeviceContextRHI RHI device context
	 * @param[in] InMeshBatch Mesh batch to draw
	 * @param[in] InSceneView Scene view
	 */
	virtual void Draw( class CBaseDeviceContextRHI* InDeviceContextRHI, const struct SMeshBatch& InMeshBatch, const class CSceneView& InSceneView );

	/**
	 * @brief Get type hash
	 * @return Return hash of drawing policy
	 */
	virtual uint64 GetTypeHash() const;

	/**
	 * @brief Compare drawing policy
	 * 
	 * @param InOtherDrawer Other drawer
	 * @return Return true if equal with InOtherDrawer, else returning false
	 */
	virtual bool Matches( const CMeshDrawingPolicy& InOtherDrawer ) const;

	/**
	 * @brief Overload operator '=='
	 */
	FORCEINLINE bool operator==( const CMeshDrawingPolicy& InOther ) const
	{
		return Matches( InOther );
	}

	/**
	 * @brief Get vertex factory
	 * @return Return vertex factory
	 */
	FORCEINLINE VertexFactoryRef_t GetVertexFactory() const
	{
		return vertexFactory;
	}

	/**
	 * @brief Get depth bias
	 * @return Return depth bias
	 */
	FORCEINLINE float GetDepthBias() const
	{
		return depthBias;
	}

	/**
	 * @brief Is valid drawing policy
	 * @return Return TRUE if drawing policy is valid, else return FALSE
	 */
	virtual bool IsValid() const;

protected:
	/**
	 * Initialize mesh drawing policy (internal implementation for override by child classes)
	 *
	 * @param InVertexFactory	Vertex factory
	 * @param InMaterial		Material
	 * @param InDepthBias		Depth bias
	 */
	virtual void InitInternal( class CVertexFactory* InVertexFactory, const TAssetHandle<CMaterial>& InMaterial, float InDepthBias = 0.f );

	bool								bInit;				/**< Is inited drawing policy */
	TAssetHandle<CMaterial>				material;			/**< Material */
	VertexFactoryRef_t					vertexFactory;		/**< Vertex factory */
	CShader*							vertexShader;		/**< Vertex shader */
	CShader*							pixelShader;		/**< Pixel shader */
	float								depthBias;			/**< Depth bias */
	uint64								hash;				/**< Hash */

	mutable BoundShaderStateRHIRef_t	boundShaderState;	/**< Bound shader state */
	mutable RasterizerStateRHIRef_t		rasterizerState;	/**< Rasterizer state */
};

#endif // !DRAWINGPOLICY_H