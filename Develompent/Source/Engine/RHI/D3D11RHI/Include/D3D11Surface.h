/**
 * @file
 * @addtogroup D3D11RHI D3D11RHI
 *
 * @authors: Yehor Pohuliaka
 * @date: 4/22/2021
 */

#ifndef D3D11SURFACE_H
#define D3D11SURFACE_H

#include <d3d11.h>

#include "Core.h"

/**
 * @ingroup D3D11RHI
 * @brief Class for work with DirectX 11 surface
 */
class D3D11Surface
{
public:
	/**
	 * @brief Constructor
	 * 
	 * @param[in] InRenderTargetView Render target view
	 */
												D3D11Surface( ID3D11RenderTargetView* InRenderTargetView );

	/**
	 * @brief Destructor
	 */
												~D3D11Surface();

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