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
class CD3D11DeviceContext : public CBaseDeviceContextRHI
{
public:
	/**
	 * @brief Constructor
	 * 
	 * @param[in] InD3D11DeviceContext Pointer to D3D11 device context
	 */
											CD3D11DeviceContext( ID3D11DeviceContext* InD3D11DeviceContext );

	/**
	 * @brief Destructor
	 */
											~CD3D11DeviceContext();

	/**
	 * @brief Clear surface
	 *
	 * @param[in] InSurface Surface for rendering
	 * @param[in] InColor Color for clearing render target
	 */
	virtual void							ClearSurface( SurfaceRHIParamRef_t InSurface, const class CColor& InColor ) override;

	/**
	 * Clear depth stencil
	 * 
	 * @param[in] InSurface Surface for clear
	 * @param[in] InIsClearDepth Is need clear depth buffer
	 * @param[in] InIsClearStencil Is need clear stencil buffer
	 * @param[in] InDepthValue Clear the depth buffer with this value
	 * @param[in] InStencilValue Clear the stencil buffer with this value
	 */
	virtual void							ClearDepthStencil( SurfaceRHIParamRef_t InSurface, bool InIsClearDepth = true, bool InIsClearStencil = true, float InDepthValue = 1.f, uint8 InStencilValue = 0 ) override;

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
