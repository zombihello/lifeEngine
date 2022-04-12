/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef BOXCOMPONENT_H
#define BOXCOMPONENT_H

#include "Components/ShapeComponent.h"

 /**
  * @ingroup Engine
  * Box collision shape
  */
class LBoxComponent : public LShapeComponent
{
	DECLARE_CLASS( LBoxComponent, LShapeComponent )

public:
	/**
	 * Constructor
	 */
	LBoxComponent();

	/**
	 * @brief Serialize component
	 * @param[in] InArchive Archive for serialize
	 */
	virtual void Serialize( class FArchive& InArchive ) override;

	/**
	 * @brief Update the body setup parameters based on shape information
	 */
	virtual void UpdateBodySetup() override;

	/**
	 * @brief Set size of box
	 * @param InSize Size
	 */
	FORCEINLINE void SetSize( const FVector& InSize )
	{
		size = InSize;
	}

	/**
	 * @brief Get size of box
	 * @return Return size of box
	 */
	FORCEINLINE const FVector& GetSize() const
	{
		return size;
	}

private:
	FVector		size;		/**< Set size of box */
};

#endif // !SHAPECOMPONENT_H