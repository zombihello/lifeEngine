/**
 * @file
 * @addtogroup D3D11RHI D3D11RHI
 *
 * @authors: Yehor Pohuliaka
 * @date: 4/19/2021
 */

#ifndef D3D11VIEWPORT_H
#define D3D11VIEWPORT_H

#include <d3d11.h>

#include "Misc/Types.h"
#include "RHI/BaseViewportRHI.h"

/**
 * @ingroup D3D11RHI
 * @brief Class for work with DirectX 11 viewport
 */
class D3D11Viewport : public BaseViewportRHI
{
public:
	/**
	 * @brief Constructor
	 * 
	 * @param[in] InWindowHandle OS handle on window
	 * @param[in] InWidth Width of viewport
	 * @param[in] InHeight Height of viewport
	 */
											D3D11Viewport( void* InWindowHandle, uint32 InWidth, uint32 InHeight );

	/**
	 * @brief Destructor
	 */
											~D3D11Viewport();

	/**
	 * @brief Presents the swap chain
	 * 
	 * @param[in] InLockToVsync Is it necessary to block for Vsync
	 */
	virtual void							Present( bool InLockToVsync ) override;

	/**
	 * @brief Get back buffer
	 * @return Pointer to back buffer
	 */
	FORCEINLINE class D3D11Surface*			GetBackBuffer() const
	{
		return backBuffer;
	}

	/**
	 * @brief Get width
	 * @return Width of viewport
	 */
	virtual uint32							GetWidth() const override;

	/**
	 * @brief Get height
	 * @return Height of viewport
	 */
	virtual uint32							GetHeight() const override;

	/**
	 * @breif Get surface of viewport
	 * @return Pointer to surface of viewport
	 */
	virtual class BaseSurfaceRHI*			GetSurface() const override;

private:
	class D3D11Surface*			backBuffer;
	IDXGISwapChain*				dxgiSwapChain;				/**< DXGI swap chain */
	uint32						width;						/**< Width of viewport */
	uint32						height;						/**< Height of viewport */
};

#endif // !D3D11VIEWPORT_H
