#include "Containers/String.h"
#include "Containers/StringConv.h"

#include "CPP_GlobalConstantBuffers.hlsl"
#include "D3D11RHI.h"
#include "D3D11Buffer.h"
#include "D3D11DeviceContext.h"

/** Macro for enable fill full constant buffer for fix bug with updating sub region in buffer */
#define D3D11_FILLFULL_CONSTANTBUFFER 1

// ------------------------------------
// GLOBALS
// ------------------------------------

const uint32 GConstantBufferSizes[ SOB_Max ] =
{
	// CBs must be a multiple of 16
	Align( ( uint32 )MAX_GLOBAL_CONSTANT_BUFFER_SIZE, 16 ),
	Align( ( uint32 )sizeof( SGlobalConstantBufferContents ), 16 ),
};

// ------------------------------------
// VERTEX BUFFER
// ------------------------------------

/*
==================
CD3D11VertexBufferRHI::CD3D11VertexBufferRHI
==================
*/
CD3D11VertexBufferRHI::CD3D11VertexBufferRHI( uint32 InUsage, uint32 InSize, const byte* InData, const tchar* InBufferName ) :
	CBaseVertexBufferRHI( InUsage, InSize ),
	d3d11Buffer( nullptr )
{
	// Explicitly Assert that the size is nonzero before allowing create vertex buffer to opaquely fail
	Assert( InSize > 0 );

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
	ID3D11Device*		device = static_cast< CD3D11RHI* >( g_RHI )->GetD3D11Device();

#if ENABLED_ASSERT
	HRESULT				result = device->CreateBuffer( &bufferDesc, pInitData, &d3d11Buffer );
	Assert( result == S_OK );
#else
	device->CreateBuffer( &bufferDesc, pInitData, &d3d11Buffer );
#endif // ENABLED_ASSERT

#if !SHIPPING_BUILD
	D3D11SetDebugName( d3d11Buffer, TCHAR_TO_ANSI( CString::Format( TEXT( "%s[VERTEX_BUFFER]" ), InBufferName ).c_str() ) );
#endif // !SHIPPING_BUILD
}

/*
==================
CD3D11VertexBufferRHI::~CD3D11VertexBufferRHI
==================
*/
CD3D11VertexBufferRHI::~CD3D11VertexBufferRHI()
{
	if ( d3d11Buffer )
	{
		d3d11Buffer->Release();
	}
}

// ------------------------------------
// INDEX BUFFER
// ------------------------------------

/*
==================
CD3D11IndexBufferRHI::CD3D11IndexBufferRHI
==================
*/
CD3D11IndexBufferRHI::CD3D11IndexBufferRHI( uint32 InUsage, uint32 InStride, uint32 InSize, const byte* InData, const tchar* InBufferName ) :
	CBaseIndexBufferRHI( InUsage, InStride, InSize ),
	d3d11Buffer( nullptr )
{	
	// Explicitly Assert that the size is nonzero before allowing create index buffer to opaquely fail
	Assert( InSize > 0 );

	D3D11_BUFFER_DESC		bufferDesc;
	bufferDesc.ByteWidth = InSize;
	bufferDesc.Usage = ( InUsage & RUF_AnyDynamic ) ? D3D11_USAGE_DYNAMIC : D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bufferDesc.CPUAccessFlags = ( InUsage & RUF_AnyDynamic ) ? D3D11_CPU_ACCESS_WRITE : 0;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;

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

	// Creating DirectX 11 index buffer
	ID3D11Device*		device = static_cast< CD3D11RHI* >( g_RHI )->GetD3D11Device();

#if ENABLED_ASSERT
	HRESULT				result = device->CreateBuffer( &bufferDesc, pInitData, &d3d11Buffer );
	Assert( result == S_OK );
#else
	device->CreateBuffer( &bufferDesc, pInitData, &d3d11Buffer );
#endif // ENABLED_ASSERT

#if !SHIPPING_BUILD
	D3D11SetDebugName( d3d11Buffer, TCHAR_TO_ANSI( CString::Format( TEXT( "%s[INDEX_BUFFER]" ), InBufferName ).c_str() ) );
#endif // !SHIPPING_BUILD
}

/*
==================
CD3D11IndexBufferRHI::~CD3D11IndexBufferRHI
==================
*/
CD3D11IndexBufferRHI::~CD3D11IndexBufferRHI()
{
	if ( d3d11Buffer )
	{
		d3d11Buffer->Release();
	}
}

// ------------------------------------
// CONSTANT BUFFER
// ------------------------------------

/*
==================
CD3D11ConstantBuffer::CD3D11ConstantBuffer
==================
*/
CD3D11ConstantBuffer::CD3D11ConstantBuffer( uint32 InSize, const tchar* InBufferName ) :
	isNeedCommit( false ),
	d3d11Buffer( nullptr ),
	size( InSize ),
	shadowData( nullptr ),
	currentUpdateSize( 0 )
{
	// Explicitly Assert that the size is nonzero before allowing create constant buffer to opaquely fail
	Assert( size > 0 );
	shadowData = new byte[ size ];
	Clear();

	D3D11_BUFFER_DESC			bufferDesc;
	bufferDesc.ByteWidth		= size;
	bufferDesc.Usage			= D3D11_USAGE_DYNAMIC;
	bufferDesc.CPUAccessFlags	= D3D11_CPU_ACCESS_WRITE;
	bufferDesc.BindFlags		= D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.MiscFlags		= 0;

	// Initialize struct for fill data buffer
	D3D11_SUBRESOURCE_DATA		initData;
	D3D11_SUBRESOURCE_DATA*		pInitData = nullptr;
	initData.pSysMem			= shadowData;
	initData.SysMemPitch		= size;
	initData.SysMemSlicePitch	= 0;
	pInitData					= &initData;

	// Creating DirectX 11 constant buffer
	ID3D11Device*		device = static_cast< CD3D11RHI* >( g_RHI )->GetD3D11Device();

#if ENABLED_ASSERT
	HRESULT				result = device->CreateBuffer( &bufferDesc, pInitData, &d3d11Buffer );
	Assert( result == S_OK );
#else
	device->CreateBuffer( &bufferDesc, pInitData, &d3d11Buffer );
#endif // ENABLED_ASSERT

#if !SHIPPING_BUILD
	D3D11SetDebugName( d3d11Buffer, TCHAR_TO_ANSI( CString::Format( TEXT( "%s[CONSTANT_BUFFER]" ), InBufferName ).c_str() ) );
#endif // !SHIPPING_BUILD
}

/*
==================
CD3D11ConstantBuffer::~CD3D11ConstantBuffer
==================
*/
CD3D11ConstantBuffer::~CD3D11ConstantBuffer()
{
	// Delete shadow data
	if ( shadowData )
	{
		delete[] shadowData;
	}

	// Delete D3D11 buffer
	if ( d3d11Buffer )
	{
		d3d11Buffer->Release();
	}
}

/*
==================
CD3D11ConstantBuffer::Update
==================
*/
void CD3D11ConstantBuffer::Update( const byte* InData, uint32 InOffset, uint32 InSize )
{
	memcpy( shadowData + InOffset, InData, InSize );
	currentUpdateSize = Max( currentUpdateSize, ( uint32 )InOffset + InSize );
	isNeedCommit = true;
}

/*
==================
CD3D11ConstantBuffer::CommitConstantsToDevice
==================
*/
void CD3D11ConstantBuffer::CommitConstantsToDevice( class CD3D11DeviceContext* InDeviceContext )
{
	if ( !isNeedCommit )
	{
		return;
	}

	Assert( InDeviceContext );
	ID3D11DeviceContext*			d3d11DeviceContext = InDeviceContext->GetD3D11DeviceContext();
	D3D11_MAPPED_SUBRESOURCE		d3d11Mapped;

	d3d11DeviceContext->Map( d3d11Buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &d3d11Mapped );

#if !D3D11_FILLFULL_CONSTANTBUFFER
	memcpy( ( byte* )d3d11Mapped.pData, shadowData, currentUpdateSize );
#else
	memcpy( ( byte* )d3d11Mapped.pData, shadowData, size );
#endif // !D3D11_FILLFULL_CONSTANTBUFFER

	d3d11DeviceContext->Unmap( d3d11Buffer, 0 );

	isNeedCommit = false;
	currentUpdateSize = 0;
}

/*
==================
CD3D11ConstantBuffer::Clear
==================
*/
void CD3D11ConstantBuffer::Clear()
{
	if ( !shadowData )		return;

	Sys_Memzero( shadowData, size );
	isNeedCommit = true;
	currentUpdateSize = size;
}