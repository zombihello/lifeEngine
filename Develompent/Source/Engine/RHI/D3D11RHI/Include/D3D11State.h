/**
 * @file
 * @addtogroup D3D11RHI D3D11RHI
 *
 * Copyright BSOD-Games, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef D3D11STATE_H
#define D3D11STATE_H

#include "RHI/BaseStateRHI.h"

/**
 * @ingroup D3D11RHI
 * @brief Class of resterize state DirectX 11
 */
class FD3D11RasterizerStateRHI : public FBaseRasterizerStateRHI
{
public:
	/**
	 * @brief Constructor
	 *
	 * @param[in] InInitializer Initializer of rasterizer state
	 */
	FD3D11RasterizerStateRHI( const FRasterizerStateInitializerRHI& InInitializer );

	/**
	 * @brief Destructor
	 */
	~FD3D11RasterizerStateRHI();

	/**
	 * @brief Get DirectX resource
	 * @return Pointer to DirectX resource
	 */
	FORCEINLINE struct ID3D11RasterizerState* GetResource()
	{
		return d3d11RasterizerState;
	}

private:
	struct ID3D11RasterizerState*				d3d11RasterizerState;		/**< Pointer to DirectX 11 rasterizer state */
};

/**
 * @ingroup D3D11RHI
 * Class of sampler state DirectX 11
 */
class FD3D11SamplerStateRHI : public FBaseSamplerStateRHI
{
public:
	/**
	 * Constructor
	 * 
	 * @param[in] InInitializer Initializer of sampler state
	 */
	FD3D11SamplerStateRHI( const FSamplerStateInitializerRHI& InInitializer );

	/**
	 * Destructor
	 */
	~FD3D11SamplerStateRHI();

	/**
	 * @brief Get DirectX resource
	 * @return Pointer to DirectX resource
	 */
	FORCEINLINE struct ID3D11SamplerState* GetResource()
	{
		return d3d11SamplerState;
	}

private:
	struct ID3D11SamplerState*				d3d11SamplerState;			/**< Pointer to DirectX 11 sampler state */
};

#endif // !D3D11STATE_H
