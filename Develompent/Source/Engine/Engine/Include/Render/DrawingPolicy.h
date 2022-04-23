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
class FMeshDrawingPolicy
{
public:
	/**
	 * Constructor
	 */
	FMeshDrawingPolicy();

	/**
	 * Constructor
	 * 
	 * @param[in] InVertexFactory Vertex factory
	 * @param[in] InMaterial Material
	 * @param[in] InDepthBias Depth bias
	 */
	FMeshDrawingPolicy( class FVertexFactory* InVertexFactory, class FMaterial* InMaterial, float InDepthBias = 0.f );

	/**
	 * Destructor
	 */
	virtual ~FMeshDrawingPolicy();

	/**
	 * Initialize mesh drawing policy
	 *
	 * @param InVertexFactory	Vertex factory
	 * @param InMaterial		Material
	 * @param InDepthBias		Depth bias
	 */
	FORCEINLINE void Init( class FVertexFactory* InVertexFactory, class FMaterial* InMaterial, float InDepthBias = 0.f )
	{
		InitInternal( InVertexFactory, InMaterial, InDepthBias );
	}

	/**
	 * Set render state for drawing
	 * 
	 * @param[in] InDeviceContextRHI RHI device context
	 */
	virtual void SetRenderState( class FBaseDeviceContextRHI* InDeviceContextRHI );

	/**
	 * Set shader parameters
	 * 
	 * @param[in] InDeviceContextRHI RHI device context
	 */
	virtual void SetShaderParameters( class FBaseDeviceContextRHI* InDeviceContextRHI );

	/**
	 * @brief Get bound shader state
	 * @return Return bound shader state of current drawing policy
	 */
	virtual FBoundShaderStateRHIRef GetBoundShaderState() const;

	/**
	 * Draw mesh
	 * 
	 * @param[in] InDeviceContextRHI RHI device context
	 * @param[in] InMeshBatch Mesh batch to draw
	 * @param[in] InSceneView Scene view
	 */
	virtual void Draw( class FBaseDeviceContextRHI* InDeviceContextRHI, const struct FMeshBatch& InMeshBatch, const class FSceneView& InSceneView ) PURE_VIRTUAL( FMeshDrawingPolicy::Draw, );

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
	virtual bool Matches( const FMeshDrawingPolicy& InOtherDrawer ) const;

	/**
	 * @brief Overload operator '=='
	 */
	FORCEINLINE bool operator==( const FMeshDrawingPolicy& InOther ) const
	{
		return Matches( InOther );
	}

	/**
	 * @brief Get vertex factory
	 * @return Return vertex factory
	 */
	FORCEINLINE FVertexFactoryRef GetVertexFactory() const
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
	virtual void InitInternal( class FVertexFactory* InVertexFactory, class FMaterial* InMaterial, float InDepthBias = 0.f );

	bool								bInit;				/**< Is inited drawing policy */
	FMaterialRef						material;			/**< Material */
	FVertexFactoryRef					vertexFactory;		/**< Vertex factory */
	FShaderRef							vertexShader;		/**< Vertex shader */
	FShaderRef							pixelShader;		/**< Pixel shader */
	float								depthBias;			/**< Depth bias */
	uint64								hash;				/**< Hash */

private:
	mutable FBoundShaderStateRHIRef		boundShaderState;	/**< Bound shader state */
};

#endif // !DRAWINGPOLICY_H