/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef BATCHEDSIMPLEELEMENTS_H
#define BATCHEDSIMPLEELEMENTS_H

#include <vector>

#include "Math/Math.h"
#include "Math/Color.h"
#include "Render/VertexFactory/SimpleElementVertexFactory.h"

/**
 * @ingroup Engine
 * @brief Batched simple elements for later rendering
 */
class FBatchedSimpleElements
{
public:
	/**
	 * @brief Adds a line to the batch
	 * 
	 * @param InStart		Start
	 * @param InEnd			End
	 * @param InColor		Color
	 */
	FORCEINLINE void AddLine( const FVector& InStart, const FVector& InEnd, const FColor& InColor )
	{
		// Reserve space for new verteces of line
		uint32		oldSize = lineVerteces.size();
		lineVerteces.resize( oldSize + 2 );

		// Fill data
		lineVerteces[ oldSize ]		= FSimpleElementVertexType{ FVector4D( InStart, 1.f ),	FVector2D( 0.f, 0.f ), InColor };
		lineVerteces[ oldSize + 1 ] = FSimpleElementVertexType{ FVector4D( InEnd, 1.f ),	FVector2D( 0.f, 0.f ), InColor };
	}

	/**
	 * @brief Is empty
	 * @return Return TRUE if batch is empty, else return FALSE
	 */
	FORCEINLINE bool IsEmpty() const
	{
		return lineVerteces.empty();
	}

	/**
	 * @brief Clear
	 */
	FORCEINLINE void Clear()
	{
		lineVerteces.clear();
	}

	/**
	 * @brief Draw all batched elements
	 * 
	 * @param InDeviceContext		RHI device context
	 * @param InSceneView			Scene view
	 */
	void Draw( class FBaseDeviceContextRHI* InDeviceContext, const class FSceneView& InSceneView ) const;

private:
	std::vector< FSimpleElementVertexType >		lineVerteces;		/**< Array of line verteces */
};

#endif // !BATCHEDSIMPLEELEMENTS_H