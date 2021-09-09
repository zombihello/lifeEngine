#include <vector>

#include "Misc/EngineGlobals.h"
#include "Containers/StringConv.h"
#include "Render/RenderUtils.h"
#include "D3D11RHI.h"
#include "D3D11Surface.h"

/**
 * Find an appropriate DXGI format for the input texture format and SRGB setting.  If the incoming
 * format is recognized as typeless, a fully formed format will be returned
 */
FORCEINLINE DXGI_FORMAT FindDXGIFormat( DXGI_FORMAT InFormat, bool InIsSRGB )
{
	if ( InIsSRGB )
	{
		switch ( InFormat )
		{
		case DXGI_FORMAT_R8G8B8A8_UNORM:
		case DXGI_FORMAT_R8G8B8A8_TYPELESS:
			return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		
		case DXGI_FORMAT_BC1_UNORM:
		case DXGI_FORMAT_BC1_TYPELESS:
			return DXGI_FORMAT_BC1_UNORM_SRGB;
		
		case DXGI_FORMAT_BC2_UNORM:
		case DXGI_FORMAT_BC2_TYPELESS:
			return DXGI_FORMAT_BC2_UNORM_SRGB;
		
		case DXGI_FORMAT_BC3_UNORM:
		case DXGI_FORMAT_BC3_TYPELESS:
			return DXGI_FORMAT_BC3_UNORM_SRGB;
		};
	}
	else
	{
		switch ( InFormat )
		{
		case DXGI_FORMAT_R8G8B8A8_UNORM:
		case DXGI_FORMAT_R8G8B8A8_TYPELESS:
			return DXGI_FORMAT_R8G8B8A8_UNORM;
		
		case DXGI_FORMAT_BC1_UNORM:
		case DXGI_FORMAT_BC1_TYPELESS:
			return DXGI_FORMAT_BC1_UNORM;
		
		case DXGI_FORMAT_BC2_UNORM:
		case DXGI_FORMAT_BC2_TYPELESS:
			return DXGI_FORMAT_BC2_UNORM;
		
		case DXGI_FORMAT_BC3_UNORM:
		case DXGI_FORMAT_BC3_TYPELESS:
			return DXGI_FORMAT_BC3_UNORM;
		};
	}

	return InFormat;
}

FD3D11Surface::FD3D11Surface( ID3D11RenderTargetView* InRenderTargetView ) :
	d3d11RenderTargetView( InRenderTargetView )
{}

FD3D11Surface::~FD3D11Surface()
{
	d3d11RenderTargetView->Release();
}

FD3D11TextureRHI::FD3D11TextureRHI( uint32 InSizeX, uint32 InSizeY, uint32 InNumMips, EPixelFormat InFormat, uint32 InFlags ) :
	FBaseTextureRHI( InSizeX, InSizeY, InNumMips, InFormat, InFlags ),
	d3d11ShaderResourceView( nullptr )
{}

FD3D11TextureRHI::~FD3D11TextureRHI()
{
	if ( d3d11ShaderResourceView )
	{
		d3d11ShaderResourceView->Release();
	}
}

FD3D11Texture2DRHI::FD3D11Texture2DRHI( const tchar* InDebugName, uint32 InSizeX, uint32 InSizeY, uint32 InNumMips, EPixelFormat InFormat, uint32 InFlags, void* InData /*= nullptr*/ ) :
	FD3D11TextureRHI( InSizeX, InSizeY, InNumMips, InFormat, InFlags ),
	d3d11Texture2D( nullptr )
{
	DXGI_FORMAT					platformFormat = FindDXGIFormat( ( DXGI_FORMAT )GPixelFormats[ InFormat ].platformFormat, InFlags & TCF_sRGB );
	D3D11_TEXTURE2D_DESC		d3d11Texture2DDesc;
	d3d11Texture2DDesc.Width				= InSizeX;
	d3d11Texture2DDesc.Height				= InSizeY;
	d3d11Texture2DDesc.MipLevels			= InNumMips;
	d3d11Texture2DDesc.ArraySize			= 1;
	d3d11Texture2DDesc.Format				= platformFormat;
	d3d11Texture2DDesc.SampleDesc.Count		= 1;
	d3d11Texture2DDesc.SampleDesc.Quality	= 0;
	d3d11Texture2DDesc.Usage				= D3D11_USAGE_DEFAULT;
	d3d11Texture2DDesc.BindFlags			= D3D11_BIND_SHADER_RESOURCE;
	d3d11Texture2DDesc.CPUAccessFlags		= 0;
	d3d11Texture2DDesc.MiscFlags			= 0;

	ID3D11Device*	d3d11Device = ( ( FD3D11RHI* )GRHI )->GetD3D11Device();
	check( d3d11Device );

	std::vector< D3D11_SUBRESOURCE_DATA >	subResourceData;
	if ( InData )
	{
		subResourceData.resize( InNumMips );
		uint32			offset = 0;
		byte*			data = ( byte* )InData;
		
		for ( uint32 mipIndex = 0; mipIndex < numMips; ++mipIndex )
		{
			uint32		mipSizeX	= Max< uint32 >( InSizeX >> mipIndex, GPixelFormats[ InFormat ].blockSizeX );
			uint32		pitch		= ( mipSizeX / GPixelFormats[ InFormat ].blockSizeX ) * GPixelFormats[ InFormat ].blockBytes;
			uint32		mipSizeY	= Max< uint32 >( InSizeY >> mipIndex, GPixelFormats[ InFormat ].blockSizeY );
			uint32		numRows		= mipSizeY / GPixelFormats[ InFormat ].blockSizeY;

			D3D11_SUBRESOURCE_DATA&		d3d11SubresourceData = subResourceData[ mipIndex ];
			d3d11SubresourceData.pSysMem = data + offset;
			d3d11SubresourceData.SysMemPitch = pitch;
			d3d11SubresourceData.SysMemSlicePitch = 1;

			offset += numRows * pitch;
		}
	}

	HRESULT			result = d3d11Device->CreateTexture2D( &d3d11Texture2DDesc, InData ? subResourceData.data() : nullptr, &d3d11Texture2D );
	check( result == S_OK );
	D3D11SetDebugName( d3d11Texture2D, TCHAR_TO_ANSI( InDebugName ) );

	if ( d3d11Texture2DDesc.BindFlags & D3D11_BIND_SHADER_RESOURCE )
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC			d3d11ShaderResourceViewDesc;
		d3d11ShaderResourceViewDesc.Format						= platformFormat;
		d3d11ShaderResourceViewDesc.ViewDimension				= D3D11_SRV_DIMENSION_TEXTURE2D;
		d3d11ShaderResourceViewDesc.Texture2D.MostDetailedMip	= 0;
		d3d11ShaderResourceViewDesc.Texture2D.MipLevels			= InNumMips;

		result = d3d11Device->CreateShaderResourceView( d3d11Texture2D, &d3d11ShaderResourceViewDesc, &d3d11ShaderResourceView );
		check( result == S_OK );
	}
}

FD3D11Texture2DRHI::~FD3D11Texture2DRHI()
{
	d3d11Texture2D->Release();
}

void FD3D11Texture2DRHI::Lock( FBaseDeviceContextRHI* InDeviceContext, uint32 InMipIndex, uint32 InArrayIndex, bool InIsDataWrite, bool InIsUseCPUShadow, FLockedData& OutLockedData )
{}

void FD3D11Texture2DRHI::Unlock( FBaseDeviceContextRHI* InDeviceContext, uint32 InMipIndex, uint32 InArrayIndex, FLockedData& InLockedData, bool InDiscardUpdate /*= false*/ )
{}