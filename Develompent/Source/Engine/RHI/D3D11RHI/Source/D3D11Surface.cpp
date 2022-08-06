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

CD3D11Surface::CD3D11Surface( ID3D11RenderTargetView* InRenderTargetView ) 
	: d3d11ShaderResourceView( nullptr )
	, d3d11RenderTargetView( InRenderTargetView )
	, d3d11DepthStencilView( nullptr )
{}

CD3D11Surface::CD3D11Surface( Texture2DRHIParamRef_t InResolveTargetTexture ) 
	: d3d11ShaderResourceView( nullptr )
	, d3d11RenderTargetView( nullptr )
	, d3d11DepthStencilView( nullptr )
	, resolveTarget2D( ( CD3D11Texture2DRHI* )InResolveTargetTexture )
{
	check( resolveTarget2D );
	d3d11ShaderResourceView = resolveTarget2D->GetShaderResourceView();
	d3d11RenderTargetView = resolveTarget2D->GetRenderTargetView();
	d3d11DepthStencilView = resolveTarget2D->GetDepthStencilView();

	if ( d3d11ShaderResourceView )
	{
		d3d11ShaderResourceView->AddRef();
	}

	if ( d3d11RenderTargetView )
	{
		d3d11RenderTargetView->AddRef();
	}

	if ( d3d11DepthStencilView )
	{
		d3d11DepthStencilView->AddRef();
	}
}

CD3D11Surface::~CD3D11Surface()
{
	if ( d3d11ShaderResourceView )
	{
		d3d11ShaderResourceView->Release();
	}

	if ( d3d11RenderTargetView )
	{
		d3d11RenderTargetView->Release();
	}

	if ( d3d11DepthStencilView )
	{
		d3d11DepthStencilView->Release();
	}
	
	resolveTarget2D.SafeRelease();
}

CD3D11TextureRHI::CD3D11TextureRHI( uint32 InSizeX, uint32 InSizeY, uint32 InNumMips, EPixelFormat InFormat, uint32 InFlags ) :
	CBaseTextureRHI( InSizeX, InSizeY, InNumMips, InFormat, InFlags ),
	d3d11ShaderResourceView( nullptr ),
	d3d11RenderTargetView( nullptr ),
	d3d11DepthStencilView( nullptr )
{}

CD3D11TextureRHI::~CD3D11TextureRHI()
{
	if ( d3d11ShaderResourceView )
	{
		d3d11ShaderResourceView->Release();
	}

	if ( d3d11RenderTargetView )
	{
		d3d11RenderTargetView->Release();
	}

	if ( d3d11DepthStencilView )
	{
		d3d11DepthStencilView->Release();
	}
}

CD3D11Texture2DRHI::CD3D11Texture2DRHI( const tchar* InDebugName, uint32 InSizeX, uint32 InSizeY, uint32 InNumMips, EPixelFormat InFormat, uint32 InFlags, void* InData /*= nullptr*/ ) :
	CD3D11TextureRHI( InSizeX, InSizeY, InNumMips, InFormat, InFlags ),
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

	if ( InFlags & TCF_DepthStencil )
	{
		d3d11Texture2DDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	}
	else if ( InFlags & TCF_ResolveTargetable )
	{
		if ( InFormat == PF_DepthStencil || InFormat == PF_ShadowDepth || InFormat == PF_FilteredShadowDepth || InFormat == PF_D32 )
		{
			d3d11Texture2DDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		}
		else
		{
			d3d11Texture2DDesc.BindFlags |= D3D11_BIND_RENDER_TARGET;
		}
	}

	ID3D11Device*	d3d11Device = ( ( CD3D11RHI* )GRHI )->GetD3D11Device();
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

		if ( d3d11Texture2DDesc.Format == DXGI_FORMAT_R32_TYPELESS )
		{
			// Use the typed shader resource view format corresponding to DXGI_FORMAT_D32_FLOAT
			d3d11ShaderResourceViewDesc.Format = DXGI_FORMAT_R32_FLOAT;
		}
		else if ( d3d11Texture2DDesc.Format == DXGI_FORMAT_R16_TYPELESS )
		{
			d3d11ShaderResourceViewDesc.Format = DXGI_FORMAT_R16_UNORM;
		}
		else if ( d3d11Texture2DDesc.Format == DXGI_FORMAT_R32G8X24_TYPELESS )
		{
			d3d11ShaderResourceViewDesc.Format = DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;
		}

		result = d3d11Device->CreateShaderResourceView( d3d11Texture2D, &d3d11ShaderResourceViewDesc, &d3d11ShaderResourceView );
		check( result == S_OK );
	}

	if ( InFlags & TCF_ResolveTargetable )
	{
		if ( d3d11Texture2DDesc.BindFlags & D3D11_BIND_RENDER_TARGET )
		{
			// Create a render-target-view for the texture if it is resolve targetable
			D3D11_RENDER_TARGET_VIEW_DESC					d3d11RenderTargetViewDesc;
			d3d11RenderTargetViewDesc.Format				= platformFormat;
			d3d11RenderTargetViewDesc.ViewDimension			= D3D11_RTV_DIMENSION_TEXTURE2D;
			d3d11RenderTargetViewDesc.Texture2D.MipSlice	= 0;

			result = d3d11Device->CreateRenderTargetView( d3d11Texture2D, &d3d11RenderTargetViewDesc, &d3d11RenderTargetView );
			check( result == S_OK );
		}
		else if ( d3d11Texture2DDesc.BindFlags & D3D11_BIND_DEPTH_STENCIL )
		{
			// Create a depth-stencil-view for the texture if it is resolve targetable
			D3D11_DEPTH_STENCIL_VIEW_DESC			d3d11DepthStencilViewDesc;
			memset( &d3d11DepthStencilViewDesc, 0, sizeof( D3D11_DEPTH_STENCIL_VIEW_DESC ) );
			d3d11DepthStencilViewDesc.Flags = 0;

			if ( d3d11Texture2DDesc.Format == DXGI_FORMAT_R32_TYPELESS )
			{
				// Use the typed depth format corresponding to DXGI_FORMAT_D32_FLOAT
				d3d11DepthStencilViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
			}
			//@ROCK - Dustin: Changed Shadow Depth to D32. We're not using the other 8 bits for shadows anyways!
			else if ( d3d11Texture2DDesc.Format == DXGI_FORMAT_R16_TYPELESS )
			{
				d3d11DepthStencilViewDesc.Format = DXGI_FORMAT_D16_UNORM;
			}
			//@Rock Bleees
			else if ( d3d11Texture2DDesc.Format == DXGI_FORMAT_R32G8X24_TYPELESS )
			{
				d3d11DepthStencilViewDesc.Format = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
			}
			else
			{
				d3d11DepthStencilViewDesc.Format = d3d11Texture2DDesc.Format;
			}

			d3d11DepthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
			d3d11DepthStencilViewDesc.Texture2D.MipSlice = 0;
			
			result = d3d11Device->CreateDepthStencilView( d3d11Texture2D, &d3d11DepthStencilViewDesc, &d3d11DepthStencilView );
			check( result == S_OK );
		}
	}
}

CD3D11Texture2DRHI::CD3D11Texture2DRHI( ID3D11Texture2D* InD3D11Texture2D, uint32 InSizeX, uint32 InSizeY, uint32 InNumMips, EPixelFormat InFormat, uint32 InFlags ) 
	: CD3D11TextureRHI( InSizeX, InSizeY, InNumMips, InFormat, InFlags )
	, d3d11Texture2D( InD3D11Texture2D )
{}

CD3D11Texture2DRHI::~CD3D11Texture2DRHI()
{
	d3d11Texture2D->Release();
}

void CD3D11Texture2DRHI::Lock( CBaseDeviceContextRHI* InDeviceContext, uint32 InMipIndex, bool InIsDataWrite, bool InIsUseCPUShadow, SLockedData& OutLockedData )
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
		checkMsg( subresource < numMips, TEXT( "CD3D11Texture2DRHI::Lock; ERROR! Subresource is out of range" ) );

		// If we're reading from the texture, or writing to a dynamic texture, we create a staging resource, 
		// copy the texture contents to it, and map it.
		
		// Create the staging texture
		CD3D11Texture2DRHI*				stagingResource = CreateStagingResource( InDeviceContext, mipSizeX, mipSizeY, subresource, InIsDataWrite, InIsUseCPUShadow );
		OutLockedData.stagingResource = stagingResource;

		// Map the staging resource, and return the mapped address
		ID3D11DeviceContext*			d3d11DeviceContext = ( ( CD3D11DeviceContext* )InDeviceContext )->GetD3D11DeviceContext();
		D3D11_MAPPED_SUBRESOURCE		d3d11MappedTexture2D;

#if DO_CHECK
		HRESULT							result = d3d11DeviceContext->Map( stagingResource->GetResource(), 0, InIsDataWrite ? D3D11_MAP_WRITE : D3D11_MAP_READ, 0, &d3d11MappedTexture2D );
		check( result == S_OK );
#else
		d3d11DeviceContext->Map( stagingResource->GetResource(), 0, InIsDataWrite ? D3D11_MAP_WRITE : D3D11_MAP_READ, 0, &d3d11MappedTexture2D );
#endif // DO_CHECK

		OutLockedData.data = ( byte* )d3d11MappedTexture2D.pData;
		OutLockedData.pitch = d3d11MappedTexture2D.RowPitch;
		OutLockedData.size = mipBytes;
		OutLockedData.isNeedFree = false;
	}
}

void CD3D11Texture2DRHI::Unlock( CBaseDeviceContextRHI* InDeviceContext, uint32 InMipIndex, SLockedData& InLockedData, bool InDiscardUpdate /*= false*/ )
{
	bool			isNeedUpdate = ( !InLockedData.stagingResource.IsValid() || flags & TCF_Dynamic ) && !InDiscardUpdate;
	if ( isNeedUpdate )
	{
		// Calculate the subresource index corresponding to the specified mip-map
		const uint32			subresource = D3D11CalcSubresource( InMipIndex, 0, numMips );
		checkMsg( subresource < numMips, TEXT( "CD3D11Texture2DRHI::Unlock; ERROR! Subresource is out of range" ) );

		// If we're writing, we need to update the subresource
		ID3D11DeviceContext*			d3d11DeviceContext = ( ( CD3D11DeviceContext* )InDeviceContext )->GetD3D11DeviceContext();
		d3d11DeviceContext->UpdateSubresource( d3d11Texture2D, subresource, nullptr, InLockedData.data, InLockedData.pitch, 0 );
	}

	if ( InLockedData.isNeedFree )
	{
		delete[] InLockedData.data;
		InLockedData.isNeedFree = false;
	}

	InLockedData.data = nullptr;
}

CD3D11Texture2DRHI* CD3D11Texture2DRHI::CreateStagingResource( class CBaseDeviceContextRHI* InDeviceContext, uint32 InMipSizeX, uint32 InMipSizeY, uint32 InSubresource, bool InIsDataWrite, bool InIsUseCPUShadow )
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

#if DO_CHECK
	HRESULT					result = ( ( CD3D11RHI* )GRHI )->GetD3D11Device()->CreateTexture2D( &d3d11StagingTextureDesc, nullptr, &d3d11StagingTexture2D );
	check( result == S_OK );
#else
	( ( CD3D11RHI* )GRHI )->GetD3D11Device()->CreateTexture2D( &d3d11StagingTextureDesc, nullptr, &d3d11StagingTexture2D );
#endif // DO_CHECK

#if !SHIPPING_BUILD
	D3D11SetDebugName( d3d11StagingTexture2D, "LockingStaging2DTexture" );
#endif // !SHIPPING_BUILD

	// Copy the mip-map data from the real resource into the staging resource
	if ( !InIsDataWrite || InIsUseCPUShadow )
	{
		ID3D11DeviceContext*		d3d11DeviceContext = ( ( CD3D11DeviceContext* )InDeviceContext )->GetD3D11DeviceContext();
		d3d11DeviceContext->CopySubresourceRegion( d3d11StagingTexture2D, 0, 0, 0, 0, d3d11Texture2D, InSubresource, nullptr );
	}

	return new CD3D11Texture2DRHI( d3d11StagingTexture2D, InMipSizeX, InMipSizeY, 1, format, TCF_None );
}