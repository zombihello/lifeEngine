/**
 * @file
 * @addtogroup D3D11RHI D3D11RHI
 *
 * Copyright BSOD-Games, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
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
class FD3D11RHI : public FBaseRHI
{
public:
	/**
	 * @brief Constructor
	 */
													FD3D11RHI();

	/**
	 * @brief Destructor
	 */
													~FD3D11RHI();

	/**
	 * @brief Initialize RHI
	 *
	 * @param[in] InIsEditor Is current application editor
	 */
	virtual void									Init( bool InIsEditor ) override;

	/**
	 * @brief Destroy RHI
	 */
	virtual void									Destroy() override;

	/**
	 * @brief Create viewport
	 *
	 * @param[in] InWindowHandle OS handle on window
	 * @param[in] InWidth Width of viewport
	 * @param[in] InHeight Height of viewport
	 * @return Pointer on viewport
	 */
	virtual class FBaseViewportRHI*					CreateViewport( void* InWindowHandle, uint32 InWidth, uint32 InHeight ) override;

	/**
	 * @brief Create vertex shader
	 *
	 * @param[in] InShaderName Shader name
	 * @param[in] InData Data to shader code
	 * @param[in] InSize Size of data
	 * @return Pointer to vertex shader
	 */
	virtual FVertexShaderRHIRef						CreateVertexShader( const tchar* InShaderName, const byte* InData, uint32 InSize ) override;

	/**
	 * @brief Create hull shader
	 *
	 * @param[in] InShaderName Shader name
	 * @param[in] InData Data to shader code
	 * @param[in] InSize Size of data
	 * @return Pointer to hull shader
	 */
	virtual FHullShaderRHIRef						CreateHullShader( const tchar* InShaderName, const byte* InData, uint32 InSize ) override;

	/**
	 * @brief Create domain shader
	 *
	 * @param[in] InShaderName Shader name
	 * @param[in] InData Data to shader code
	 * @param[in] InSize Size of data
	 * @return Pointer to domain shader
	 */
	virtual FDomainShaderRHIRef						CreateDomainShader( const tchar* InShaderName, const byte* InData, uint32 InSize ) override;

	/**
	 * @brief Create pixel shader
	 *
	 * @param[in] InShaderName Shader name
	 * @param[in] InData Data to shader code
	 * @param[in] InSize Size of data
	 * @return Pointer to pixel shader
	 */
	virtual FPixelShaderRHIRef						CreatePixelShader( const tchar* InShaderName, const byte* InData, uint32 InSize ) override;

	/**
	 * @brief Create geometry shader
	 *
	 * @param[in] InShaderName Shader name
	 * @param[in] InData Data to shader code
	 * @param[in] InSize Size of data
	 * @return Pointer to geometry shader
	 */
	virtual FGeometryShaderRHIRef					CreateGeometryShader( const tchar* InShaderName, const byte* InData, uint32 InSize ) override;

	/**
	 * @brief Create vertex buffer
	 *
	 * @param[in] InBufferName Buffer name
	 * @param[in] InSize Size buffer
	 * @param[in] InData Pointer to data
	 * @param[in] InUsage Usage flags
	 * @return Pointer to vertex buffer
	 */
	virtual FVertexBufferRHIRef						CreateVertexBuffer( const tchar* InBufferName, uint32 InSize, const byte* InData, uint32 InUsage ) override;

	/**
	 * @brief Begin drawing viewport
	 *
	 * @param[in] InDeviceContext Device context
	 * @param[in] InViewport Viewport
	 */
	virtual void									BeginDrawingViewport( class FBaseDeviceContextRHI* InDeviceContext, class FBaseViewportRHI* InViewport ) override;

	/**
	 * @brief End drawing viewport
	 *
	 * @param[in] InDeviceContext Device context
	 * @param[in] InViewport Viewport
	 * @param[in] InIsPresent Whether to display the frame on the screen
	 * @param[in] InLockToVsync Is it necessary to block for Vsync
	 */
	virtual void									EndDrawingViewport( class FBaseDeviceContextRHI* InDeviceContext, class FBaseViewportRHI* InViewport, bool InIsPresent, bool InLockToVsync ) override;

#if WITH_EDITOR
	/**
	 * @brief Compile shader
	 *
	 * @param[in] InSourceFileName Path to source file of shader
	 * @param[in] InFunctionName Main function in shader
	 * @param[in] InFrequency Frequency of shader (Vertex, pixel, etc)
	 * @param[in] InEnvironment Environment of shader
	 * @param[out] InOutput Output data after compiling
	 * @param[in] InDebugDump Is need create debug dump of shader?
	 * @param[in] InShaderSubDir SubDir for debug dump
	 * @return Return true if compilation is succeed, else returning false
	 */
	virtual bool									CompileShader( const tchar* InSourceFileName, const tchar* InFunctionName, EShaderFrequency InFrequency, const FShaderCompilerEnvironment& InEnvironment, FShaderCompilerOutput& InOutput, bool InDebugDump = false, const tchar* InShaderSubDir = TEXT( "" ) ) override;

	/**
	 * @brief Initialize ImGUI
	 */
	virtual bool									InitImGUI( class FBaseDeviceContextRHI* InDeviceContext ) override;

	/**
	 * @brief Shutdown render of ImGUI
	 *
	 * @param[in] InDeviceContext Device context
	 */
	virtual void									ShutdownImGUI( class FBaseDeviceContextRHI* InDeviceContext ) override;

	/**
	 * @brief Begin drawing ImGUI
	 *
	 * @param[in] InDeviceContext Device context
	 */
	virtual void									BeginDrawingImGUI( class FBaseDeviceContextRHI* InDeviceContext ) override;

	/**
	 * @brief End drawing ImGUI
	 *
	 * @param[in] InDeviceContext Device context
	 */
	virtual void									EndDrawingImGUI( class FBaseDeviceContextRHI* InDeviceContext ) override;
#endif // WITH_EDITOR

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
	virtual void									SetViewport( class FBaseDeviceContextRHI* InDeviceContext, uint32 InMinX, uint32 InMinY, float InMinZ, uint32 InMaxX, uint32 InMaxY, float InMaxZ ) override;

	/**
	 * @brief Lock vertex buffer
	 *
	 * @param[in] InDeviceContext Device context
	 * @param[in] InVertexBuffer Pointer to vertex buffer
	 * @param[in] InSize Size
	 * @param[out] OutLockedData Locked data in buffer
	 */
	virtual void									LockVertexBuffer( class FBaseDeviceContextRHI* InDeviceContext, const FVertexBufferRHIRef InVertexBuffer, uint32 InSize, FLockedData& OutLockedData ) override;

	/**
	 * @brief Unlock vertex buffer
	 *
	 * @param[in] InDeviceContext Device context
	 * @param[in] InVertexBuffer Pointer to vertex buffer
	 * @param[in] InLockedData Locked data in buffer
	 */
	virtual void									UnlockVertexBuffer( class FBaseDeviceContextRHI* InDeviceContext, const FVertexBufferRHIRef InVertexBuffer, FLockedData& InLockedData ) override;

	/**
	 * @brief Is initialized RHI
	 * @return Return true if RHI is initialized, else false
	 */
	virtual bool									IsInitialize() const override;

	/**
	 * @brief Get RHI name
	 * @return Return RHI name
	 */
	virtual const tchar*							GetRHIName() const override;

	/**
	 * @brief Get device context
	 * @return Pointer to device context
	 */
	virtual class FBaseDeviceContextRHI*			GetImmediateContext() const override;

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
	bool							isInitialize;				/**< Is RHI is initialized */
	class FD3D11DeviceContext*		immediateContext;			/**< Immediate context */

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
