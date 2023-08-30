/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef SIMPLEELEMENTVERTEXFACTORY_H
#define SIMPLEELEMENTVERTEXFACTORY_H

#include "Math/Math.h"
#include "Math/Color.h"
#include "Render/VertexFactory/VertexFactory.h"
#include "Render/RenderUtils.h"

 /**
  * @ingroup Engine
  * Simple element vertex type
  */
struct SimpleElementVertexType
{
	Vector4D		position;		/**< Position vertex */
	Vector2D		texCoord;		/**< Texture coords */
	CColor			color;			/**< Color */

	/**
	 * Overload operator ==
	 */
	bool FORCEINLINE operator==( const SimpleElementVertexType& InOther ) const
	{
		return position == InOther.position && texCoord == InOther.texCoord && color == InOther.color;
	}
};

/**
 * @ingroup Engine
 * The simple element vertex declaration resource type
 */
class CSimpleElementVertexDeclaration : public CRenderResource
{
public:
	/**
	 * @brief Get vertex declaration RHI
	 * @return Return vertex declaration RHI
	 */
	FORCEINLINE VertexDeclarationRHIRef_t GetVertexDeclarationRHI()
	{
		if ( !vertexDeclarationRHI )
		{
			InitRHI();
		}
		return vertexDeclarationRHI;
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
	VertexDeclarationRHIRef_t		vertexDeclarationRHI;		/**< Vertex declaration RHI */
};

/**
 * @ingroup Engine
 * Global resource of simple element vertex declaration
 */
extern TGlobalResource< CSimpleElementVertexDeclaration >			g_SimpleElementVertexDeclaration;

/**
 * @ingroup Engine
 * Simple element vertex factory
 */
class CSimpleElementVertexFactory : public CVertexFactory
{
	DECLARE_VERTEX_FACTORY_TYPE( CSimpleElementVertexFactory )

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
	 * @brief Construct vertex factory shader parameters
	 *
	 * @param InShaderFrequency Shader frequency
	 * @return Return instance of vertex factory shader parameters
	 */
	static CVertexFactoryShaderParameters* ConstructShaderParameters( EShaderFrequency InShaderFrequency );
};

#endif // !SIMPLEELEMENTVERTEXFACTORY_H
