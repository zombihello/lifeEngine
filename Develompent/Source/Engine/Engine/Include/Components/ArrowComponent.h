/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef ARRAYCOMPONENT_H
#define ARRAYCOMPONENT_H

#include "Components/PrimitiveComponent.h"

/**
 * @ingroup Engine
 * @brief Arrow component
 */
class CArrowComponent : public CPrimitiveComponent
{
	DECLARE_CLASS( CArrowComponent, CPrimitiveComponent )

public:
	/**
	 * @brief Constructor
	 */
	CArrowComponent();

	/**
	 * @brief Adds mesh batches for draw in scene
	 *
	 * @param InSceneView Current view of scene
	 */
	virtual void AddToDrawList( const class CSceneView& InSceneView ) override;

	/**
	 * @brief Set length
	 * @param InLength		Length of arrow
	 */
	FORCEINLINE void SetLength( float InLength )
	{
		length = InLength;
	}

	/**
	 * @brief Get length
	 * @return Return length of arrow
	 */
	FORCEINLINE float GetLength() const
	{
		return length;
	}

private:
	float		length;		/**< Length of arrow */
};

#endif // !ARRAYCOMPONENT_H
