#include "Core.h"
#include "Logger/LoggerMacros.h"
#include "D3D11RHI.h"
#include "D3D11ViewportRHI.h"

/**
 * Constructor
 */
D3D11RHI::D3D11RHI() :
	d3d11Device( nullptr ),
	d3d11DeviceContext( nullptr )
{}

/**
 * Destructor
 */
D3D11RHI::~D3D11RHI()
{
	// BG yehor.pohuliaka - Add release objects
}

/**
 * Initialize RHI
 */
void D3D11RHI::Init( bool InIsEditor )
{
	if ( d3d11Device )			return;

	uint32					deviceFlags = 0;

	// In Direct3D 11, if you are trying to create a hardware or a software device, set pAdapter != NULL which constrains the other inputs to be:
	//		DriverType must be D3D_DRIVER_TYPE_UNKNOWN 
	//		Software must be NULL. 
	D3D_DRIVER_TYPE			driverType = D3D_DRIVER_TYPE_UNKNOWN;

#if !SHIPPING_BUILD
	deviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif // !SHIPPING_BUILD

	// Create DXGI factory and adapter
	HRESULT		result = CreateDXGIFactory( __uuidof( IDXGIFactory ), ( void** ) &dxgiFactory );
	check( result == S_OK );

	uint32			currentAdapter = 0;
	while ( dxgiFactory->EnumAdapters( currentAdapter, &dxgiAdapter ) == DXGI_ERROR_NOT_FOUND )
	{
		++currentAdapter;
	}
	checkMsg( dxgiAdapter, "GPU adapter not found" );

	D3D_FEATURE_LEVEL				maxFeatureLevel = D3D_FEATURE_LEVEL_11_0;
	D3D_FEATURE_LEVEL				featureLevel;
	result = D3D11CreateDevice( dxgiAdapter, driverType, nullptr, deviceFlags, &maxFeatureLevel, 1, D3D11_SDK_VERSION, &d3d11Device, &featureLevel, &d3d11DeviceContext );
	check( result == S_OK );

	// Print info adapter
	DXGI_ADAPTER_DESC				adapterDesc;
	dxgiAdapter->GetDesc( &adapterDesc );

	LE_LOG( LT_Log, LC_Init, TEXT( "Found D3D11 adapter: %s" ), adapterDesc.Description );
	LE_LOG( LT_Log, LC_Init, TEXT( "Adapter has %uMB of dedicated video memory, %uMB of dedicated system memory, and %uMB of shared system memory" ),
			adapterDesc.DedicatedVideoMemory / ( 1024 * 1024 ),
			adapterDesc.DedicatedSystemMemory / ( 1024 * 1024 ),
			adapterDesc.SharedSystemMemory / ( 1024 * 1024 ) );
}

/**
 * Create viewport
 */
class BaseViewportRHI* D3D11RHI::CreateViewport( void* InWindowHandle, uint32 InWidth, uint32 InHeight )
{
	return new D3D11ViewportRHI( InWindowHandle, InWidth, InHeight );
}