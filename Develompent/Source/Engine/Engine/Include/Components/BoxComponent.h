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
class CBoxComponent : public CShapeComponent
{
	DECLARE_CLASS( CBoxComponent, CShapeComponent, 0, 0 )

public:
	/**
	 * Constructor
	 */
	CBoxComponent();

#if WITH_EDITOR
	/**
	 * Function called every frame for drawing debug geometry. Override this function to implement custom logic of drawing debug geometry
	 */
	virtual void DrawDebugComponent() override;
#endif // WITH_EDITOR

	/**
	 * @brief Serialize component
	 * @param[in] InArchive Archive for serialize
	 */
	virtual void Serialize( class CArchive& InArchive ) override;

	/**
	 * @brief Update the body setup parameters based on shape information
	 */
	virtual void UpdateBodySetup() override;

	/**
	 * @brief Set size of box
	 * @param InSize Size
	 */
	FORCEINLINE void SetSize( const Vector& InSize )
	{
		size = InSize;
	}

	/**
	 * @brief Get size of box
	 * @return Return size of box
	 */
	FORCEINLINE const Vector& GetSize() const
	{
		return size;
	}

private:
	Vector		size;		/**< Set size of box */
};

#endif // !SHAPECOMPONENT_H