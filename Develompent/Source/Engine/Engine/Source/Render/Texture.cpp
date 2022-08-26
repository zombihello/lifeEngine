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

CTexture2D::CTexture2D()
	: CAsset( AT_Texture2D )
	, sizeX( 0 )
	, sizeY( 0 )
	, pixelFormat( PF_Unknown )
	, addressU( SAM_Wrap )
	, addressV( SAM_Wrap )
	, samplerFilter( SF_Point )
{}

CTexture2D::~CTexture2D()
{}

void CTexture2D::InitRHI()
{
	check( data.Num() > 0 );
	texture = GRHI->CreateTexture2D( CString::Format( TEXT( "%s" ), GetAssetName().c_str() ).c_str(), sizeX, sizeY, pixelFormat, 1, 0, data.GetData() );

	if ( !GIsEditor && !GIsCommandlet )
	{
		data.RemoveAllElements();
	}
}

void CTexture2D::ReleaseRHI()
{
	texture.SafeRelease();
}

void CTexture2D::SetData( EPixelFormat InPixelFormat, uint32 InSizeX, uint32 InSizeY, const std::vector<byte>& InData )
{
	pixelFormat		= InPixelFormat;
	sizeX			= InSizeX;
	sizeY			= InSizeY;
	data			= InData;

	MarkDirty();
	BeginUpdateResource( this );
}

void CTexture2D::Serialize( class CArchive& InArchive )
{
	CAsset::Serialize( InArchive );

	if ( InArchive.Ver() < VER_RemovedTFC )
	{
		std::wstring		textureCachePath;
		uint32				textureCacheHash;

		InArchive << textureCachePath;
		InArchive << textureCacheHash;
		LE_LOG( LT_Warning, LC_Package, TEXT( "Deprecated version CTexture2D in package. Texture not loaded correctly" ) );
	}
	else if ( InArchive.Ver() < VER_CompressedZlib )
	{
		std::vector<byte>		tmpData;
		InArchive << tmpData;
		data = tmpData;
		LE_LOG( LT_Warning, LC_Package, TEXT( "Deprecated package version, in future must be removed supports" ) );
	}
	else
	{
		InArchive << data;
	}

	InArchive << sizeX;
	InArchive << sizeY;
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