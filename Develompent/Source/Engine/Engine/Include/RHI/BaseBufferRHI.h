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
#include "TypesRHI.h"

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
struct SLockedData
{
	/**
	 * Constructor
	 */
	SLockedData() :
		data( nullptr ),
		isNeedFree( false ),
		size( 0 ),
		pitch( 0 )
	{}

	/**
	 * Destructor
	 */
	~SLockedData()
	{
		if ( isNeedFree )
		{
			delete[] data;
			isNeedFree = false;
		}

		data = nullptr;
	}

	byte*					data;				/**< Pointer to data */
	bool					isNeedFree;			/**< Need free data? */
	uint32					size;				/**< Size of data */
	uint32					pitch;				/**< Pitch of data */
	ResourceRHIRef_t		stagingResource;	/**< Staging resource for read/write operations */
};

/**
 * @ingroup Engine
 * @brief Base class for work with vertex buffer
 */
class CBaseVertexBufferRHI : public CBaseResourceRHI
{
public:
	/**
	 * @brief Constructor
	 * @param[in] InUsage Usage flags
	 * @param[in] InSize Size of buffer
	 */
	CBaseVertexBufferRHI( uint32 InUsage, uint32 InSize ) :
		usage( InUsage ),
		size( InSize )
	{}

	/**
	 * @brief Destructor
	 */
	virtual ~CBaseVertexBufferRHI()												{}

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

/**
 * @ingroup Engine
 * @brief Base class for work with index buffer
 */
class CBaseIndexBufferRHI : public CBaseResourceRHI
{
public:
	/**
	 * @brief Constructor
	 * @param[in] InUsage Usage flags
	 * @param[in] InStride Stride of struct
	 * @param[in] InSize Size of buffer
	 */
	CBaseIndexBufferRHI( uint32 InUsage, uint32 InStride, uint32 InSize )
	{}

	/**
	 * @brief Destructor
	 */
	virtual ~CBaseIndexBufferRHI()							{}

	/**
	 * @brief Get usage flags
	 * @return Usage flags
	 */
	FORCEINLINE uint32 GetUsage() const						{ return usage; }

	/**
	 * @brief Get buffer size
	 * @return Buffer size
	 */
	FORCEINLINE uint32 GetSize() const						{ return size; }

	/**
	 * @brief Get stride
	 * @return Stride of struct
	 */
	FORCEINLINE uint32 GetStride() const					{ return stride; }

private:
	uint32			usage;		/**< Usage flags */
	uint32			stride;		/**< Stride of struct */
	uint32			size;		/**< Size of buffer */
};

#endif // !BASEBUFFERRHI_H