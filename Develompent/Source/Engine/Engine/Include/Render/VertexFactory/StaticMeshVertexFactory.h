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

 /**
  * @ingroup Engine
  * Vertex type for static mesh
  */
struct FStaticMeshVertexType
{
	FVector4D		position;		/**< Position vertex */
	FVector2D		texCoord;		/**< Texture coords */
	FVector4D		normal;			/**< Normal */
	FVector4D		tangent;		/**< Tangent */
	FVector4D		binormal;		/**< Binormal */

	/**
	 * Overload operator ==
	 */
	bool FORCEINLINE operator==( const FStaticMeshVertexType& InOther ) const
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
 * Vertex factory for render static meshes
 */
class FStaticMeshVertexFactory : public FVertexFactory
{
	DECLARE_VERTEX_FACTORY_TYPE( FStaticMeshVertexFactory )

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
	 * @brief Construct vertex factory shader parameters
	 * 
	 * @param InShaderFrequency Shader frequency
	 * @return Return instance of vertex factory shader parameters
	 */
	static FVertexFactoryShaderParameters* ConstructShaderParameters( EShaderFrequency InShaderFrequency );
};

//
// Serialization
//

FORCEINLINE FArchive& operator<<( FArchive& InArchive, FStaticMeshVertexType& InValue )
{
	InArchive << InValue.position;
	InArchive << InValue.texCoord;
	InArchive << InValue.normal;
	InArchive << InValue.tangent;
	InArchive << InValue.binormal;
	return InArchive;
}

FORCEINLINE FArchive& operator<<( FArchive& InArchive, const FStaticMeshVertexType& InValue )
{
	InArchive << InValue.position;
	InArchive << InValue.texCoord;
	InArchive << InValue.normal;
	InArchive << InValue.tangent;
	InArchive << InValue.binormal;
	return InArchive;
}

#endif // !STATICMESHVERTEXFACTORY_H
