#include "Containers/String.h"
#include "Containers/StringConv.h"

#include "D3D11RHI.h"
#include "D3D11Buffer.h"

// ------------------------------------
// VERTEX BUFFER
// ------------------------------------

/**
 * Constructor
 */
FD3D11VertexBufferRHI::FD3D11VertexBufferRHI( uint32 InUsage, uint32 InSize, const byte* InData, const tchar* InBufferName ) :
	FBaseVertexBufferRHI( InUsage, InSize ),
	d3d11Buffer( nullptr )
{
	// Explicitly check that the size is nonzero before allowing create vertex buffer to opaquely fail
	check( InSize > 0 );

	D3D11_BUFFER_DESC		bufferDesc;
	bufferDesc.ByteWidth			= InSize;
	bufferDesc.Usage				= ( InUsage & RUF_AnyDynamic ) ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags			= D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags		= ( InUsage & RUF_AnyDynamic ) ? D3D11_CPU_ACCESS_WRITE : 0;
	bufferDesc.MiscFlags			= 0;
	bufferDesc.StructureByteStride	= 0;

	// If a data was provided for the resource, create the resource pre-populated
	D3D11_SUBRESOURCE_DATA		initData;
	D3D11_SUBRESOURCE_DATA*		pInitData = nullptr;
	if ( InData )
	{
		initData.pSysMem = InData;
		initData.SysMemPitch = InSize;
		initData.SysMemSlicePitch = 0;
		pInitData = &initData;
	}

	// Creating DirectX 11 vertex buffer
	ID3D11Device*		device = static_cast< FD3D11RHI* >( GRHI )->GetD3D11Device();
	HRESULT				result = device->CreateBuffer( &bufferDesc, pInitData, &d3d11Buffer );
	check( result == S_OK );

	d3d11Buffer->AddRef();

#if !SHIPPING_BUILD
	D3D11SetDebugName( d3d11Buffer, TCHAR_TO_ANSI( FString::Format( TEXT( "%s[VERTEX_BUFFER]" ), InBufferName ) ) );
#endif // !SHIPPING_BUILD
}

/**
 * Destructor
 */
FD3D11VertexBufferRHI::~FD3D11VertexBufferRHI()
{
	if ( d3d11Buffer )
	{
		d3d11Buffer->Release();
	}
}

// ------------------------------------
// INDEX BUFFER
// ------------------------------------

/**
 * Constructor
 */
FD3D11IndexBufferRHI::FD3D11IndexBufferRHI( uint32 InUsage, uint32 InStride, uint32 InSize, const byte* InData, const tchar* InBufferName ) :
	FBaseIndexBufferRHI( InUsage, InStride, InSize ),
	d3d11Buffer( nullptr )
{	
	// Explicitly check that the size is nonzero before allowing create index buffer to opaquely fail
	check( InSize > 0 );

	D3D11_BUFFER_DESC		bufferDesc;
	bufferDesc.ByteWidth = InSize;
	bufferDesc.Usage = ( InUsage & RUF_AnyDynamic ) ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bufferDesc.CPUAccessFlags = ( InUsage & RUF_AnyDynamic ) ? D3D11_CPU_ACCESS_WRITE : 0;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;

	// If a data was provided for the resource, create the resource pre-populated
	D3D11_SUBRESOURCE_DATA		initData;
	D3D11_SUBRESOURCE_DATA* pInitData = nullptr;
	if ( InData )
	{
		initData.pSysMem = InData;
		initData.SysMemPitch = InSize;
		initData.SysMemSlicePitch = 0;
		pInitData = &initData;
	}

	// Creating DirectX 11 vertex buffer
	ID3D11Device*		device = static_cast< FD3D11RHI* >( GRHI )->GetD3D11Device();
	HRESULT				result = device->CreateBuffer( &bufferDesc, pInitData, &d3d11Buffer );
	check( result == S_OK );

	d3d11Buffer->AddRef();

#if !SHIPPING_BUILD
	D3D11SetDebugName( d3d11Buffer, TCHAR_TO_ANSI( FString::Format( TEXT( "%s[INDEX_BUFFER]" ), InBufferName ) ) );
#endif // !SHIPPING_BUILD
}

/**
 * Destructor
 */
FD3D11IndexBufferRHI::~FD3D11IndexBufferRHI()
{
	if ( d3d11Buffer )
	{
		d3d11Buffer->Release();
	}
}