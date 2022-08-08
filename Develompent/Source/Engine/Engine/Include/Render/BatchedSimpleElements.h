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
#include "Render/HitProxies.h"
#include "LEBuild.h"

/**
 * @ingroup Engine
 * @brief Batched simple elements for later rendering
 */
class CBatchedSimpleElements
{
public:
#if ENABLE_HITPROXY
	/**
	 * @brief Adds a line to the batch
	 *
	 * @param InStart		Start
	 * @param InEnd			End
	 * @param InHitProxyId	Hit proxy id
	 * @param InThickness	Thickness
	 */
	FORCEINLINE void AddLine( const Vector& InStart, const Vector& InEnd, const CHitProxyId& InHitProxyId, float InThickness = 0.f )
	{
		AddLine( InStart, InEnd, InHitProxyId.GetColor().ToNormalizedVector4D(), InThickness );
	}
#endif // ENABLE_HITPROXY

	/**
	 * @brief Adds a line to the batch
	 * 
	 * @param InStart		Start
	 * @param InEnd			End
	 * @param InColor		Color
	 * @param InThickness	Thickness
	 */
	FORCEINLINE void AddLine( const Vector& InStart, const Vector& InEnd, const CColor& InColor, float InThickness = 0.f )
	{
		// Reserve space for new verteces of line without thickness
		if ( InThickness == 0.f )
		{
			uint32		oldSize = lineVerteces.size();
			lineVerteces.resize( oldSize + 2 );

			// Fill data
			lineVerteces[ oldSize ]		= SSimpleElementVertexType{ Vector4D( InStart, 1.f ),	Vector2D( 0.f, 0.f ), InColor };
			lineVerteces[ oldSize + 1 ] = SSimpleElementVertexType{ Vector4D( InEnd, 1.f ),	Vector2D( 0.f, 0.f ), InColor };
		}
		else
		{
			uint32		oldSize = thickLines.size();
			thickLines.resize( oldSize + 1 );

			SBatchedThickLines&		thickLine = thickLines[ oldSize ];
			thickLine.start			= InStart;
			thickLine.end			= InEnd;
			thickLine.thickness		= InThickness;
			thickLine.color			= InColor;
		}
	}

	/**
	 * @brief Is empty
	 * @return Return TRUE if batch is empty, else return FALSE
	 */
	FORCEINLINE bool IsEmpty() const
	{
		return lineVerteces.empty() && thickLines.empty();
	}

	/**
	 * @brief Clear
	 */
	FORCEINLINE void Clear()
	{
		lineVerteces.clear();
		thickLines.clear();
	}

	/**
	 * @brief Draw all batched elements
	 * 
	 * @param InDeviceContext		RHI device context
	 * @param InSceneView			Scene view
	 */
	void Draw( class CBaseDeviceContextRHI* InDeviceContext, const class CSceneView& InSceneView ) const;

protected:
	/**
	 * @brief Struct of batched thick lines
	 */
	struct SBatchedThickLines
	{
		Vector		start;		/**< Start line */
		Vector		end;		/**< End line */
		float		thickness;	/**< Thickness */
		CColor		color;		/**< Color */
	};

	std::vector<SSimpleElementVertexType>		lineVerteces;		/**< Array of line verteces */
	std::vector<SBatchedThickLines>				thickLines;			/**< Array of thick lines */
};

#endif // !BATCHEDSIMPLEELEMENTS_H