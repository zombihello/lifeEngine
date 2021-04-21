/**
 * @file
 * @addtogroup D3D11RHI D3D11RHI
 *
 * @authors: Yehor Pohuliaka
 * @date: 4/19/2021
 */

#ifndef D3D11VIEWPORTRHI_H
#define D3D11VIEWPORTRHI_H

#include <d3d11.h>

#include "Misc/Types.h"
#include "RHI/BaseViewportRHI.h"

/**
 * @ingroup D3D11RHI
 * @brief Class for work with DirectX 11 viewport
 */
class D3D11ViewportRHI : public BaseViewportRHI
{
public:
	/**
	 * @brief Constructor
	 * 
	 * @param[in] InWindowHandle OS handle on window
	 * @param[in] InWidth Width of viewport
	 * @param[in] InHeight Height of viewport
	 */
								D3D11ViewportRHI( void* InWindowHandle, uint32 InWidth, uint32 InHeight );

	/**
	 * @brief Destructor
	 */
								~D3D11ViewportRHI();

	/**
	 * @brief Presents the swap chain
	 */
	virtual void				Present() override;

private:
	ID3D11RenderTargetView*		d3d11RenderTargetView;		/**< D3D11 Render target view */
	IDXGISwapChain*				dxgiSwapChain;				/**< DXGI swap chain */
};

#endif // !D3D11VIEWPORTRHI_H
