/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef STATICMESHVERTEXFACTORY_H
#define STATICMESHVERTEXFACTORY_H

#include "Math/Math.h"
#include "Render/VertexFactory/VertexFactory.h"
#include "Render/RenderUtils.h"

 /**
  * @ingroup Engine
  * Vertex type for static mesh
  */
struct SStaticMeshVertexType
{
	Vector4D		position;		/**< Position vertex */
	Vector2D		texCoord;		/**< Texture coords */
	Vector4D		normal;			/**< Normal */
	Vector4D		tangent;		/**< Tangent */
	Vector4D		binormal;		/**< Binormal */

	/**
	 * Overload operator ==
	 */
	bool FORCEINLINE operator==( const SStaticMeshVertexType& InOther ) const
	{
		return position == InOther.position &&
			texCoord == InOther.texCoord &&
			normal == InOther.normal &&
			tangent == InOther.tangent &&
			binormal == InOther.binormal;
	}
};

/**
 * @ingroup Engine
 * The static mesh vertex declaration resource type
 */
class CStaticMeshVertexDeclaration : public CRenderResource
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
 * Global resource of static mesh vertex declaration
 */
extern TGlobalResource< CStaticMeshVertexDeclaration >			g_StaticMeshVertexDeclaration;

/**
 * @ingroup Engine
 * Vertex factory for render static meshes
 */
class CStaticMeshVertexFactory : public CVertexFactory
{
	DECLARE_VERTEX_FACTORY_TYPE( CStaticMeshVertexFactory )

public:
	enum EStreamSourceSlot
	{
		SSS_Main = 0		/**< Main vertex buffer */
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
	static CVertexFactoryShaderParameters* ConstructShaderParameters( EShaderFrequency InShaderFrequency );
};

//
// Serialization
//

FORCEINLINE CArchive& operator<<( CArchive& InArchive, SStaticMeshVertexType& InValue )
{
	InArchive << InValue.position;
	InArchive << InValue.texCoord;
	InArchive << InValue.normal;
	InArchive << InValue.tangent;
	InArchive << InValue.binormal;
	return InArchive;
}

FORCEINLINE CArchive& operator<<( CArchive& InArchive, const SStaticMeshVertexType& InValue )
{
	InArchive << InValue.position;
	InArchive << InValue.texCoord;
	InArchive << InValue.normal;
	InArchive << InValue.tangent;
	InArchive << InValue.binormal;
	return InArchive;
}

#endif // !STATICMESHVERTEXFACTORY_H
