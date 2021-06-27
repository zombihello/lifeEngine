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

#endif // !D3D11BUFFERRHI_H