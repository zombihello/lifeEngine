#include "Containers/StringConv.h"
#include "Containers/String.h"
#include "Misc/CoreGlobals.h"
#include "System/Archive.h"
#include "System/BaseFileSystem.h"
#include "Logger/LoggerMacros.h"
#include "Misc/EngineGlobals.h"
#include "Render/Texture.h"
#include "Render/RenderUtils.h"
#include "RHI/BaseRHI.h"
#include "RHI/BaseSurfaceRHI.h"

#if WITH_EDITOR
#include <compressonator.h>

/*
==================
ConvertEPixelFormatToCmpFormat
==================
*/
static CMP_FORMAT ConvertEPixelFormatToCmpFormat( EPixelFormat InPixelFormat )
{
	switch ( InPixelFormat )
	{
	case PF_A8R8G8B8:			return CMP_FORMAT_ARGB_8888;
	case PF_FloatRGB:			return CMP_FORMAT_RGB_32F;
	case PF_FloatRGBA:			return CMP_FORMAT_RGBA_32F;
	case PF_R32F:				return CMP_FORMAT_R_32F;
	case PF_BC1:				return CMP_FORMAT_BC1;
	case PF_BC2:				return CMP_FORMAT_BC2;
	case PF_BC3:				return CMP_FORMAT_BC3;
	case PF_BC5:				return CMP_FORMAT_BC5;
	case PF_BC6H:				return CMP_FORMAT_BC6H;
	case PF_BC7:				return CMP_FORMAT_BC7;

	case PF_DepthStencil:
	case PF_ShadowDepth:
	case PF_FilteredShadowDepth:
	case PF_D32:
	default:
		Sys_Errorf( TEXT( "Unsupported EPixelFormat %i" ), ( uint32 )InPixelFormat );
		return CMP_FORMAT_Unknown;
	}
}

/*
==================
GenerateMipmapsMemory
==================
*/
static void GenerateMipmapsMemory( EPixelFormat InPixelFormat, const Texture2DMipMap& InZeroMip, std::vector<Texture2DMipMap>& OutMipmaps, uint32 InRequestMips = 10 )
{
	CMP_MipSet cmp_MipSet;
	Sys_Memzero( &cmp_MipSet, sizeof( CMP_MipSet ) );
	CMP_ERROR	result = CMP_CreateMipSet( &cmp_MipSet, InZeroMip.sizeX, InZeroMip.sizeY, 1, CF_8bit, TT_2D );
	Assert( result == CMP_OK && ( *cmp_MipSet.m_pMipLevelTable )->m_dwLinearSize == InZeroMip.data.Num() );

	// Copy data to mipmap0
	memcpy( ( *cmp_MipSet.m_pMipLevelTable )->m_pbData, ( CMP_BYTE* )InZeroMip.data.GetData(), InZeroMip.data.Num() );

	// Generate mip levels and copy him to OutMipmaps
	CMP_GenerateMIPLevels( &cmp_MipSet, CMP_CalcMinMipSize( InZeroMip.sizeY, InZeroMip.sizeX, InRequestMips ) );
	for ( uint32 index = 0; index < cmp_MipSet.m_nMipLevels; index++ )
	{
		CMP_MipLevel*		cmp_mipLevel;
		CMP_GetMipLevel( &cmp_mipLevel, &cmp_MipSet, index, 0 );

		Texture2DMipMap	mipmap;
		mipmap.sizeX		= cmp_mipLevel->m_nWidth;
		mipmap.sizeY		= cmp_mipLevel->m_nWidth;
		mipmap.data.Resize( cmp_mipLevel->m_dwLinearSize );
		memcpy( mipmap.data.GetData(), cmp_mipLevel->m_pbData, cmp_mipLevel->m_dwLinearSize );
		OutMipmaps.push_back( mipmap );
	}

	CMP_FreeMipSet( &cmp_MipSet );
}
#endif // WITH_EDITOR


/*
==================
CTexture2D::CTexture2D
==================
*/
CTexture2D::CTexture2D()
	: CAsset( AT_Texture2D )
	, pixelFormat( PF_Unknown )
	, addressU( SAM_Wrap )
	, addressV( SAM_Wrap )
	, samplerFilter( SF_Point )
{}

/*
==================
CTexture2D::~CTexture2D
==================
*/
CTexture2D::~CTexture2D()
{}

/*
==================
CTexture2D::InitRHI
==================
*/
void CTexture2D::InitRHI()
{
	Assert( mipmaps.size() > 0 );
	texture = g_RHI->CreateTexture2D( CString::Format( TEXT( "%s" ), GetAssetName().c_str() ).c_str(), GetSizeX(), GetSizeY(), pixelFormat, mipmaps.size(), 0, nullptr );

	// Load all mip-levels to GPU
	for ( uint32 index = 0, count = mipmaps.size(); index < count; ++index )
	{
		const Texture2DMipMap&		mipmap				= mipmaps[index];
		CBaseDeviceContextRHI*		deviceContextRHI	= g_RHI->GetImmediateContext();
		LockedData					lockedData;
		
		g_RHI->LockTexture2D( deviceContextRHI, texture, index, true, lockedData );
		memcpy( lockedData.data, mipmap.data.GetData(), mipmap.data.Num() );
		g_RHI->UnlockTexture2D( deviceContextRHI, texture, index, lockedData );
	}

	if ( !g_IsEditor && !g_IsCommandlet )
	{
		for ( uint32 index = 0, count = mipmaps.size(); index < count; ++index )
		{
			mipmaps[index].data.RemoveAllElements();
		}
	}
}

/*
==================
CTexture2D::ReleaseRHI
==================
*/
void CTexture2D::ReleaseRHI()
{
	texture.SafeRelease();
}

/*
==================
CTexture2D::SetData
==================
*/
void CTexture2D::SetData( EPixelFormat InPixelFormat, uint32 InSizeX, uint32 InSizeY, const std::vector<byte>& InData, bool InIsGenerateMipmaps /* = false */ )
{
	Texture2DMipMap	mipmap0;
	pixelFormat			= InPixelFormat;
	mipmap0.sizeX		= InSizeX;
	mipmap0.sizeY		= InSizeY;
	mipmap0.data		= InData;

	// Clear mipmaps
	mipmaps.clear();

#if WITH_EDITOR
	if ( InIsGenerateMipmaps )
	{
		GenerateMipmapsMemory( InPixelFormat, mipmap0, mipmaps );
	}
	else
#endif // WITH_EDITOR
	{		
		mipmaps.push_back( mipmap0 );
	}

	MarkDirty();
	BeginUpdateResource( this );
}

#if WITH_EDITOR
/*
==================
CTexture2D::GenerateMipmaps
==================
*/
void CTexture2D::GenerateMipmaps()
{
	Assert( !mipmaps.empty() );
	Texture2DMipMap	mipmap0 = mipmaps[0];
	mipmaps.clear();
	
	GenerateMipmapsMemory( pixelFormat, mipmap0, mipmaps );

	MarkDirty();
	BeginUpdateResource( this );
}

/*
==================
CTexture2D::GetMemorySize
==================
*/
uint64 CTexture2D::GetMemorySize() const
{
	uint64		totalSize = 0;
	for ( uint32 index = 0, count = mipmaps.size(); index < count; ++index )
	{
		totalSize += mipmaps[index].data.Num();
	}
	return totalSize;
}
#endif // WITH_EDITOR

/*
==================
CTexture2D::Serialize
==================
*/
void CTexture2D::Serialize( class CArchive& InArchive )
{
	CAsset::Serialize( InArchive );

	// Clear all mipmaps before loading
	if ( InArchive.IsLoading() )
	{
		mipmaps.clear();
	}

	if ( InArchive.Ver() < VER_Mipmaps )
	{
		Texture2DMipMap	mipmap0;
		InArchive << mipmap0.data;
		InArchive << mipmap0.sizeX;
		InArchive << mipmap0.sizeY;
		mipmaps.push_back( mipmap0 );

		std::wstring		referenceToThisAsset;
		MakeReferenceToAsset( GetAssetHandle(), referenceToThisAsset );
		Warnf( TEXT( "%s :: Deprecated package version, in future must be removed supports\n" ), referenceToThisAsset.c_str() );
	}
	else
	{
		InArchive << mipmaps;
	}

	InArchive << pixelFormat;
	InArchive << addressU;
	InArchive << addressV;
	InArchive << samplerFilter;

	// If we loading Texture2D - update render resource
	if ( InArchive.IsLoading() )
	{
		BeginUpdateResource( this );
	}
}