/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef LOCALVERTEXFACTORY_H
#define LOCALVERTEXFACTORY_H

#include "Math/Math.h"
#include "Render/VertexFactory/VertexFactory.h"
#include "Render/RenderUtils.h"

 /**
  * @ingroup Engine
  * Vertex type for render in screen space
  */
struct FLocalVertexType
{
	FVector4D		position;		/**< Position vertex */
	FVector2D		texCoord;		/**< Texture coords */

	/**
	 * Overload operator ==
	 */
	bool FORCEINLINE operator==( const FLocalVertexType& InOther ) const
	{
		return position == InOther.position && texCoord == InOther.texCoord;
	}
};

/**
 * @ingroup Engine
 * The local vertex declaration resource type
 */
class FLocalVertexDeclaration : public FRenderResource
{
public:
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

	/**
	 * @brief Get vertex declaration RHI
	 * @return Return vertex declaration RHI
	 */
	FORCEINLINE FVertexDeclarationRHIRef GetVertexDeclarationRHI()
	{
		return vertexDeclarationRHI;
	}

private:
	FVertexDeclarationRHIRef		vertexDeclarationRHI;		/**< Vertex declaration RHI */
};

/**
 * @ingroup Engine
 * Global resource of local vertex declaration
 */
extern TGlobalResource< FLocalVertexDeclaration >			GLocalVertexDeclaration;

/**
 * @ingroup Engine
 * Vertex factory for render in screen space
 */
class FLocalVertexFactory : public FVertexFactory
{
	DECLARE_VERTEX_FACTORY_TYPE( FLocalVertexFactory )

public:
	enum EStreamSourceSlot
	{
		SSS_Main			= 0		/**< Main vertex buffer */
	};

	/**
	 * @brief Initializes the RHI resources used by this resource.
	 * Called when the resource is initialized.
	 * This is only called by the rendering thread.
	 */
	virtual void InitRHI() override;

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
	static FVertexFactoryShaderParameters* ConstructShaderParameters( EShaderFrequency InShaderFrequency );
};

#endif // !LOCALVERTEXFACTORY_H
