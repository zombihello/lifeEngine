/**
 * @file
 * @addtogroup D3D11RHI D3D11RHI
 *
 * Copyright BSOD-Games, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef D3D11SURFACE_H
#define D3D11SURFACE_H

#include <d3d11.h>

#include "Core.h"
#include "RHI/BaseSurfaceRHI.h"

/**
 * @ingroup D3D11RHI
 * @brief Class for work with DirectX 11 surface
 */
class FD3D11Surface : public FBaseSurfaceRHI
{
public:
	/**
	 * @brief Constructor
	 * 
	 * @param[in] InRenderTargetView Render target view
	 */
												FD3D11Surface( ID3D11RenderTargetView* InRenderTargetView );

	/**
	 * @brief Destructor
	 */
												~FD3D11Surface();

	/**
	 * @brief Get render target view
	 */
	FORCEINLINE ID3D11RenderTargetView*			GetD3D11RenderTargetView() const
	{
		return d3d11RenderTargetView;
	}

private:
	ID3D11RenderTargetView*				d3d11RenderTargetView;		/**< A view of the surface as a render target */
};

#endif // D3D11SURFACE_H