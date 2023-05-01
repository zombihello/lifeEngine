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
#include <unordered_map>

#include "Misc/EngineGlobals.h"
#include "Render/BoundShaderStateCache.h"
#include "D3D11State.h"
#include "D3D11Buffer.h"
#include "RHI/BaseRHI.h"

/**
 * @ingroup D3D11RHI
 * @brief Main class of DirectX 11
 */
class CD3D11RHI : public CBaseRHI
{
public:
	/**
	 * @brief Constructor
	 */
													CD3D11RHI();

	/**
	 * @brief Destructor
	 */
													~CD3D11RHI();

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
	 * @brief Acquire thread ownership
	 */
	virtual void									AcquireThreadOwnership() override;

	/**
	 * @brief Release thread ownership
	 */
	virtual void									ReleaseThreadOwnership() override;

	/**
	 * @brief Create viewport
	 *
	 * @param[in] InWindowHandle OS handle on window
	 * @param[in] InWidth Width of viewport
	 * @param[in] InHeight Height of viewport
	 * @return Pointer on viewport
	 */
	virtual ViewportRHIRef_t CreateViewport( WindowHandle_t InWindowHandle, uint32 InWidth, uint32 InHeight ) override;

	/**
	 * @brief Create viewport
	 *
	 * @param InTargetSurface	Target surface to render
	 * @param InWidth			Width of viewport
	 * @param InHeight			Height of viewport
	 * @return Pointer on viewport
	 */
	virtual ViewportRHIRef_t CreateViewport( SurfaceRHIParamRef_t InSurfaceRHI, uint32 InWidth, uint32 InHeight ) override;

	/**
	 * @brief Create vertex shader
	 *
	 * @param[in] InShaderName Shader name
	 * @param[in] InData Data to shader code
	 * @param[in] InSize Size of data
	 * @return Pointer to vertex shader
	 */
	virtual VertexShaderRHIRef_t						CreateVertexShader( const tchar* InShaderName, const byte* InData, uint32 InSize ) override;

	/**
	 * @brief Create hull shader
	 *
	 * @param[in] InShaderName Shader name
	 * @param[in] InData Data to shader code
	 * @param[in] InSize Size of data
	 * @return Pointer to hull shader
	 */
	virtual HullShaderRHIRef_t						CreateHullShader( const tchar* InShaderName, const byte* InData, uint32 InSize ) override;

	/**
	 * @brief Create domain shader
	 *
	 * @param[in] InShaderName Shader name
	 * @param[in] InData Data to shader code
	 * @param[in] InSize Size of data
	 * @return Pointer to domain shader
	 */
	virtual DomainShaderRHIRef_t						CreateDomainShader( const tchar* InShaderName, const byte* InData, uint32 InSize ) override;

	/**
	 * @brief Create pixel shader
	 *
	 * @param[in] InShaderName Shader name
	 * @param[in] InData Data to shader code
	 * @param[in] InSize Size of data
	 * @return Pointer to pixel shader
	 */
	virtual PixelShaderRHIRef_t						CreatePixelShader( const tchar* InShaderName, const byte* InData, uint32 InSize ) override;

	/**
	 * @brief Create geometry shader
	 *
	 * @param[in] InShaderName Shader name
	 * @param[in] InData Data to shader code
	 * @param[in] InSize Size of data
	 * @return Pointer to geometry shader
	 */
	virtual GeometryShaderRHIRef_t					CreateGeometryShader( const tchar* InShaderName, const byte* InData, uint32 InSize ) override;

	/**
	 * @brief Create vertex buffer
	 *
	 * @param[in] InBufferName Buffer name
	 * @param[in] InSize Size buffer
	 * @param[in] InData Pointer to data
	 * @param[in] InUsage Usage flags
	 * @return Pointer to vertex buffer
	 */
	virtual VertexBufferRHIRef_t						CreateVertexBuffer( const tchar* InBufferName, uint32 InSize, const byte* InData, uint32 InUsage ) override;

	/**
	 * @brief Create index buffer
	 *
	 * @param[in] InBufferName Buffer name
	 * @param[in] InStride Stride of struct
	 * @param[in] InSize Size buffer
	 * @param[in] InData Pointer to data
	 * @param[in] InUsage Usage flags
	 * @return Pointer to index buffer
	 */
	virtual IndexBufferRHIRef_t						CreateIndexBuffer( const tchar* InBufferName, uint32 InStride, uint32 InSize, const byte* InData, uint32 InUsage ) override;

	/**
	 * @brief Create vertex declaration
	 *
	 * @param[in] InElementList Array of vertex elements
	 * @return Pointer to vertex declaration
	 */
	virtual VertexDeclarationRHIRef_t				CreateVertexDeclaration( const VertexDeclarationElementList_t& InElementList ) override;

	/**
	 * @brief Create bound shader state
	 *
	 * @param[in] InBoundShaderStateName Bound shader state name for debug
	 * @param[in] InVertexDeclaration Vertex declaration
	 * @param[in] InVertexShader Vertex shader
	 * @param[in] InPixelShader Pixel shader
	 * @param[in] InHullShader Hull shader
	 * @param[in] InDomainShader Domain shader
	 * @param[in] InGeometryShader Geometry shader
	 * @return Pointer to bound shader state
	 */
	virtual BoundShaderStateRHIRef_t					CreateBoundShaderState( const tchar* InBoundShaderStateName, VertexDeclarationRHIRef_t InVertexDeclaration, VertexShaderRHIRef_t InVertexShader, PixelShaderRHIRef_t InPixelShader, HullShaderRHIRef_t InHullShader = nullptr, DomainShaderRHIRef_t InDomainShader = nullptr, GeometryShaderRHIRef_t InGeometryShader = nullptr ) override;

	/**
	 * @brief Create rasterizer state
	 *
	 * @param[in] InInitializer Initializer of rasterizer state
	 * @return Pointer to rasterizer state
	 */
	virtual RasterizerStateRHIRef_t					CreateRasterizerState( const SRasterizerStateInitializerRHI& InInitializer ) override;

	/**
	 * @brief Create sampler state
	 *
	 * @param[in] InInitializer Initializer of sampler state
	 * @return Pointer to sampler state
	 */
	virtual SamplerStateRHIRef_t						CreateSamplerState( const SSamplerStateInitializerRHI& InInitializer ) override;

	/**
	 * @brief Create depth state
	 *
	 * @param InInitializer		Initializer of depth state
	 * @return Pointer to depth state
	 */
	virtual DepthStateRHIRef_t						CreateDepthState( const SDepthStateInitializerRHI& InInitializer ) override;

	/**
	 * @brief Create blend state
	 *
	 * @param InInitializer		Initializer of blend state
	 * @return Pointer to blend state
	 */
	virtual BlendStateRHIRef_t						CreateBlendState( const SBlendStateInitializerRHI& InInitializer ) override;

	/**
	 * @brief Create stencil state
	 *
	 * @param InInitializer		Initializer of stencil state
	 * @return Pointer to stencil state
	 */
	virtual StencilStateRHIRef_t					CreateStencilState( const SStencilStateInitializerRHI& InInitializer ) override;

	/**
	 * @brief Create texture 2D
	 *
	 * @param[in] InDebugName Debug name
	 * @param[in] InSizeX Width
	 * @param[in] InSizeY Height
	 * @param[in] InFormat Pixel format
	 * @param[in] InNumMips Count mips
	 * @param[in] InFlags Texture create flags (use ETextureCreateFlags)
	 * @param[in] InData Pointer to data texture
	 */
	virtual Texture2DRHIRef_t						CreateTexture2D( const tchar* InDebugName, uint32 InSizeX, uint32 InSizeY, EPixelFormat InFormat, uint32 InNumMips, uint32 InFlags, void* InData = nullptr ) override;

	/**
	 * Creates a RHI surface that can be bound as a render target
	 *
	 * @param[in] InDebugName Debug name
	 * @param[in] InSizeX The width of the surface to create
	 * @param[in] InSizeY The height of the surface to create
	 * @param[in] InFormat The surface format to create
	 * @param[in] InResolveTargetTexture The 2d texture which the surface will be resolved to.  It must have been allocated with bResolveTargetable=TRUE
	 * @param[in] InFlags Surface creation flags
	 * @return Return pointer to created surface
	 */
	virtual SurfaceRHIRef_t							CreateTargetableSurface( const tchar* InDebugName, uint32 InSizeX, uint32 InSizeY, EPixelFormat InFormat, Texture2DRHIParamRef_t InResolveTargetTexture, uint32 InFlags ) override;

	/**
	 * @brief Begin drawing viewport
	 *
	 * @param[in] InDeviceContext Device context
	 * @param[in] InViewport Viewport
	 */
	virtual void									BeginDrawingViewport( class CBaseDeviceContextRHI* InDeviceContext, class CBaseViewportRHI* InViewport ) override;

	/**
	 * @brief End drawing viewport
	 *
	 * @param[in] InDeviceContext Device context
	 * @param[in] InViewport Viewport
	 * @param[in] InIsPresent Whether to display the frame on the screen
	 * @param[in] InLockToVsync Is it necessary to block for Vsync
	 */
	virtual void									EndDrawingViewport( class CBaseDeviceContextRHI* InDeviceContext, class CBaseViewportRHI* InViewport, bool InIsPresent, bool InLockToVsync ) override;

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
	virtual bool									CompileShader( const tchar* InSourceFileName, const tchar* InFunctionName, EShaderFrequency InFrequency, const SShaderCompilerEnvironment& InEnvironment, SShaderCompilerOutput& InOutput, bool InDebugDump = false, const tchar* InShaderSubDir = TEXT( "" ) ) override;
#endif // WITH_EDITOR

	/**
	 * @brief Get shader platform
	 * @return Return shader platform
	 */
	virtual EShaderPlatform						GetShaderPlatform() const override;

#if WITH_IMGUI
	/**
	 * @brief Initialize render of ImGUI
	 *
	 * @param[in] InDeviceContext Device context
	 */
	virtual void								InitImGUI( class CBaseDeviceContextRHI* InDeviceContext ) override;

	/**
	 * @brief Shutdown render of ImGUI
	 *
	 * @param[in] InDeviceContext Device context
	 */
	virtual void								ShutdownImGUI( class CBaseDeviceContextRHI* InDeviceContext ) override;

	/**
	 * @brief Draw ImGUI
	 *
	 * @param[in] InDeviceContext Device context
	 * @param[in] InImGUIDrawData Pointer to draw data of ImGUI
	 */
	virtual void								DrawImGUI( class CBaseDeviceContextRHI* InDeviceContext, struct ImDrawData* InImGUIDrawData ) override;
#endif // WITH_IMGUI

#if FRAME_CAPTURE_MARKERS
	/**
	 * @brief Begin draw event
	 *
	 * @param InDeviceContext Device context
	 * @param InColor Color event
	 * @param InName Event name
	 */
	virtual void								BeginDrawEvent( class CBaseDeviceContextRHI* InDeviceContext, const CColor& InColor, const tchar* InName ) override;

	/**
	 * @brief End draw event
	 *
	 * @param[in] InDeviceContext Device context
	 */
	virtual void								EndDrawEvent( class CBaseDeviceContextRHI* InDeviceContext ) override;
#endif // FRAME_CAPTURE_MARKERS

	/**
	 * @brief Setup instancing
	 *
	 * @param[in] InDeviceContext Device context
	 * @param[in] InStreamIndex Stream index
	 * @param[in] InInstanceData Pointer to instance data
	 * @param[in] InInstanceStride Stride of instance data
	 * @param[in] InInstanceSize Size in bytes of instance data
	 * @param[in] InNumInstances Number of instances
	 */
	virtual void								SetupInstancing( class CBaseDeviceContextRHI* InDeviceContext, uint32 InStreamIndex, void* InInstanceData, uint32 InInstanceStride, uint32 InInstanceSize, uint32 InNumInstances ) override;

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
	virtual void									SetViewport( class CBaseDeviceContextRHI* InDeviceContext, uint32 InMinX, uint32 InMinY, float InMinZ, uint32 InMaxX, uint32 InMaxY, float InMaxZ ) override;

	/**
	 * @brief Set bound shader state
	 *
	 * @param[in] InDeviceContext Device context
	 * @param[in] InBoundShaderState Bound shader state
	 */
	virtual void									SetBoundShaderState( class CBaseDeviceContextRHI* InDeviceContext, BoundShaderStateRHIParamRef_t InBoundShaderState ) override;

	/**
	 * @brief Set stream source
	 *
	 * @param[in] InDeviceContext Device context
	 * @param[in] InStreamIndex Stream index
	 * @param[in] InVertexBuffer Vertex buffer
	 * @param[in] InStride Stride
	 * @param[in] InOffset Offset
	 */
	virtual void									SetStreamSource( class CBaseDeviceContextRHI* InDeviceContext, uint32 InStreamIndex, VertexBufferRHIParamRef_t InVertexBuffer, uint32 InStride, uint32 InOffset ) override;

	/**
	 * @brief Set rasterizer state
	 *
	 * @param[in] InDeviceContext Device context
	 * @param[in] InNewState New rasterizer state
	 */
	virtual void									SetRasterizerState( class CBaseDeviceContextRHI* InDeviceContext, RasterizerStateRHIParamRef_t InNewState ) override;

	/**
	 * @brief Set sampler state
	 *
	 * @param[in] InDeviceContext Device context
	 * @param[in] InPixelShader Pointer to pixel shader
	 * @param[in] InNewState New sampler state
	 * @param[in] InStateIndex Slot for bind sampler
	 */
	virtual void									SetSamplerState( class CBaseDeviceContextRHI* InDeviceContext, PixelShaderRHIParamRef_t InPixelShader, SamplerStateRHIParamRef_t InNewState, uint32 InStateIndex ) override;

	/**
	 * Set texture parameter in pixel shader
	 *
	 * @param[in] InDeviceContext Device context
	 * @param[in] InPixelShader Pointer to pixel shader
	 * @param[in] InTexture Pointer to texture
	 * @param[in] InTextureIndex Slot for bind texture
	 */
	virtual void									SetTextureParameter( class CBaseDeviceContextRHI* InDeviceContext, PixelShaderRHIParamRef_t InPixelShader, TextureRHIParamRef_t InTexture, uint32 InTextureIndex ) override;

	/**
	 * Set view parameters
	 *
	 * @param[in] InDeviceContext Device context
	 * @param[in] InSceneView Scene view
	 */
	virtual void									SetViewParameters( class CBaseDeviceContextRHI* InDeviceContext, class CSceneView& InSceneView ) override;

	/**
	 * Set render target
	 *
	 * @param[in] InDeviceContext Device context
	 * @param[in] InNewRenderTarget New render target
	 * @param[in] InNewDepthStencilTarget New depth stencil target
	 */
	virtual void									SetRenderTarget( class CBaseDeviceContextRHI* InDeviceContext, SurfaceRHIParamRef_t InNewRenderTarget, SurfaceRHIParamRef_t InNewDepthStencilTarget ) override;

	/**
	 * Set MRT render target
	 *
	 * @param[in] InDeviceContext Device context
	 * @param[in] InNewRenderTarget New render target
	 * @param[in] InTargetIndex Target index
	 */
	virtual void									SetMRTRenderTarget( class CBaseDeviceContextRHI* InDeviceContext, SurfaceRHIParamRef_t InNewRenderTarget, uint32 InTargetIndex ) override;

	/**
	 * Set vertex shader parameter
	 * 
	 * @param[in] InDeviceContext Device context
	 * @param[in] InBufferIndex Buffer index
	 * @param[in] InBaseIndex Offset in bytes to begin parameter
	 * @param[in] InNumBytes Number bytes of parameter
	 * @param[in] InNewValue New value
	 */
	virtual void									SetVertexShaderParameter( class CBaseDeviceContextRHI* InDeviceContext, uint32 InBufferIndex, uint32 InBaseIndex, uint32 InNumBytes, const void* InNewValue ) override;

	/**
	 * Set pixel shader parameter
	 * 
	 * @param[in] InDeviceContext Device context
	 * @param[in] InBufferIndex Buffer index
	 * @param[in] InBaseIndex Offset in bytes to begin parameter
	 * @param[in] InNumBytes Number bytes of parameter
	 * @param[in] InNewValue New value
	 */
	virtual void									SetPixelShaderParameter( class CBaseDeviceContextRHI* InDeviceContext, uint32 InBufferIndex, uint32 InBaseIndex, uint32 InNumBytes, const void* InNewValue ) override;

	/**
	 * Set depth test
	 *
	 * @param InDeviceContext		Device context
	 * @param InNewState			New depth test
	 */
	virtual void									SetDepthState( class CBaseDeviceContextRHI* InDeviceContext, DepthStateRHIParamRef_t InNewState ) override;

	/**
	 * Set blend state
	 *
	 * @param InDeviceContext		Device context
	 * @param InNewState			New blend state
	 */
	virtual void									SetBlendState( class CBaseDeviceContextRHI* InDeviceContext, BlendStateRHIParamRef_t InNewState ) override;

	/**
	 * Set Color write enable
	 *
	 * @param InDeviceContext		Device context
	 * @param InIsEnable			Enable or disable color write
	 */
	virtual void SetColorWriteEnable( class CBaseDeviceContextRHI* InDeviceContext, bool InIsEnable ) override;

	/**
	 * Set stencil state
	 *
	 * @param InDeviceContext		Device context
	 * @param InNewState			New stencil state
	 */
	virtual void									SetStencilState( class CBaseDeviceContextRHI* InDeviceContext, StencilStateRHIParamRef_t InNewState ) override;

	/**
	 * Commit constants
	 * 
	 * @param[in] InDeviceContext Device context
	 */
	virtual void									CommitConstants( class CBaseDeviceContextRHI* InDeviceContext ) override;

	/**
	 * @brief Lock vertex buffer
	 *
	 * @param[in] InDeviceContext Device context
	 * @param[in] InVertexBuffer Pointer to vertex buffer
	 * @param[in] InSize Size
	 * @param[in] InOffset Offset in buffer
	 * @param[out] OutLockedData Locked data in buffer
	 */
	virtual void									LockVertexBuffer( class CBaseDeviceContextRHI* InDeviceContext, const VertexBufferRHIRef_t InVertexBuffer, uint32 InSize, uint32 InOffset, SLockedData& OutLockedData ) override;

	/**
	 * @brief Unlock vertex buffer
	 *
	 * @param[in] InDeviceContext Device context
	 * @param[in] InVertexBuffer Pointer to vertex buffer
	 * @param[in] InLockedData Locked data in buffer
	 */
	virtual void									UnlockVertexBuffer( class CBaseDeviceContextRHI* InDeviceContext, const VertexBufferRHIRef_t InVertexBuffer, SLockedData& InLockedData ) override;

	/**
	 * @brief Lock index buffer
	 *
	 * @param[in] InDeviceContext Device context
	 * @param[in] InIndexBuffer Pointer to index buffer
	 * @param[in] InSize Size
	 * @param[in] InOffset Offset in buffer
	 * @param[out] OutLockedData Locked data in buffer
	 */
	virtual void									LockIndexBuffer( class CBaseDeviceContextRHI* InDeviceContext, const IndexBufferRHIRef_t InIndexBuffer, uint32 InSize, uint32 InOffset, SLockedData& OutLockedData ) override;

	/**
	 * @brief Unlock index buffer
	 *
	 * @param[in] InDeviceContext Device context
	 * @param[in] InIndexBuffer Pointer to index buffer
	 * @param[in] InLockedData Locked data in buffer
	 */
	virtual void									UnlockIndexBuffer( class CBaseDeviceContextRHI* InDeviceContext, const IndexBufferRHIRef_t InIndexBuffer, SLockedData& InLockedData ) override;

	/**
	 * @brief Lock texture 2D
	 *
	 * @param[in] InDeviceContext Device context
	 * @param[in] InTexture Pointer to texture 2D
	 * @param[in] InMipIndex Mip index
	 * @param[in] InIsDataWrite Is begin written to texture
	 * @param[out] OutLockedData Locked data in texture
	 * @param[in] InIsUseCPUShadow Is use CPU shadow
	 */
	virtual void									LockTexture2D( class CBaseDeviceContextRHI* InDeviceContext, Texture2DRHIParamRef_t InTexture, uint32 InMipIndex, bool InIsDataWrite, SLockedData& OutLockedData, bool InIsUseCPUShadow = false ) override;

	/**
	 * @brief Unlock texture 2D
	 *
	 * @param[in] InDeviceContext Device context
	 * @param[in] InTexture Pointer to texture 2D
	 * @param[in] InMipIndex Mip index
	 * @param[in] InLockedData Locked data in texture
	 */
	virtual void									UnlockTexture2D( class CBaseDeviceContextRHI* InDeviceContext, Texture2DRHIParamRef_t InTexture, uint32 InMipIndex, SLockedData& InLockedData ) override;

	/**
	 * @brief Draw primitive
	 *
	 * @param[in] InDeviceContext Device context
	 * @param[in] InPrimitiveType Primitive type
	 * @param[in] InBaseVertexIndex Base vertex index
	 * @param[in] InNumPrimitives Number primitives for render
	 * @param[in] InNumInstances Number instances to draw
	 */
	virtual void								DrawPrimitive( class CBaseDeviceContextRHI* InDeviceContext, EPrimitiveType InPrimitiveType, uint32 InBaseVertexIndex, uint32 InNumPrimitives, uint32 InNumInstances = 1 ) override;

	/**
	 * @brief Draw primitive
	 *
	 * @param[in] InDeviceContext Device context
	 * @param[in] InIndexBuffer Index buffer
	 * @param[in] InPrimitiveType Primitive type
	 * @param[in] InBaseVertexIndex Base vertex index
	 * @param[in] InStartIndex Start index in index buffer
	 * @param[in] InNumPrimitives Number primitives for render
	 * @param[in] InNumInstances Number instances to draw
	 */
	virtual void								DrawIndexedPrimitive( class CBaseDeviceContextRHI* InDeviceContext, class CBaseIndexBufferRHI* InIndexBuffer, EPrimitiveType InPrimitiveType, uint32 InBaseVertexIndex, uint32 InStartIndex, uint32 InNumPrimitives, uint32 InNumInstances = 1 ) override;

	/**
	 * @brief Draw primitive
	 *
	 * @param[in] InDeviceContext Device context
	 * @param[in] InPrimitiveType Primitive type
	 * @param[in] InBaseVertexIndex Base vertex index
	 * @param[in] InNumPrimitives Number primitives for render
	 * @param[in] InVertexData Reference to vertex data
	 * @param[in] InVertexDataStride The size of one vertex
	 * @param[in] InNumInstances Number instances to draw
	 */
	virtual void DrawPrimitiveUP( class CBaseDeviceContextRHI* InDeviceContext, EPrimitiveType InPrimitiveType, uint32 InBaseVertexIndex, uint32 InNumPrimitives, const void* InVertexData, uint32 InVertexDataStride, uint32 InNumInstances = 1 ) override;

	/**
	 * @brief Draw primitive
	 *
	 * @param[in] InDeviceContext Device context
	 * @param[in] InPrimitiveType Primitive type
	 * @param[in] InBaseVertexIndex The lowest vertex index used by the index buffer
	 * @param[in] InNumPrimitives The number of primitives described by the index buffer
	 * @param[in] InIndexData Reference to index data
	 * @param[in] InIndexDataStride The size of one index
	 * @param[in] InVertexData Reference to vertex data
	 * @param[in] InVertexDataStride The size of one vertex
	 * @param[in] InNumInstances Number instances to draw
	 */
	virtual void DrawIndexedPrimitiveUP( class CBaseDeviceContextRHI* InDeviceContext, EPrimitiveType InPrimitiveType, uint32 InBaseVertexIndex, uint32 InNumPrimitives, uint32 InNumVertices, const void* InIndexData, uint32 InIndexDataStride, const void* InVertexData, uint32 InVertexDataStride, uint32 InNumInstances = 1 ) override;

	/**
	 * @brief Copies the contents of the given surface to its resolve target texture
	 *
	 * @param InDeviceContext		Device context
	 * @param InSourceSurface		Surface with a resolve texture to copy to
	 * @param InResolveParams		Optional resolve params
	 */
	virtual void CopyToResolveTarget( class CBaseDeviceContextRHI* InDeviceContext, SurfaceRHIParamRef_t InSourceSurface, const SResolveParams& InResolveParams ) override;

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
	virtual class CBaseDeviceContextRHI*			GetImmediateContext() const override;

	/**
	 * @brief Get viewport
	 *
	 * @param OutMinX Min x
	 * @param OutMinY Min y
	 * @param OutMinZ Min z
	 * @param OutMaxX Max x
	 * @param OutMaxY Max y
	 * @param OutMaxZ Max z
	 */
	virtual void GetViewport( uint32& OutMinX, uint32& OutMinY, float& OutMinZ, uint32& OutMaxX, uint32& OutMaxY, float& OutMaxZ ) const override;

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

	/**
	 * @brief Get bound shader state history
	 * @return Reference to bound shader state history
	 */
	FORCEINLINE CBoundShaderStateHistory&			GetBoundShaderStateHistory()
	{
		return boundShaderStateHistory;
	}

	/**
	 * @brief Get DirectX state cache
	 * @return Return DirectX state cache
	 */
	FORCEINLINE const SD3D11StateCache& GetStateCache() const
	{
		return stateCache;
	}

private:
	/**
	 * @brief Get cached blend state
	 * 
	 * @param InBlendStateInfo			Blend state info
	 * @param InIsColorWriteEnable		Is enabled color write
	 * @return Return new or old blend state
	 */
	TRefCountPtr<CD3D11BlendStateRHI> GetCachedBlendState( const SBlendStateInitializerRHI& InInitializer, bool InIsColorWriteEnable );

	bool																isInitialize;						/**< Is RHI is initialized */
	class CD3D11ConstantBuffer*											globalConstantBuffer;				/**< Global constant buffer */
	class CD3D11ConstantBuffer*											vsConstantBuffers[ SOB_Max ];		/**< Constant buffers for vertex shader */
	class CD3D11ConstantBuffer*											psConstantBuffer;					/**< Constant buffer for pixel shader */
	class CD3D11DeviceContext*											immediateContext;					/**< Immediate context */
	CBoundShaderStateHistory											boundShaderStateHistory;			/**< History of using bound shader states */
	SD3D11StateCache													stateCache;							/**< DirectX 11 state cache */
	TRefCountPtr< CD3D11VertexBufferRHI >								instanceBuffer;						/**< Instance buffer */
	std::unordered_map<uint64, TRefCountPtr<CD3D11BlendStateRHI>>		cachedBlendStates;					/**< Cached blend states */

	D3D_FEATURE_LEVEL													d3dFeatureLevel;					/**< DirectX feature level */
	ID3D11Device*														d3d11Device;						/**< D3D11 Device */
	IDXGIFactory*														dxgiFactory;						/**< DXGI factory */
	IDXGIAdapter*														dxgiAdapter;						/**< DXGI adapter */
};

/**
 * @brief Set debug name fore DirectX 11 resource
 * 
 * @param[in] InObject Object of D3D11
 * @param[in] InName Debug name of resource
 */
void				D3D11SetDebugName( ID3D11DeviceChild* InObject, const achar* InName );

/**
 * @brief Set debug name fore DirectX 11 resource
 *
 * @param[in] InObject Object of D3D11
 * @param[in] InName Debug name of resource
 */
void				D3D11SetDebugName( IDXGIObject* InObject, const achar* InName );

#endif // !D3D11RHI_H
