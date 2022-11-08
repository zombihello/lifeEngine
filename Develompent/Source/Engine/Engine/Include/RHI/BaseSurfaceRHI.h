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
#include "RHI/TypesRHI.h"
#include "System/Archive.h"

/**
 * @ingroup Engine
 * Enumeration of pixel formats
 */
enum EPixelFormat
{
	PF_Unknown,					/**< Unknown pixel format */
	PF_A8R8G8B8,				/**< ARGB with 8 bit per channel */
	PF_DepthStencil,			/**< Depth stencil format */
	PF_ShadowDepth,				/**< Shadow depth format */
	PF_FilteredShadowDepth,		/**< Filtered shadow depth format*/
	PF_D32,						/**< Depth buffer 32 bit */
	PF_FloatRGB,				/**< Float RGB */
	PF_FloatRGBA,				/**< Float RGBA */
	PF_R32F,					/**< R with float 32 bit channel */
	PF_BC1,						/**< BC1 (DXT1) compression format */
	PF_BC2,						/**< BC2 (DXT3) compression format */
	PF_BC3,						/**< BC3 (DXT5) compression format */
	PF_BC5,						/**< BC5 compression format */
	PF_BC6H,					/**< BC6 compression format */
	PF_BC7,						/**< BC7 compression format */
	PF_Max						/**< Max count pixel formats */
};

/**
 * @ingroup Engine
 * Enumeration of flags for create texture
 */
enum ETextureCreateFlags
{
	TCF_None					= 0,		/**< Null flag */
	TCF_sRGB					= 1 << 0,	/**< Texture is encoded in sRGB gamma space */
	TCF_Dynamic					= 1 << 1,	/**< Texture that may be updated every frame */
	TCF_ResolveTargetable		= 1 << 2,	/**< Texture can be used as a resolve target */
	TCF_DepthStencil			= 1 << 3	/**< Texture is a depth stencil format that can be sampled */
};

/**
 * @ingroup Engine
 * Rect for resolve targets
 */
struct SResolveRect
{
	int32	x1;		/**< X1 */
	int32	y1;		/**< Y1 */
	int32	x2;		/**< X2 */
	int32	y2;		/**< Y2 */

	/**
	 * Constructor
	 * 
	 * @param InX1	X1
	 * @param InY1	Y1
	 * @param InX2	X2
	 * @param InY2	Y2
	 */
	SResolveRect( int32 InX1 = -1, int32 InY1 = -1, int32 InX2 = -1, int32 InY2 = -1 )
		: x1( InX1 )
		, y1( InY1 )
		, x2( InX2 )
		, y2( InY2 )
	{}
};

/**
 * @ingroup Engine
 * Parameters for resolve targets
 */
struct SResolveParams
{
	SResolveRect			rect;					/**< Resolve rect bounded by [X1,Y1]..[X2,Y2]. Or -1 for fullscreen */
	SurfaceRHIParamRef_t	resolveTargetSurface;	/**< Surface to resolve to. If NULL attempt to use 'resolveTarget' */
	Texture2DRHIParamRef_t	resolveTarget;			/**< Texture to resolve to. If NULL it will resolve to the texture associated with the render target */

	/**
	 * Constructor
	 */ 
	SResolveParams( const SResolveRect& InRect = SResolveRect(), Texture2DRHIParamRef_t InResolveTarget = nullptr, SurfaceRHIParamRef_t InResolveTargetSurface = nullptr )
		: rect( InRect )
		, resolveTargetSurface( InResolveTargetSurface )
		, resolveTarget( InResolveTarget )
	{}

	/**
	 * Constructor
	 */
	SResolveParams( Texture2DRHIParamRef_t InResolveTarget )
		: resolveTargetSurface( nullptr )
		, resolveTarget( InResolveTarget )
	{}
};

/**
 * @ingroup Engine
 * @brief Base class of surface for RHI
 */
class CBaseSurfaceRHI : public CBaseResourceRHI
{
public:
	/**
	 * @brief Destructor
	 */
	virtual					~CBaseSurfaceRHI() {}
};

/**
 * @ingroup Engine
 * Base class of texture for RHI
 */
class CBaseTextureRHI : public CBaseResourceRHI
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
	CBaseTextureRHI( uint32 InSizeX, uint32 InSizeY, uint32 InNumMips, EPixelFormat InFormat, uint32 InFlags ) :
		sizeX( InSizeX ),
		sizeY( InSizeY ),
		numMips( InNumMips ),
		format( InFormat ),
		flags( InFlags )
	{}

	/**
	 * Destructor
	 */
	virtual ~CBaseTextureRHI() {}

	/**
	 * Get texture handle
	 * @return Return texture handle
	 */
	virtual void* GetHandle() const { return nullptr; }

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

FORCEINLINE CArchive& operator<<( CArchive& InArchive, EPixelFormat& InValue )
{
	InArchive.Serialize( &InValue, sizeof( InValue ) );
	return InArchive;
}

FORCEINLINE CArchive& operator<<( CArchive& InArchive, const EPixelFormat& InValue )
{
	check( InArchive.IsSaving() );
	InArchive.Serialize( ( void* ) &InValue, sizeof( InValue ) );
	return InArchive;
}

#endif // !BASESURFACERHI_H
