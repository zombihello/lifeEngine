/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef ASTATICMESH_H
#define ASTATICMESH_H

#include "Actors/Actor.h"
#include "Components/StaticMeshComponent.h"

 /**
  * @ingroup Engine
  * Actor of static mesh
  */
class AStaticMesh : public AActor
{
	DECLARE_CLASS( AStaticMesh, AActor )

public:
	/**
	 * Constructor
	 */
	AStaticMesh();

	/**
	 * Destructor
	 */
	virtual ~AStaticMesh();

	/**
	 * Overridable native event for when play begins for this actor
	 */
	virtual void BeginPlay() override;

private:
	TRefCountPtr< LStaticMeshComponent >			staticMeshComponent;		/**< Static mesh component */
};

#endif // !ASTATICMESH_H