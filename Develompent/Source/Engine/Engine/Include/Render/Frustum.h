/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef FRUSTUM_H
#define FRUSTUM_H

#include "Math/Math.h"
#include "Math/Box.h"

/**
 * @ingroup Engine
 * Frustum for culling in scene
 */
class CFrustum
{
public:
	/**
	 * Update frustum
	 * 
	 * @param InProjectionView Projection * View matrix
	 */
	FORCEINLINE void Update( const Matrix& InProjectionView )
	{
		// Right side
		planes[ FS_Right ].x = InProjectionView[ 0 ].w - InProjectionView[ 0 ].x;
		planes[ FS_Right ].y = InProjectionView[ 1 ].w - InProjectionView[ 1 ].x;
		planes[ FS_Right ].z = InProjectionView[ 2 ].w - InProjectionView[ 2 ].x;
		planes[ FS_Right ].w = InProjectionView[ 3 ].w - InProjectionView[ 3 ].x;

		// Left side
		planes[ FS_Left ].x = InProjectionView[ 0 ].w + InProjectionView[ 0 ].x;
		planes[ FS_Left ].y = InProjectionView[ 1 ].w + InProjectionView[ 1 ].x;
		planes[ FS_Left ].z = InProjectionView[ 2 ].w + InProjectionView[ 2 ].x;
		planes[ FS_Left ].w = InProjectionView[ 3 ].w + InProjectionView[ 3 ].x;

		// Bottom side
		planes[ FS_Bottom ].x = InProjectionView[ 0 ].w + InProjectionView[ 0 ].y;
		planes[ FS_Bottom ].y = InProjectionView[ 1 ].w + InProjectionView[ 1 ].y;
		planes[ FS_Bottom ].z = InProjectionView[ 2 ].w + InProjectionView[ 2 ].y;
		planes[ FS_Bottom ].w = InProjectionView[ 3 ].w + InProjectionView[ 3 ].y;

		// Top side
		planes[ FS_Top ].x = InProjectionView[ 0 ].w - InProjectionView[ 0 ].y;
		planes[ FS_Top ].y = InProjectionView[ 1 ].w - InProjectionView[ 1 ].y;
		planes[ FS_Top ].z = InProjectionView[ 2 ].w - InProjectionView[ 2 ].y;
		planes[ FS_Top ].w = InProjectionView[ 3 ].w - InProjectionView[ 3 ].y;

		// Back side
		planes[ FS_Back ].x = InProjectionView[ 0 ].w + InProjectionView[ 0 ].z;
		planes[ FS_Back ].y = InProjectionView[ 1 ].w + InProjectionView[ 1 ].z;
		planes[ FS_Back ].z = InProjectionView[ 2 ].w + InProjectionView[ 2 ].z;
		planes[ FS_Back ].w = InProjectionView[ 3 ].w + InProjectionView[ 3 ].z;

		// Front side
		planes[ FS_Front ].x = InProjectionView[ 0 ].w - InProjectionView[ 0 ].z;
		planes[ FS_Front ].y = InProjectionView[ 1 ].w - InProjectionView[ 1 ].z;
		planes[ FS_Front ].z = InProjectionView[ 2 ].w - InProjectionView[ 2 ].z;
		planes[ FS_Front ].w = InProjectionView[ 3 ].w - InProjectionView[ 3 ].z;

		NormalizePlanes();
	}

	/**
	 * Is box in frustum
	 * 
	 * @param InMinPosition Min position of box
	 * @param InMaxPosition Max position of box
	 * @return Return true if box in frustum, else returning false
	 */
	FORCEINLINE bool IsIn( const Vector& InMinPosition, const Vector& InMaxPosition ) const
	{
		for ( uint32 index = 0; index < 6; ++index )
		{
			if ( planes[ index ].x * InMinPosition.x + planes[ index ].y * InMinPosition.y + planes[ index ].z * InMinPosition.z + planes[ index ].w > 0 )		continue;
			if ( planes[ index ].x * InMaxPosition.x + planes[ index ].y * InMinPosition.y + planes[ index ].z * InMinPosition.z + planes[ index ].w > 0 )		continue;
			if ( planes[ index ].x * InMaxPosition.x + planes[ index ].y * InMaxPosition.y + planes[ index ].z * InMinPosition.z + planes[ index ].w > 0 )		continue;
			if ( planes[ index ].x * InMinPosition.x + planes[ index ].y * InMaxPosition.y + planes[ index ].z * InMinPosition.z + planes[ index ].w > 0 )		continue;
			if ( planes[ index ].x * InMinPosition.x + planes[ index ].y * InMinPosition.y + planes[ index ].z * InMaxPosition.z + planes[ index ].w > 0 )		continue;
			if ( planes[ index ].x * InMaxPosition.x + planes[ index ].y * InMinPosition.y + planes[ index ].z * InMaxPosition.z + planes[ index ].w > 0 )		continue;
			if ( planes[ index ].x * InMaxPosition.x + planes[ index ].y * InMaxPosition.y + planes[ index ].z * InMaxPosition.z + planes[ index ].w > 0 )		continue;
			if ( planes[ index ].x * InMinPosition.x + planes[ index ].y * InMaxPosition.y + planes[ index ].z * InMaxPosition.z + planes[ index ].w > 0 )		continue;

			return false;
		}

		return true;
	}

	/**
	 * Is box in frustum
	 *
	 * @param InBox Box
	 * @return Return true if box in frustum, else returning false
	 */
	FORCEINLINE bool IsIn( const CBox& InBox ) const
	{
		if ( !InBox.IsValid() )
		{
			return true;
		}

		return IsIn( InBox.GetMin(), InBox.GetMax() );
	}

	/**
	 * Is sphere in frustum
	 * 
	 * @param InPosition Position of sphere
	 * @param InRadius Radius of sphere
	 * @return Return true if sphere in frustum, else returning false
	 */
	FORCEINLINE bool IsIn( const Vector& InPosition, float InRadius ) const
	{
		for ( uint32 index = 0; index < 6; ++index )
		{
			if ( planes[ index ].x * InPosition.x + planes[ index ].y * InPosition.y +
				 planes[ index ].z * InPosition.z + planes[ index ].w <= -InRadius )
			{
				return false;
			}
		}

		return true;
	}

private:
	/**
	 * Enumeration of frustum side
	 */
	enum EFrustumSide
	{
		FS_Right,		/**< Right side */
		FS_Left,		/**< Left side */
		FS_Bottom,		/**< Bottom side */
		FS_Top,			/**< Top side */
		FS_Back,		/**< Back side */
		FS_Front		/**< Front side */
	};

	/**
	 * Normalize planes of frustum
	 */
	FORCEINLINE void NormalizePlanes()
	{
		for ( uint32 side = 0; side < 6; ++side )
		{
			planes[ side ] = SMath::NormalizeVector( planes[ side ] );
		}
	}

	Vector4D		planes[ 6 ];		/**< Planes of frustum */
};

#endif // !FRUSTUM_H