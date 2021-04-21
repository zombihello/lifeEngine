/**
 * @file
 * @addtogroup D3D11RHI D3D11RHI
 *
 * @authors: Yehor Pohuliaka
 * @date: 4/19/2021
 */

#ifndef D3D11RHI_H
#define D3D11RHI_H

#include <d3d11.h>

#include "Misc/EngineGlobals.h"
#include "RHI/BaseRHI.h"

/**
 * @ingroup D3D11RHI
 * @brief Main class of DirectX 11
 */
class D3D11RHI : public BaseRHI
{
public:
	/**
	 * @brief Constructor
	 */
													D3D11RHI();

	/**
	 * @brief Destructor
	 */
													~D3D11RHI();

	/**
	 * @brief Initialize RHI
	 *
	 * @param[in] InIsEditor Is current application editor
	 */
	virtual void									Init( bool InIsEditor ) override;

	/**
	 * @brief Create viewport
	 *
	 * @param[in] InWindowHandle OS handle on window
	 * @param[in] InWidth Width of viewport
	 * @param[in] InHeight Height of viewport
	 * @return Pointer on viewport
	 */
	virtual class BaseViewportRHI*					CreateViewport( void* InWindowHandle, uint32 InWidth, uint32 InHeight ) override;

	/**
	 * @brief Begin drawing viewport
	 *
	 * @param[in] InDeviceContext Device context
	 * @param[in] InViewport Viewport
	 */
	virtual void									BeginDrawingViewport( class BaseDeviceContextRHI* InDeviceContext, class BaseViewportRHI* InViewport ) override;

	/**
	 * @brief End drawing viewport
	 *
	 * @param[in] InDeviceContext Device context
	 * @param[in] InViewport Viewport
	 * @param[in] InIsPresent Whether to display the frame on the screen
	 * @param[in] InLockToVsync Is it necessary to block for Vsync
	 */
	virtual void									EndDrawingViewport( class BaseDeviceContextRHI* InDeviceContext, class BaseViewportRHI* InViewport, bool InIsPresent, bool InLockToVsync ) override;

	/**
	 * @brief Set viewport
	 *
	 * @param[in] InDeviceContext Device context
	 * @param[in] InMinX Min x
	 * @param[in] InMinY Min y
	 * @param[in] InMinZ Min z
	 * @param[in] InMaxX Max x
	 * @param[in] InMaxY Max y
	 * @param[in] InMaxZ Max z
	 */
	virtual void									SetViewport( class BaseDeviceContextRHI* InDeviceContext, uint32 InMinX, uint32 InMinY, float InMinZ, uint32 InMaxX, uint32 InMaxY, float InMaxZ ) override;

	/**
	 * @brief Get device context
	 * @return Pointer to device context
	 */
	virtual class BaseDeviceContextRHI*				GetImmediateContext() const override;

	/**
	 * @brief Get D3D11 device
	 * @return Pointer to D3D11 device
	 */
	FORCEINLINE ID3D11Device*						GetD3D11Device() const
	{
		return d3d11Device;
	}

	/**
	 * @brief Get DXGI factory
	 * @return Pointer to DXGI factory
	 */
	FORCEINLINE IDXGIFactory*						GetDXGIFactory() const
	{
		return dxgiFactory;
	}

	/**
	 * @brief Get DXGI adapter
	 * @return Pointer to DXGI adapter
	 */
	FORCEINLINE IDXGIAdapter*						GetDXGIAdapter() const
	{
		return dxgiAdapter;
	}

private:
	class D3D11DeviceContext*		immediateContext;			/**< Immediate context */

	ID3D11Device*					d3d11Device;				/**< D3D11 Device */
	IDXGIFactory*					dxgiFactory;				/**< DXGI factory */
	IDXGIAdapter*					dxgiAdapter;				/**< DXGI adapter */
};

/**
 * @brief Set debug name fore DirectX 11 resource
 * 
 * @param[in] InObject Object of D3D11
 * @param[in] InName Debug name of resource
 */
void				D3D11SetDebugName( ID3D11DeviceChild* InObject, achar* InName );

/**
 * @brief Set debug name fore DirectX 11 resource
 *
 * @param[in] InObject Object of D3D11
 * @param[in] InName Debug name of resource
 */
void				D3D11SetDebugName( IDXGIObject* InObject, achar* InName );

#endif // !D3D11RHI_H
