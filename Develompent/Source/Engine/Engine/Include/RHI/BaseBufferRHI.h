/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright BSOD-Games, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef BASEBUFFERRHI_H
#define BASEBUFFERRHI_H

#include "Core.h"
#include "Misc/RefCounted.h"

/**
 * @ingroup Engine
 * @brief Enumeration of resource usage flags for vertex and index buffers
 */
enum EResourceUsageFlags
{
	RUF_Static		= 1 << 0,							/**< The resource will be created, filled, and never repacked */
	RUF_Dynamic		= 1 << 1,							/**< The resource will be repacked in-frequently */
	RUF_Volatile	= 1 << 2,							/**< The resource will be repacked EVERY frame */

	RUF_AnyDynamic	= RUF_Dynamic | RUF_Volatile		/**< Helper bit-mask for dynamic */
};

/**
 * @ingroup Engine
 * @brief Information about a resource that is currently locked
 */
struct FLockedData
{
	/**
	 * Constructor
	 */
	FLockedData() :
		data( nullptr ),
		isNeedFree( false ),
		size( 0 ),
		pitch( 0 )
	{}

	/**
	 * Destructor
	 */
	~FLockedData()
	{
		if ( isNeedFree )
		{
			delete[] data;
			isNeedFree = false;
		}

		data = nullptr;
	}

	byte*		data;			/**< Pointer to data */
	bool		isNeedFree;		/**< Need free data? */
	uint32		size;			/**< Size of data */
	uint32		pitch;			/**< Pitch of data */
};

/**
 * @ingroup Engine
 * @brief Base class for work with vertex buffer
 */
class FBaseVertexBufferRHI : public FRefCounted
{
public:
	/**
	 * @brief Constructor
	 * @param[in] InUsage Usage flags
	 * @param[in] InSize Size of buffer
	 */
	FBaseVertexBufferRHI( uint32 InUsage, uint32 InSize ) :
		usage( InUsage ),
		size( InSize )
	{}

	/**
	 * @brief Destructor
	 */
	virtual ~FBaseVertexBufferRHI()												{}

	/**
	 * @brief Get usage flags
	 * @return Usage flags
	 */
	FORCEINLINE uint32 GetUsage() const											{ return usage; }

	/**
	 * @brief Get buffer size
	 * @return Buffer size
	 */
	FORCEINLINE uint32 GetSize() const											{ return size; }

private:
	uint32			usage;		/**< Usage flags */
	uint32			size;		/**< Size of buffer */
};

#endif // !BASEBUFFERRHI_H