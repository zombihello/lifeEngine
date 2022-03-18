/**
 * @file
 * @addtogroup D3D11RHI D3D11RHI
 *
 * Copyright BSOD-Games, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef D3D11BUFFERRHI_H
#define D3D11BUFFERRHI_H

#include <d3d11.h>

#include "RHI/BaseBufferRHI.h"

/**
 * @ingroup D3D11RHI
 * Size of the default constant buffer
 */
#define MAX_GLOBAL_CONSTANT_BUFFER_SIZE		4096

/**
 * @ingroup D3D11RHI
 * Enumeration of constant buffer slots
 * @warning These offsets must match the cbuffer register definitions in Common.hlsl
 */
enum ED3D11ShaderOffsetBuffer
{
	SOB_ShaderConstants,	/**< Global constants in shader */
	SOB_GlobalConstants,	/**< Vertex shader view-dependent constants set in RHISetViewParameters */
	SOB_Max					/**< Max count constant buffer slots */
};

/**
 * @ingroup D3D11RHI
 * Sizes of constant buffers defined in ED3D11ShaderOffsetBuffer
 */
extern const uint32 GConstantBufferSizes[ SOB_Max ];

/**
 * @ingroup D3D11RHI
 * @brief Class for work with DirectX 11 vertex buffer
 */
class FD3D11VertexBufferRHI : public FBaseVertexBufferRHI
{
public:
	/**
	 * @brief Constructor
	 * @param[in] InUsage Usage flags
	 * @param[in] InSize Size of buffer
	 * @param[in] InData Pointer to data for buffer
	 * @param[in] InBufferName Buffer name
	 */
	FD3D11VertexBufferRHI( uint32 InUsage, uint32 InSize, const byte* InData, const tchar* InBufferName );

	/**
	 * @brief Destructor
	 */
	virtual ~FD3D11VertexBufferRHI();

	/**
	 * @brief Get pointer to DirectX 11 buffer
	 * @return Pointer to DirectX 11 buffer
	 */
	FORCEINLINE ID3D11Buffer* GetD3D11Buffer() const 
	{
		return d3d11Buffer;
	}

private:
	ID3D11Buffer*			d3d11Buffer;		/**< Pointer to DirectX 11 buffer */
};

/**
 * @ingroup D3D11RHI
 * @brief Class for work with DirectX 11 index buffer
 */
class FD3D11IndexBufferRHI : public FBaseIndexBufferRHI
{
public:
	/**
	 * @brief Constructor
	 * @param[in] InUsage Usage flags
	 * @param[in] InStride Stride of struct
	 * @param[in] InSize Size of buffer
	 * @param[in] InData Pointer to data for buffer
	 * @param[in] InBufferName Buffer name
	 */
	FD3D11IndexBufferRHI( uint32 InUsage, uint32 InStride, uint32 InSize, const byte* InData, const tchar* InBufferName );

	/**
	 * @brief Destructor
	 */
	virtual ~FD3D11IndexBufferRHI();

	/**
	 * @brief Get pointer to DirectX 11 buffer
	 * @return Pointer to DirectX 11 buffer
	 */
	FORCEINLINE ID3D11Buffer* GetD3D11Buffer() const
	{
		return d3d11Buffer;
	}

private:
	ID3D11Buffer*			d3d11Buffer;		/**< Pointer to DirectX 11 buffer */
};

/**
 * @ingroup D3D11RHI
 * @brief Class for work with DirectX 11 constant buffer
 */
class FD3D11ConstantBuffer : public FBaseResourceRHI
{
public:
	/**
	 * Constructor
	 * 
	 * @param[in] InSize Size buffer
	 * @param[in] InBufferName Buffer name
	 */
	FD3D11ConstantBuffer( uint32 InSize, const tchar* InBufferName );

	/**
	 * Destructor
	 */
	virtual ~FD3D11ConstantBuffer();

	/**
	 * Update data in buffer
	 * 
	 * @param[in] InData Pointer to data
	 * @param[in] InOffset Offset in buffer
	 * @param[in] InSize Size data to update
	 */
	void Update( const byte* InData, uint32 InOffset, uint32 InSize );

	/**
	 * Flush data to GPU
	 * 
	 * @param[in] InDeviceContext Device context
	 */
	void CommitConstantsToDevice( class FD3D11DeviceContext* InDeviceContext );

	/**
	 * Clear data
	 */
	void Clear();

	/**
	 * @brief Get pointer to DirectX 11 buffer
	 * @return Pointer to DirectX 11 buffer
	 */
	FORCEINLINE ID3D11Buffer* GetD3D11Buffer() const
	{
		return d3d11Buffer;
	}

private:
	bool				isNeedCommit;			/**< If true - need commit buffer to device */
	ID3D11Buffer*		d3d11Buffer;			/**< Pointer to DirectX 11 buffer */
	uint32				size;					/**< Size buffer */
	byte*				shadowData;				/**< Local version of buffer, which is updated and uploaded to the GPU at once */
	uint32				currentUpdateSize;		/**< Size to update buffer */
};

#endif // !D3D11BUFFERRHI_H