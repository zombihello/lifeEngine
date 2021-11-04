/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright BSOD-Games, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef BASESURFACERHI_H
#define BASESURFACERHI_H

#include "Misc/RefCounted.h"
#include "RHI/BaseResourceRHI.h"
#include "System/Archive.h"

/**
 * @ingroup Engine
 * Enumeration of pixel formats
 */
enum EPixelFormat
{
    PF_Unknown,			/**< Unknown pixel format */
    PF_A8R8G8B8,		/**< ARGB with 8 bit per channel */
	PF_Max				/**< Max count pixel formats */
};

/**
 * @ingroup Engine
 * Enumeration of flags for create texture
 */
enum ETextureCreateFlags
{
	TCF_None			= 0,		/**< Null flag */
	TCF_sRGB			= 1 << 0,	/**< Texture is encoded in sRGB gamma space */
	TCF_Dynamic			= 1 << 1	/**< Texture that may be updated every frame */
};

/**
 * @ingroup Engine
 * @brief Base class of surface for RHI
 */
class FBaseSurfaceRHI : public FBaseResourceRHI
{
public:
	/**
	 * @brief Destructor
	 */
	virtual					~FBaseSurfaceRHI() {}
};

/**
 * @ingroup Engine
 * Base class of texture for RHI
 */
class FBaseTextureRHI : public FBaseResourceRHI
{
public:
	/**
	 * Constructor
	 * 
	 * @param[in] InSizeX Width of texture
	 * @param[in] InSizeY Height of texture
	 * @param[in] InNumMips Number of mip-maps in texture
	 * @param[in] InFormat Pixel format in texture
	 * @param[in] InFlags Texture create flags (use ETextureCreateFlags)
	 */
	FBaseTextureRHI( uint32 InSizeX, uint32 InSizeY, uint32 InNumMips, EPixelFormat InFormat, uint32 InFlags ) :
		sizeX( InSizeX ),
		sizeY( InSizeY ),
		numMips( InNumMips ),
		format( InFormat ),
		flags( InFlags )
	{}

	/**
	 * Destructor
	 */
	virtual ~FBaseTextureRHI() {}

	/**
	 * Get width of texture
	 * @return Return width of texture
	 */
	FORCEINLINE uint32 GetSizeX() const
	{
		return sizeX;
	}

	/**
	 * Get height of texture
	 * @return Return height of texture
	 */
	FORCEINLINE uint32 GetSizeY() const
	{
		return sizeY;
	}

	/**
	 * Get number of mip-maps in texture
	 * @return Return number of mip-maps in texture
	 */
	FORCEINLINE uint32 GetNumMips() const
	{
		return numMips;
	}

	/**
	 * Get pixel format in texture
	 * @return Return pixel format
	 */
	FORCEINLINE EPixelFormat GetFormat() const
	{
		return format;
	}

	/**
	 * Get texture create flags
	 * @return Return texture create flags
	 */
	FORCEINLINE uint32 GetFlags() const
	{
		return flags;
	}

protected:
	uint32			sizeX;			/**< Width of texture */
	uint32			sizeY;			/**< Height of texture */
	uint32			numMips;		/**< Number of mip-maps in texture */
	EPixelFormat	format;			/**< Pixel format in texture */
	uint32			flags;			/**< Flags used when the texture was created (contain ETextureCreateFlags) */
};

//
// Overloaded operators for serialize in archive
//

FORCEINLINE FArchive& operator<<( FArchive& InArchive, EPixelFormat& InValue )
{
	InArchive.Serialize( &InValue, sizeof( InValue ) );
	return InArchive;
}

FORCEINLINE FArchive& operator<<( FArchive& InArchive, const EPixelFormat& InValue )
{
	check( InArchive.IsSaving() );
	InArchive.Serialize( ( void* ) &InValue, sizeof( InValue ) );
	return InArchive;
}

#endif // !BASESURFACERHI_H
