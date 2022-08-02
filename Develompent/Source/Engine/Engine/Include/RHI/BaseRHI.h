/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright BSOD-Games, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef BASERHI_H
#define BASERHI_H

#include "LEBuild.h"
#include "Misc/Types.h"
#include "Render/Shaders/ShaderCompiler.h"
#include "RHI/TypesRHI.h"
#include "RHI/BaseViewportRHI.h"
#include "RHI/BaseSurfaceRHI.h"
#include "RHI/BaseBufferRHI.h"
#include "RHI/BaseStateRHI.h"

/**
 * @ingroup Engine
 * @brief The alignment in bytes between elements of array shader parameters
 */
enum { ShaderArrayElementAlignBytes = 16 };

/**
 * @ingroup Engine
 * @brief Enumeration of primitive types
 */
enum EPrimitiveType
{
	PT_PointList,			/**< Points */
	PT_TriangleList,		/**< Triangles */
	PT_TriangleStrip,		/**< Triangles strip */
	PT_LineList,			/**< Lines */
	PT_Num					/**< Count primitives */
};

/**
 * @ingroup Engine
 * @brief Base class of RHI
 */
class CBaseRHI
{
public:
	/**
	 * @brief Destructor
	 */
	virtual										~CBaseRHI() {}

	/**
	 * @brief Initialize RHI
	 * 
	 * @param[in] InIsEditor Is current application editor
	 */
	virtual void								Init( bool InIsEditor )	{}

	/**
	 * @brief Destroy RHI
	 */
	virtual void								Destroy() {}

	/**
	 * @brief Acquire thread ownership
	 */
	virtual void								AcquireThreadOwnership() {}

	/**
	 * @brief Release thread ownership
	 */
	virtual void								ReleaseThreadOwnership() {}

	/**
	 * @brief Create viewport
	 * 
	 * @param[in] InWindowHandle OS handle on window
	 * @param[in] InWidth Width of viewport
	 * @param[in] InHeight Height of viewport
	 * @return Pointer on viewport
	 */
	virtual ViewportRHIRef_t						CreateViewport( void* InWindowHandle, uint32 InWidth, uint32 InHeight )								{ return nullptr; }

	/**
	 * @brief Create vertex shader
	 * 
	 * @param[in] InShaderName Shader name
	 * @param[in] InData Data to shader code
	 * @param[in] InSize Size of data
	 * @return Pointer to vertex shader
	 */
	virtual VertexShaderRHIRef_t					CreateVertexShader( const tchar* InShaderName, const byte* InData, uint32 InSize )					{ return nullptr; }

	/**
	 * @brief Create hull shader
	 *
	 * @param[in] InShaderName Shader name
	 * @param[in] InData Data to shader code
	 * @param[in] InSize Size of data
	 * @return Pointer to hull shader
	 */
	virtual HullShaderRHIRef_t					CreateHullShader( const tchar* InShaderName, const byte* InData, uint32 InSize )					{ return nullptr; }

	/**
	 * @brief Create domain shader
	 *
	 * @param[in] InShaderName Shader name
	 * @param[in] InData Data to shader code
	 * @param[in] InSize Size of data
	 * @return Pointer to domain shader
	 */
	virtual DomainShaderRHIRef_t					CreateDomainShader( const tchar* InShaderName, const byte* InData, uint32 InSize )					{ return nullptr; }

	/**
	 * @brief Create pixel shader
	 *
	 * @param[in] InShaderName Shader name
	 * @param[in] InData Data to shader code
	 * @param[in] InSize Size of data
	 * @return Pointer to pixel shader
	 */
	virtual PixelShaderRHIRef_t					CreatePixelShader( const tchar* InShaderName, const byte* InData, uint32 InSize )					{ return nullptr; }

	/**
	 * @brief Create geometry shader
	 *
	 * @param[in] InShaderName Shader name
	 * @param[in] InData Data to shader code
	 * @param[in] InSize Size of data
	 * @return Pointer to geometry shader
	 */
	virtual GeometryShaderRHIRef_t				CreateGeometryShader( const tchar* InShaderName, const byte* InData, uint32 InSize )				{ return nullptr; }

	/**
	 * @brief Create vertex buffer
	 * 
	 * @param[in] InBufferName Buffer name
	 * @param[in] InSize Size buffer
	 * @param[in] InData Pointer to data
	 * @param[in] InUsage Usage flags
	 * @return Pointer to vertex buffer
	 */
	virtual VertexBufferRHIRef_t					CreateVertexBuffer( const tchar* InBufferName, uint32 InSize, const byte* InData, uint32 InUsage )	{ return nullptr; }

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
	virtual IndexBufferRHIRef_t					CreateIndexBuffer( const tchar* InBufferName, uint32 InStride, uint32 InSize, const byte* InData, uint32 InUsage )	{ return nullptr; }

	/**
	 * @brief Create vertex declaration
	 * 
	 * @param[in] InElementList Array of vertex elements
	 * @return Pointer to vertex declaration
	 */
	virtual VertexDeclarationRHIRef_t			CreateVertexDeclaration( const VertexDeclarationElementList_t& InElementList ) { return nullptr; }

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
	virtual BoundShaderStateRHIRef_t				CreateBoundShaderState( const tchar* InBoundShaderStateName, VertexDeclarationRHIRef_t InVertexDeclaration, VertexShaderRHIRef_t InVertexShader, PixelShaderRHIRef_t InPixelShader, HullShaderRHIRef_t InHullShader = nullptr, DomainShaderRHIRef_t InDomainShader = nullptr, GeometryShaderRHIRef_t InGeometryShader = nullptr ) { return nullptr; }

	/**
	 * @brief Create rasterizer state
	 * 
	 * @param[in] InInitializer Initializer of rasterizer state
	 * @return Pointer to rasterizer state
	 */
	virtual RasterizerStateRHIRef_t				CreateRasterizerState( const SRasterizerStateInitializerRHI& InInitializer ) { return nullptr; }

	/**
	 * @brief Create sampler state
	 * 
	 * @param[in] InInitializer Initializer of sampler state
	 * @return Pointer to sampler state
	 */
	virtual SamplerStateRHIRef_t					CreateSamplerState( const SSamplerStateInitializerRHI& InInitializer ) { return nullptr; }

	/**
	 * @brief Create depth state
	 *
	 * @param InInitializer		Initializer of depth state
	 * @return Pointer to depth state
	 */
	virtual DepthStateRHIRef_t					CreateDepthState( const SDepthStateInitializerRHI& InInitializer ) { return nullptr; }

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
	 * @return Return pointer to created texture 2D
	 */
	virtual Texture2DRHIRef_t					CreateTexture2D( const tchar* InDebugName, uint32 InSizeX, uint32 InSizeY, EPixelFormat InFormat, uint32 InNumMips, uint32 InFlags, void* InData = nullptr ) { return nullptr; }

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
	virtual SurfaceRHIRef_t						CreateTargetableSurface( const tchar* InDebugName, uint32 InSizeX, uint32 InSizeY, EPixelFormat InFormat, Texture2DRHIParamRef_t InResolveTargetTexture, uint32 InFlags ) { return nullptr; }

	/**
	 * @brief Begin drawing viewport
	 * 
	 * @param[in] InDeviceContext Device context
	 * @param[in] InViewport Viewport
	 */
	virtual void								BeginDrawingViewport( class CBaseDeviceContextRHI* InDeviceContext, class CBaseViewportRHI* InViewport ) {}

	/**
	 * @brief End drawing viewport
	 * 
	 * @param[in] InDeviceContext Device context
	 * @param[in] InViewport Viewport
	 * @param[in] InIsPresent Whether to display the frame on the screen
	 * @param[in] InLockToVsync Is it necessary to block for Vsync
	 */
	virtual void								EndDrawingViewport( class CBaseDeviceContextRHI* InDeviceContext, class CBaseViewportRHI* InViewport, bool InIsPresent, bool InLockToVsync ) {}

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
	virtual bool								CompileShader( const tchar* InSourceFileName, const tchar* InFunctionName, EShaderFrequency InFrequency, const SShaderCompilerEnvironment& InEnvironment, SShaderCompilerOutput& InOutput, bool InDebugDump = false, const tchar* InShaderSubDir = TEXT( "" ) )			{ return false; }
#endif // WITH_EDITOR

	/**
	 * @brief Get shader platform
	 * @return Return shader platform
	 */
	virtual EShaderPlatform						GetShaderPlatform() const			{ return SP_NumPlatforms; }

#if WITH_IMGUI
	/**
	 * @brief Initialize render of ImGUI
	 *
	 * @param[in] InDeviceContext Device context
	 */
	virtual void								InitImGUI( class CBaseDeviceContextRHI* InDeviceContext ) {}

	/**
	 * @brief Shutdown render of ImGUI
	 *
	 * @param[in] InDeviceContext Device context
	 */
	virtual void								ShutdownImGUI( class CBaseDeviceContextRHI* InDeviceContext ) {}

	/**
	 * @brief Draw ImGUI
	 *
	 * @param[in] InDeviceContext Device context
	 * @param[in] InImGUIDrawData Pointer to draw data of ImGUI
	 */
	virtual void								DrawImGUI( class CBaseDeviceContextRHI* InDeviceContext, struct ImDrawData* InImGUIDrawData ) {}
#endif // WITH_IMGUI

#if FRAME_CAPTURE_MARKERS
	/**
	 * @brief Begin draw event
	 *
	 * @param InDeviceContext Device context
	 * @param InColor Color event
	 * @param InName Event name
	 */
	virtual void								BeginDrawEvent( class CBaseDeviceContextRHI* InDeviceContext, const CColor& InColor, const tchar* InName ) {}

	/**
	 * @brief End draw event
	 *
	 * @param[in] InDeviceContext Device context
	 */
	virtual void								EndDrawEvent( class CBaseDeviceContextRHI* InDeviceContext ) {}
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
	virtual void								SetupInstancing( class CBaseDeviceContextRHI* InDeviceContext, uint32 InStreamIndex, void* InInstanceData, uint32 InInstanceStride, uint32 InInstanceSize, uint32 InNumInstances ) {}

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
	virtual void								SetViewport( class CBaseDeviceContextRHI* InDeviceContext, uint32 InMinX, uint32 InMinY, float InMinZ, uint32 InMaxX, uint32 InMaxY, float InMaxZ ) {}

	/**
	 * @brief Set bound shader state
	 * 
	 * @param[in] InDeviceContext Device context
	 * @param[in] InBoundShaderState Bound shader state
	 */
	virtual void								SetBoundShaderState( class CBaseDeviceContextRHI* InDeviceContext, BoundShaderStateRHIParamRef_t InBoundShaderState ) {}

	/**
	 * @brief Set stream source
	 * 
	 * @param[in] InDeviceContext Device context
	 * @param[in] InStreamIndex Stream index
	 * @param[in] InVertexBuffer Vertex buffer
	 * @param[in] InStride Stride
	 * @param[in] InOffset Offset
	 */
	virtual void								SetStreamSource( class CBaseDeviceContextRHI* InDeviceContext, uint32 InStreamIndex, VertexBufferRHIParamRef_t InVertexBuffer, uint32 InStride, uint32 InOffset ) {}

	/**
	 * @brief Set rasterizer state
	 * 
	 * @param[in] InDeviceContext Device context
	 * @param[in] InNewState New rasterizer state
	 */
	virtual void								SetRasterizerState( class CBaseDeviceContextRHI* InDeviceContext, RasterizerStateRHIParamRef_t InNewState ) {}

	/**
	 * @brief Set sampler state
	 * 
	 * @param[in] InDeviceContext Device context
	 * @param[in] InPixelShader Pointer to pixel shader
	 * @param[in] InNewState New sampler state
	 * @param[in] InStateIndex Slot for bind sampler
	 */
	virtual void								SetSamplerState( class CBaseDeviceContextRHI* InDeviceContext, PixelShaderRHIParamRef_t InPixelShader, SamplerStateRHIParamRef_t InNewState, uint32 InStateIndex ) {}

	/**
	 * Set texture parameter in pixel shader
	 * 
	 * @param[in] InDeviceContext Device context
	 * @param[in] InPixelShader Pointer to pixel shader
	 * @param[in] InTexture Pointer to texture
	 * @param[in] InTextureIndex Slot for bind texture
	 */
	virtual void								SetTextureParameter( class CBaseDeviceContextRHI* InDeviceContext, PixelShaderRHIParamRef_t InPixelShader, TextureRHIParamRef_t InTexture, uint32 InTextureIndex ) {}

	/**
	 * Set view parameters
	 * 
	 * @param[in] InDeviceContext Device context
	 * @param[in] InSceneView Scene view
	 */
	virtual void								SetViewParameters( class CBaseDeviceContextRHI* InDeviceContext, class CSceneView& InSceneView ) {}

	/**
	 * Set render target
	 * 
	 * @param[in] InDeviceContext Device context
	 * @param[in] InNewRenderTarget New render target
	 * @param[in] InNewDepthStencilTarget New depth stencil target
	 */
	virtual void								SetRenderTarget( class CBaseDeviceContextRHI* InDeviceContext, SurfaceRHIParamRef_t InNewRenderTarget , SurfaceRHIParamRef_t InNewDepthStencilTarget ) {}

	/**
	 * Set vertex shader parameter
	 * 
	 * @param[in] InDeviceContext Device context
	 * @param[in] InBufferIndex Buffer index
	 * @param[in] InBaseIndex Offset in bytes to begin parameter
	 * @param[in] InNumBytes Number bytes of parameter
	 * @param[in] InNewValue New value
	 */
	virtual void								SetVertexShaderParameter( class CBaseDeviceContextRHI* InDeviceContext, uint32 InBufferIndex, uint32 InBaseIndex, uint32 InNumBytes, const void* InNewValue ) {}

	/**
	 * Set pixel shader parameter
	 * 
	 * @param[in] InDeviceContext Device context
	 * @param[in] InBufferIndex Buffer index
	 * @param[in] InBaseIndex Offset in bytes to begin parameter
	 * @param[in] InNumBytes Number bytes of parameter
	 * @param[in] InNewValue New value
	 */
	virtual void								SetPixelShaderParameter( class CBaseDeviceContextRHI* InDeviceContext, uint32 InBufferIndex, uint32 InBaseIndex, uint32 InNumBytes, const void* InNewValue ) {}

	/**
	 * Set depth test
	 * 
	 * @param InDeviceContext		Device context
	 * @param InNewState			New depth test
	 */
	virtual void								SetDepthTest( class CBaseDeviceContextRHI* InDeviceContext, DepthStateRHIParamRef_t InNewState ) {}

	/**
	 * Commit constants
	 * 
	 * @param[in] InDeviceContext Device context
	 */
	virtual void								CommitConstants( class CBaseDeviceContextRHI* InDeviceContext ) {}

	/**
	 * @brief Lock vertex buffer
	 * 
	 * @param[in] InDeviceContext Device context
	 * @param[in] InVertexBuffer Pointer to vertex buffer
	 * @param[in] InSize Size
	 * @param[in] InOffset Offset in buffer
	 * @param[out] OutLockedData Locked data in buffer	 
	 */
	virtual void								LockVertexBuffer( class CBaseDeviceContextRHI* InDeviceContext, const VertexBufferRHIRef_t InVertexBuffer, uint32 InSize, uint32 InOffset, SLockedData& OutLockedData ) {}

	/**
	 * @brief Unlock vertex buffer
	 * 
	 * @param[in] InDeviceContext Device context
	 * @param[in] InVertexBuffer Pointer to vertex buffer
	 * @param[in] InLockedData Locked data in buffer
	 */
	virtual void								UnlockVertexBuffer( class CBaseDeviceContextRHI* InDeviceContext, const VertexBufferRHIRef_t InVertexBuffer, SLockedData& InLockedData ) {}

	/**
	 * @brief Lock index buffer
	 * 
	 * @param[in] InDeviceContext Device context
	 * @param[in] InIndexBuffer Pointer to index buffer
	 * @param[in] InSize Size
	 * @param[in] InOffset Offset in buffer
	 * @param[out] OutLockedData Locked data in buffer
	 */
	virtual void								LockIndexBuffer( class CBaseDeviceContextRHI* InDeviceContext, const IndexBufferRHIRef_t InIndexBuffer, uint32 InSize, uint32 InOffset, SLockedData& OutLockedData ) {}

	/**
	 * @brief Unlock index buffer
	 *
	 * @param[in] InDeviceContext Device context
	 * @param[in] InIndexBuffer Pointer to index buffer
	 * @param[in] InLockedData Locked data in buffer
	 */
	virtual void								UnlockIndexBuffer( class CBaseDeviceContextRHI* InDeviceContext, const IndexBufferRHIRef_t InIndexBuffer, SLockedData& InLockedData ) {}

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
	virtual void								LockTexture2D( class CBaseDeviceContextRHI* InDeviceContext, Texture2DRHIParamRef_t InTexture, uint32 InMipIndex, bool InIsDataWrite, SLockedData& OutLockedData, bool InIsUseCPUShadow = false ) {}

	/**
	 * @brief Unlock texture 2D
	 * 
	 * @param[in] InDeviceContext Device context
	 * @param[in] InTexture Pointer to texture 2D
	 * @param[in] InMipIndex Mip index
	 * @param[in] InLockedData Locked data in texture
	 */
	virtual void								UnlockTexture2D( class CBaseDeviceContextRHI* InDeviceContext, Texture2DRHIParamRef_t InTexture, uint32 InMipIndex, SLockedData& InLockedData ) {}

	/**
	 * @brief Draw primitive
	 * 
	 * @param[in] InDeviceContext Device context
	 * @param[in] InPrimitiveType Primitive type
	 * @param[in] InBaseVertexIndex Base vertex index
	 * @param[in] InNumPrimitives Number primitives for render
	 * @param[in] InNumInstances Number instances to draw
	 */
	virtual void								DrawPrimitive( class CBaseDeviceContextRHI* InDeviceContext, EPrimitiveType InPrimitiveType, uint32 InBaseVertexIndex, uint32 InNumPrimitives, uint32 InNumInstances = 1 ) {}

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
	virtual void								DrawIndexedPrimitive( class CBaseDeviceContextRHI* InDeviceContext, class CBaseIndexBufferRHI* InIndexBuffer, EPrimitiveType InPrimitiveType, uint32 InBaseVertexIndex, uint32 InStartIndex, uint32 InNumPrimitives, uint32 InNumInstances = 1 ) {}

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
	virtual void DrawPrimitiveUP( class CBaseDeviceContextRHI* InDeviceContext, EPrimitiveType InPrimitiveType, uint32 InBaseVertexIndex, uint32 InNumPrimitives, const void* InVertexData, uint32 InVertexDataStride, uint32 InNumInstances = 1 ) {}

	/**
	 * @brief Draw primitive
	 * 
	 * @param[in] InDeviceContext Device context
	 * @param[in] InPrimitiveType Primitive type
	 * @param[in] InBaseVertexIndex The lowest vertex index used by the index buffer
	 * @param[in] InNumPrimitives The number of primitives described by the index buffer
	 * @param[in] InNumVertices The number of vertices in the vertex buffer
	 * @param[in] InIndexData Reference to index data
	 * @param[in] InIndexDataStride The size of one index
	 * @param[in] InVertexData Reference to vertex data
	 * @param[in] InVertexDataStride The size of one vertex
	 * @param[in] InNumInstances Number instances to draw
	 */
	virtual void DrawIndexedPrimitiveUP( class CBaseDeviceContextRHI* InDeviceContext, EPrimitiveType InPrimitiveType, uint32 InBaseVertexIndex, uint32 InNumPrimitives, uint32 InNumVertices, const void* InIndexData, uint32 InIndexDataStride, const void* InVertexData, uint32 InVertexDataStride, uint32 InNumInstances = 1 ) {}

	/**
	 * @brief Is initialized RHI
	 * @return Return true if RHI is initialized, else false
	 */
	virtual bool								IsInitialize() const			{ return false; }

	/**
	 * @brief Get RHI name
	 * @return Return RHI name
	 */
	virtual const tchar*						GetRHIName() const				{ return TEXT( "NullRHI" ); }

	/**
	 * @brief Get device context
	 * @return Pointer to device context
	 */
	virtual class CBaseDeviceContextRHI*		GetImmediateContext() const		{ return nullptr; }
};

#endif // !BASERHI_H
