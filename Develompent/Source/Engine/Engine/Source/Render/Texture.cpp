#include "Containers/StringConv.h"
#include "System/BaseArchive.h"
#include "Misc/EngineGlobals.h"
#include "Render/Texture.h"
#include "Render/RenderUtils.h"
#include "RHI/BaseRHI.h"
#include "RHI/BaseSurfaceRHI.h"
#include "Render/TextureFileCache.h"

FTexture2D::FTexture2D() :
	sizeX( 0 ),
	sizeY( 0 ),
	pixelFormat( PF_Unknown )
{}

FTexture2D::~FTexture2D()
{
}

void FTexture2D::InitRHI()
{
	check( !data.empty() );
	texture = GRHI->CreateTexture2D( TEXT( "" ), sizeX, sizeY, pixelFormat, 1, 0, data.data() );

	FSamplerStateInitializerRHI			samplerStateInitializerRHI;
	appMemzero( &samplerStateInitializerRHI, sizeof( FSamplerStateInitializerRHI ) );
	samplerStateInitializerRHI.filter		= SF_Bilinear;
	samplerStateInitializerRHI.addressU		= SAM_Wrap;
	samplerStateInitializerRHI.addressV		= SAM_Wrap;
	samplerState = GRHI->CreateSamplerState( samplerStateInitializerRHI );
	data.clear();
}

void FTexture2D::ReleaseRHI()
{
	texture = nullptr;
}

void FTexture2D::SetData( const struct FTextureCacheItem& InTextureCache )
{
	check( InTextureCache.hash != FTextureCacheItem::INVALID_HASH );
	
	// Copy new parameters of texture
	sizeX			= InTextureCache.sizeX;
	sizeY			= InTextureCache.sizeY;
	pixelFormat		= InTextureCache.pixelFormat;
	data			= InTextureCache.data;

	BeginUpdateResource( this );
}