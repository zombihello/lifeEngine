/**
 * @file
 * @addtogroup Physics Physics
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef PHYSICSBODYSETUP_H
#define PHYSICSBODYSETUP_H

#include <vector>
#include "System/PhysicsBoxGeometry.h"

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
	FORCEINLINE void AddBoxGeometry( const SPhysicsBoxGeometry& InBoxGeometry )
	{
		boxGeometries.push_back( InBoxGeometry );
	}

	/**
	 * @brief Remove box geometry
	 * @param InBoxGeometry Box geometry to remove
	 */
	void RemoveBoxGeometry( const SPhysicsBoxGeometry& InBoxGeometry );

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
	FORCEINLINE const std::vector< SPhysicsBoxGeometry >& GetBoxGeometries() const
	{
		return boxGeometries;
	}

	/**
	 * @brief Get array of box geometries
	 * @return Return array of box geometries
	 */
	FORCEINLINE std::vector< SPhysicsBoxGeometry >& GetBoxGeometries()
	{
		return boxGeometries;
	}

private:
	std::vector< SPhysicsBoxGeometry >		boxGeometries;		/**< Array of box collisions */
};

#endif // !PHYSICSBODYSETUP_H