/**
 * @file
 * @addtogroup Physics Physics
 *
 * ************************************************************
 *                  This file is part of:
 *                      LIFEENGINE
 *          https://github.com/zombihello/lifeEngine
 * ************************************************************
 * Copyright (C) 2024 Yehor Pohuliaka.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef PHYSICSBODYSETUP_H
#define PHYSICSBODYSETUP_H

#include <vector>
#include "Physics/System/PhysicsBoxGeometry.h"

/**
 * @ingroup Audio
 * @brief Reference to CPhysicsBodySetup
 */
typedef TRefCountPtr< class CPhysicsBodySetup >				PhysicsBodySetupRef_t;

/**
 * @ingroup Physics
 * @brief BodySetup contains all collision information that is associated with a single asset
 */
class CPhysicsBodySetup : public CRefCounted
{
public:
	/**
	 * Constructor
	 */
	CPhysicsBodySetup();

	/**
	 * @brief Destructor
	 */
	~CPhysicsBodySetup();

	/**
	 * Serialize
	 *
	 * @param[in] InArchive Archive
	 */
	void Serialize( class CArchive& InArchive );

	/**
	 * @brief Add box geometry
	 * @param InBoxGeometry Box geometry
	 */
	FORCEINLINE void AddBoxGeometry( const PhysicsBoxGeometry& InBoxGeometry )
	{
		boxGeometries.push_back( InBoxGeometry );
	}

	/**
	 * @brief Remove box geometry
	 * @param InBoxGeometry Box geometry to remove
	 */
	void RemoveBoxGeometry( const PhysicsBoxGeometry& InBoxGeometry );

	/**
	 * @brief Remove box geometry by index
	 * @param InIndex Index of box geometry
	 */
	FORCEINLINE void RemoveBoxGeometry( uint32 InIndex )
	{
		if ( boxGeometries.size() <= InIndex )
		{
			return;
		}
		boxGeometries.erase( boxGeometries.begin() + InIndex );
	}

	/**
	 * @brief Remove all box geometries
	 */
	FORCEINLINE void RemoveAllBoxGeometries()
	{
		boxGeometries.clear();
	}

	/**
	 * @brief Get array of box geometries
	 * @return Return array of box geometries
	 */
	FORCEINLINE const std::vector< PhysicsBoxGeometry >& GetBoxGeometries() const
	{
		return boxGeometries;
	}

	/**
	 * @brief Get array of box geometries
	 * @return Return array of box geometries
	 */
	FORCEINLINE std::vector< PhysicsBoxGeometry >& GetBoxGeometries()
	{
		return boxGeometries;
	}

private:
	std::vector< PhysicsBoxGeometry >		boxGeometries;		/**< Array of box collisions */
};

#endif // !PHYSICSBODYSETUP_H