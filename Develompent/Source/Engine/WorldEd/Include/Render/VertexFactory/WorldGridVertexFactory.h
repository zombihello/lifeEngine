/**
 * @file
 * @addtogroup WorldEd WorldEd
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef WORLDGRIDVERTEXFACTORY_H
#define WORLDGRIDVERTEXFACTORY_H

#include "Math/Math.h"
#include "Render/VertexFactory/VertexFactory.h"

 /**
  * @ingroup WorldEd
  * Vertex type for world grid
  */
struct FWorldGridVertexType
{
	FVector4D		position;		/**< Position vertex */

	/**
	 * Overload operator ==
	 */
	bool FORCEINLINE operator==( const FWorldGridVertexType& InOther ) const
	{
		return position == InOther.position;
	}
};

/**
 * @ingroup WorldEd
 * Vertex factory for render world grid
 */
class FWorldGridVertexFactory : public FVertexFactory
{
	DECLARE_VERTEX_FACTORY_TYPE( FWorldGridVertexFactory )

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

FORCEINLINE FArchive& operator<<( FArchive& InArchive, FWorldGridVertexType& InValue )
{
	InArchive << InValue.position;
	return InArchive;
}

FORCEINLINE FArchive& operator<<( FArchive& InArchive, const FWorldGridVertexType& InValue )
{
	InArchive << InValue.position;
	return InArchive;
}

#endif // !WORLDGRIDVERTEXFACTORY_H
