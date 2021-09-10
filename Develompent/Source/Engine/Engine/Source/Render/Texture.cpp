#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "Containers/StringConv.h"
#include "Misc/EngineGlobals.h"
#include "Render/Texture.h"
#include "RHI/BaseRHI.h"
#include "RHI/BaseSurfaceRHI.h"

FTexture2D::FTexture2D() :
	sizeX( 0 ),
	sizeY( 0 ),
	data( nullptr )
{}

FTexture2D::FTexture2D( const std::wstring& InFilename ) :
	sizeX( 0 ),
	sizeY( 0 ),
	data( nullptr )
{
	SetFilename( InFilename );
}

FTexture2D::~FTexture2D()
{
	if ( data )
	{
		stbi_image_free( data );
	}
}

void FTexture2D::InitRHI()
{
	texture = GRHI->CreateTexture2D( TEXT( "" ), sizeX, sizeY, PF_A8R8G8B8, 1, 0, data );

	FSamplerStateInitializerRHI			samplerStateInitializerRHI;
	appMemzero( &samplerStateInitializerRHI, sizeof( FSamplerStateInitializerRHI ) );
	samplerStateInitializerRHI.filter		= SF_Bilinear;
	samplerStateInitializerRHI.addressU		= SAM_Wrap;
	samplerStateInitializerRHI.addressV		= SAM_Wrap;
	samplerState = GRHI->CreateSamplerState( samplerStateInitializerRHI );

	if ( data )
	{
		stbi_image_free( data );
		data = nullptr;
	}
}

void FTexture2D::ReleaseRHI()
{
	texture = nullptr;
}

void FTexture2D::SetFilename( const std::wstring& InFilename )
{
	int				numComponents = 0;
	data = stbi_load( TCHAR_TO_ANSI( InFilename.c_str() ), ( int* )&sizeX, ( int* )&sizeY, &numComponents, 4 );
	BeginUpdateResource( this );
}