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
	 * @brief Get D3D11 device
	 * @return Pointer to D3D11 device
	 */
	FORCEINLINE ID3D11Device*						GetD3D11Device() const
	{
		return d3d11Device;
	}

	/**
	 * @brief Get D3D11 device context
	 * @return Pointer to D3D11 device context
	 */
	FORCEINLINE ID3D11DeviceContext*				GetD3D11DeviceContext() const
	{
		return d3d11DeviceContext;
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
	ID3D11Device*					d3d11Device;				/**< D3D11 Device */
	ID3D11DeviceContext*			d3d11DeviceContext;			/**< D3D11 Device context */

	IDXGIFactory*					dxgiFactory;				/**< DXGI factory */
	IDXGIAdapter*					dxgiAdapter;				/**< DXGI adapter */
};

#endif // !D3D11RHI_H
