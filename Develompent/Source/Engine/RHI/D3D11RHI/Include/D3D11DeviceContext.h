/**
 * @file
 * @addtogroup D3D11RHI D3D11RHI
 *
 * Copyright BSOD-Games, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef D3D11DEVICECONTEXT_H
#define D3D11DEVICECONTEXT_H

#include <d3d11.h>

#include "RHI/BaseDeviceContextRHI.h"

/**
 * @ingroup D3D11RHI
 * @brief Class for work with DirectX 11 device context
*/
class FD3D11DeviceContext : public FBaseDeviceContextRHI
{
public:
	/**
	 * @brief Constructor
	 * 
	 * @param[in] InD3D11DeviceContext Pointer to D3D11 device context
	 */
											FD3D11DeviceContext( ID3D11DeviceContext* InD3D11DeviceContext );

	/**
	 * @brief Destructor
	 */
											~FD3D11DeviceContext();

	/**
	 * @brief Clear surface
	 *
	 * @param[in] InSurface Surface for rendering
	 * @param[in] InColor Color for clearing render target
	 */
	virtual void							ClearSurface( FSurfaceRHIParamRef InSurface, const class FColor& InColor ) override;

	/**
	 * @brief Get D3D11 device context
	 * @return Pointer to D3D11 device context
	 */
	FORCEINLINE ID3D11DeviceContext*		GetD3D11DeviceContext() const
	{
		return d3d11DeviceContext;
	}

private:
	ID3D11DeviceContext*			d3d11DeviceContext;			/**< D3D11 Device context */
};

#endif // !D3D11DEVICECONTEXT_H
