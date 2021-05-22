#include "Core.h"
#include "Logger/LoggerMacros.h"
#include "D3D11RHI.h"
#include "D3D11Viewport.h"
#include "D3D11DeviceContext.h"
#include "D3D11Surface.h"
#include "D3D11Shader.h"

#include "D3D11ImGUI.h"

/**
 * Constructor
 */
FD3D11RHI::FD3D11RHI() :
	isInitialize( false ),
	immediateContext( nullptr ),
	d3d11Device( nullptr )
{}

/**
 * Destructor
 */
FD3D11RHI::~FD3D11RHI()
{
	Destroy();
}

/**
 * Initialize RHI
 */
void FD3D11RHI::Init( bool InIsEditor )
{
	if ( IsInitialize() )			return;

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
	ID3D11DeviceContext*			d3d11DeviceContext = nullptr;
	result = D3D11CreateDevice( dxgiAdapter, driverType, nullptr, deviceFlags, &maxFeatureLevel, 1, D3D11_SDK_VERSION, &d3d11Device, &featureLevel, &d3d11DeviceContext );
	check( result == S_OK );

	immediateContext = new FD3D11DeviceContext( d3d11DeviceContext );
	
	// Print info adapter
	DXGI_ADAPTER_DESC				adapterDesc;
	dxgiAdapter->GetDesc( &adapterDesc );

	LE_LOG( LT_Log, LC_Init, TEXT( "Found D3D11 adapter: %s" ), adapterDesc.Description );
	LE_LOG( LT_Log, LC_Init, TEXT( "Adapter has %uMB of dedicated video memory, %uMB of dedicated system memory, and %uMB of shared system memory" ),
			adapterDesc.DedicatedVideoMemory / ( 1024 * 1024 ),
			adapterDesc.DedicatedSystemMemory / ( 1024 * 1024 ),
			adapterDesc.SharedSystemMemory / ( 1024 * 1024 ) );

	isInitialize = true;
}

/**
 * Is initialized RHI
 */
bool FD3D11RHI::IsInitialize() const
{
	return isInitialize;
}

/**
 * Destroy RHI
 */
void FD3D11RHI::Destroy()
{
	if ( !isInitialize )		return;

	delete immediateContext;
	d3d11Device->Release();
	dxgiAdapter->Release();
	dxgiFactory->Release();

	immediateContext = nullptr;
	d3d11Device = nullptr;
	dxgiAdapter = nullptr;
	dxgiFactory = nullptr;
}

/**
 * Create viewport
 */
class FBaseViewportRHI* FD3D11RHI::CreateViewport( void* InWindowHandle, uint32 InWidth, uint32 InHeight )
{
	return new FD3D11Viewport( InWindowHandle, InWidth, InHeight );
}

/**
 * Create vertex shader
 */
FVertexShaderRHIRef FD3D11RHI::CreateVertexShader( const byte* InData, uint32 InSize )
{
	return new FD3D11VertexShaderRHI( InData, InSize );
}

/**
 * Create hull shader
 */
FHullShaderRHIRef FD3D11RHI::CreateHullShader( const byte* InData, uint32 InSize )
{
	return new FD3D11HullShaderRHI( InData, InSize );
}

/**
 * Create domain shader
 */
FDomainShaderRHIRef FD3D11RHI::CreateDomainShader( const byte* InData, uint32 InSize )
{
	return new FD3D11DomainShaderRHI( InData, InSize );
}

/**
 * Create pixel shader
 */
FPixelShaderRHIRef FD3D11RHI::CreatePixelShader( const byte* InData, uint32 InSize )
{
	return new FD3D11PixelShaderRHI( InData, InSize );
}

/**
 * Create geometry shader
 */
FGeometryShaderRHIRef FD3D11RHI::CreateGeometryShader( const byte* InData, uint32 InSize )
{
	return new FD3D11GeometryShaderRHI( InData, InSize );
}

/**
 * Get device context
 */
class FBaseDeviceContextRHI* FD3D11RHI::GetImmediateContext() const
{
	return immediateContext;
}

/**
 * Set viewport
 */
void FD3D11RHI::SetViewport( class FBaseDeviceContextRHI* InDeviceContext, uint32 InMinX, uint32 InMinY, float InMinZ, uint32 InMaxX, uint32 InMaxY, float InMaxZ )
{
	D3D11_VIEWPORT			d3d11Viewport = { ( float )InMinX, ( float )InMinY, ( float )InMaxX - InMinX, ( float )InMaxY - InMinY, ( float )InMinZ, InMaxZ };
	if ( d3d11Viewport.Width > 0 && d3d11Viewport.Height > 0 )
	{
		static_cast< FD3D11DeviceContext* >( InDeviceContext )->GetD3D11DeviceContext()->RSSetViewports( 1, &d3d11Viewport );
	}
}

/**
 * Begin drawing viewport
 */
void FD3D11RHI::BeginDrawingViewport( class FBaseDeviceContextRHI* InDeviceContext, class FBaseViewportRHI* InViewport )
{
	check( InDeviceContext && InViewport );
	FD3D11DeviceContext*			deviceContext = ( FD3D11DeviceContext* )InDeviceContext;
	FD3D11Viewport*					viewport = ( FD3D11Viewport* )InViewport;

	ID3D11RenderTargetView*			d3d11RenderTargetView = viewport->GetBackBuffer()->GetD3D11RenderTargetView();
	deviceContext->GetD3D11DeviceContext()->OMSetRenderTargets( 1, &d3d11RenderTargetView, nullptr );
	SetViewport( InDeviceContext, 0, 0, 0.f, viewport->GetWidth(), viewport->GetHeight(), 1.f );
}

/**
 * End drawing viewport
 */
void FD3D11RHI::EndDrawingViewport( class FBaseDeviceContextRHI* InDeviceContext, class FBaseViewportRHI* InViewport, bool InIsPresent, bool InLockToVsync )
{
	check( InViewport );

	if ( InIsPresent )
	{
		InViewport->Present( InLockToVsync );
	}
}

#if WITH_EDITOR
/**
 * Initialize ImGUI
 */
bool FD3D11RHI::InitImGUI( class FBaseDeviceContextRHI* InDeviceContext )
{
	check( d3d11Device && InDeviceContext );
	FD3D11DeviceContext*		deviceContext = ( FD3D11DeviceContext* )InDeviceContext;
	return ImGui_ImplDX11_Init( d3d11Device, deviceContext->GetD3D11DeviceContext() );
}

/**
 * Shutdown render of ImGUI
 */
void FD3D11RHI::ShutdownImGUI( class FBaseDeviceContextRHI* InDeviceContext )
{
	ImGui_ImplDX11_Shutdown();
}

/**
 * Begin drawing ImGUI
 */
void FD3D11RHI::BeginDrawingImGUI( class FBaseDeviceContextRHI* InDeviceContext )
{
	ImGui_ImplDX11_NewFrame();
}

/**
 * End drawing ImGUI
 */
void FD3D11RHI::EndDrawingImGUI( class FBaseDeviceContextRHI* InDeviceContext )
{
	ImGui_ImplDX11_RenderDrawData( ImGui::GetDrawData() );
}
#endif // WITH_EDITOR

/**
 * Set debug name fore DirectX 11 resource
 */
void D3D11SetDebugName( ID3D11DeviceChild* InObject, achar* InName )
{
	if ( !InName )			return;
	InObject->SetPrivateData( WKPDID_D3DDebugObjectName, ( uint32 )strlen( InName ), InName );
}

/**
 * Set debug name fore DirectX 11 resource
 */
void D3D11SetDebugName( IDXGIObject* InObject, achar* InName )
{
	if ( !InName )			return;
	InObject->SetPrivateData( WKPDID_D3DDebugObjectName, ( uint32 )strlen( InName ), InName );
}