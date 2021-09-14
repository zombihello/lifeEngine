/**
 * @file
 * @addtogroup D3D11RHI D3D11RHI
 *
 * Copyright BSOD-Games, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
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
class FD3D11Viewport : public FBaseViewportRHI
{
public:
	/**
	 * @brief Constructor
	 * 
	 * @param[in] InWindowHandle OS handle on window
	 * @param[in] InWidth Width of viewport
	 * @param[in] InHeight Height of viewport
	 */
											FD3D11Viewport( void* InWindowHandle, uint32 InWidth, uint32 InHeight );

	/**
	 * @brief Destructor
	 */
											~FD3D11Viewport();

	/**
	 * @brief Presents the swap chain
	 * 
	 * @param[in] InLockToVsync Is it necessary to block for Vsync
	 */
	virtual void							Present( bool InLockToVsync ) override;

	/**
	 * Resize viewport
	 *
	 * @param[in] InWidth New width
	 * @param[in] InHeight New height
	 * @param[in] InIsFullscreen Is fullscreen
	 */
	virtual void							Resize( uint32 InWidth, uint32 InHeight ) override;

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
	virtual FSurfaceRHIRef					GetSurface() const override;

private:
	FSurfaceRHIRef				backBuffer;					/**< Pointer to back buffer */
	IDXGISwapChain*				dxgiSwapChain;				/**< DXGI swap chain */
	uint32						width;						/**< Width of viewport */
	uint32						height;						/**< Height of viewport */
};

#endif // !D3D11VIEWPORT_H
