#include <vector>

#include "Misc/EngineGlobals.h"
#include "Containers/StringConv.h"
#include "Render/RenderUtils.h"
#include "D3D11RHI.h"
#include "D3D11Surface.h"
#include "D3D11DeviceContext.h"

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

FD3D11Texture2DRHI::FD3D11Texture2DRHI( ID3D11Texture2D* InD3D11Texture2D, uint32 InSizeX, uint32 InSizeY, uint32 InNumMips, EPixelFormat InFormat, uint32 InFlags ) :
	FD3D11TextureRHI( InSizeX, InSizeY, InNumMips, InFormat, InFlags ),
	d3d11Texture2D( InD3D11Texture2D )
{
	check( d3d11Texture2D );
	d3d11Texture2D->AddRef();
}

FD3D11Texture2DRHI::~FD3D11Texture2DRHI()
{
	d3d11Texture2D->Release();
}

void FD3D11Texture2DRHI::Lock( FBaseDeviceContextRHI* InDeviceContext, uint32 InMipIndex, bool InIsDataWrite, bool InIsUseCPUShadow, FLockedData& OutLockedData )
{
	check( OutLockedData.data == nullptr );

	// Calculate the dimensions of the mip-map
	const uint32 blockSizeX			= GPixelFormats[ format ].blockSizeX;
	const uint32 blockSizeY			= GPixelFormats[ format ].blockSizeY;
	const uint32 blockBytes			= GPixelFormats[ format ].blockBytes;
	const uint32 mipSizeX			= Max( sizeX >> InMipIndex, blockSizeX );
	const uint32 mipSizeY			= Max( sizeY >> InMipIndex, blockSizeY );
	const uint32 numBlocksX			= ( mipSizeX + blockSizeX - 1 ) / blockSizeX;
	const uint32 numBlocksY			= ( mipSizeY + blockSizeY - 1 ) / blockSizeY;
	const uint32 mipBytes			= numBlocksX * numBlocksY * blockBytes;

	if ( InIsDataWrite && !( flags & TCF_Dynamic ) )
	{
		// If we're writing to the texture, allocate a system memory buffer to receive the new contents.
		OutLockedData.data = new byte[ mipBytes ];
		OutLockedData.pitch = numBlocksX * blockBytes;
		OutLockedData.size = mipBytes;
		OutLockedData.isNeedFree = true;
	}
	else
	{
		// Calculate the subresource index corresponding to the specified mip-map
		const uint32			subresource = D3D11CalcSubresource( InMipIndex, 0, numMips );
		checkMsg( subresource < numMips, TEXT( "FD3D11Texture2DRHI::Lock; ERROR! Subresource is out of range" ) );

		// If we're reading from the texture, or writing to a dynamic texture, we create a staging resource, 
		// copy the texture contents to it, and map it.
		
		// Create the staging texture
		FD3D11Texture2DRHI*				stagingResource = CreateStagingResource( InDeviceContext, mipSizeX, mipSizeY, subresource, InIsDataWrite, InIsUseCPUShadow );
		OutLockedData.stagingResource = stagingResource;

		// Map the staging resource, and return the mapped address
		ID3D11DeviceContext*			d3d11DeviceContext = ( ( FD3D11DeviceContext* )InDeviceContext )->GetD3D11DeviceContext();
		D3D11_MAPPED_SUBRESOURCE		d3d11MappedTexture2D;
		HRESULT							result = d3d11DeviceContext->Map( stagingResource->GetResource(), 0, InIsDataWrite ? D3D11_MAP_WRITE : D3D11_MAP_READ, 0, &d3d11MappedTexture2D );
		check( result == S_OK );

		OutLockedData.data = ( byte* )d3d11MappedTexture2D.pData;
		OutLockedData.pitch = d3d11MappedTexture2D.RowPitch;
		OutLockedData.size = mipBytes;
		OutLockedData.isNeedFree = false;
	}
}

void FD3D11Texture2DRHI::Unlock( FBaseDeviceContextRHI* InDeviceContext, uint32 InMipIndex, FLockedData& InLockedData, bool InDiscardUpdate /*= false*/ )
{
	bool			isNeedUpdate = ( !InLockedData.stagingResource.IsValid() || flags & TCF_Dynamic ) && !InDiscardUpdate;
	if ( isNeedUpdate )
	{
		// Calculate the subresource index corresponding to the specified mip-map
		const uint32			subresource = D3D11CalcSubresource( InMipIndex, 0, numMips );
		checkMsg( subresource < numMips, TEXT( "FD3D11Texture2DRHI::Unlock; ERROR! Subresource is out of range" ) );

		// If we're writing, we need to update the subresource
		ID3D11DeviceContext*			d3d11DeviceContext = ( ( FD3D11DeviceContext* )InDeviceContext )->GetD3D11DeviceContext();
		d3d11DeviceContext->UpdateSubresource( d3d11Texture2D, subresource, nullptr, InLockedData.data, InLockedData.pitch, 0 );
	}

	if ( InLockedData.isNeedFree )
	{
		delete[] InLockedData.data;
		InLockedData.isNeedFree = false;
	}

	InLockedData.data = nullptr;
}

FD3D11Texture2DRHI* FD3D11Texture2DRHI::CreateStagingResource( class FBaseDeviceContextRHI* InDeviceContext, uint32 InMipSizeX, uint32 InMipSizeY, uint32 InSubresource, bool InIsDataWrite, bool InIsUseCPUShadow )
{
	check( d3d11Texture2D );
	D3D11_TEXTURE2D_DESC			d3d11StagingTextureDesc;
	d3d11Texture2D->GetDesc( &d3d11StagingTextureDesc );
	d3d11StagingTextureDesc.Width				= InMipSizeX;
	d3d11StagingTextureDesc.Height				= InMipSizeY;
	d3d11StagingTextureDesc.MipLevels			= 1;
	d3d11StagingTextureDesc.ArraySize			= 1;
	d3d11StagingTextureDesc.Usage				= D3D11_USAGE_STAGING;
	d3d11StagingTextureDesc.BindFlags			= 0;
	d3d11StagingTextureDesc.CPUAccessFlags		= InIsDataWrite ? D3D11_CPU_ACCESS_WRITE : D3D11_CPU_ACCESS_READ;

	ID3D11Texture2D*		d3d11StagingTexture2D = nullptr;
	HRESULT					result = ( ( FD3D11RHI* )GRHI )->GetD3D11Device()->CreateTexture2D( &d3d11StagingTextureDesc, nullptr, &d3d11StagingTexture2D );
	check( result == S_OK );
	D3D11SetDebugName( d3d11StagingTexture2D, "LockingStaging2DTexture" );

	// Copy the mip-map data from the real resource into the staging resource
	if ( !InIsDataWrite || InIsUseCPUShadow )
	{
		ID3D11DeviceContext*		d3d11DeviceContext = ( ( FD3D11DeviceContext* )InDeviceContext )->GetD3D11DeviceContext();
		d3d11DeviceContext->CopySubresourceRegion( d3d11StagingTexture2D, 0, 0, 0, 0, d3d11Texture2D, InSubresource, nullptr );
	}

	return new FD3D11Texture2DRHI( d3d11StagingTexture2D, InMipSizeX, InMipSizeY, 1, format, TCF_None );
}