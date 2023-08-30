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
class CD3D11RasterizerStateRHI : public CBaseRasterizerStateRHI
{
public:
	/**
	 * @brief Constructor
	 *
	 * @param[in] InInitializer Initializer of rasterizer state
	 */
	CD3D11RasterizerStateRHI( const RasterizerStateInitializerRHI& InInitializer );

	/**
	 * @brief Destructor
	 */
	~CD3D11RasterizerStateRHI();

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
 * Class for caching binded vertex buffer
 */
struct CD3D11StateVertexBuffer
{
	/**
	 * Overload operator ==
	 */
	FORCEINLINE bool operator==( const CD3D11StateVertexBuffer& InOther ) const
	{
		return vertexBuffer == InOther.vertexBuffer && stride == InOther.stride && offset == InOther.offset;
	}

	/**
	 * Overload operator !=
	 */
	FORCEINLINE bool operator!=( const CD3D11StateVertexBuffer& InOther ) const
	{
		return !( *this == InOther );
	}

	ID3D11Buffer*			vertexBuffer;		/**< Pointer to vertex buffer */
	uint32					stride;				/**< Stride of vertex buffer */
	uint32					offset;				/**< Offset in vertex buffer */
};

/**
 * @ingroup D3D11RHI
 * Class for caching binded index buffer
 */
struct CD3D11StateIndexBuffer
{
	/**
	 * Overload operator ==
	 */
	FORCEINLINE bool operator==( const CD3D11StateIndexBuffer& InOther ) const
	{
		return indexBuffer == InOther.indexBuffer && format == InOther.format && offset == InOther.offset;
	}

	/**
	 * Overload operator !=
	 */
	FORCEINLINE bool operator!=( const CD3D11StateIndexBuffer& InOther ) const
	{
		return !( *this == InOther );
	}

	ID3D11Buffer*			indexBuffer;		/**< Pointer to index buffer */
	DXGI_FORMAT				format;				/**< Format data in index buffer */
	uint32					offset;				/**< Offset in index buffer */
};

/**
 * @ingroup D3D11RHI
 * Class of sampler state DirectX 11
 */
class CD3D11SamplerStateRHI : public CBaseSamplerStateRHI
{
public:
	/**
	 * Constructor
	 * 
	 * @param[in] InInitializer Initializer of sampler state
	 */
	CD3D11SamplerStateRHI( const SSamplerStateInitializerRHI& InInitializer );

	/**
	 * Destructor
	 */
	~CD3D11SamplerStateRHI();

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

/**
 * @ingroup D3D11RHI
 * Class of depth state DirectX 11
 */
class CD3D11DepthStateRHI : public CBaseDepthStateRHI
{
public:
	/**
	 * Constructor
	 * 
	 * @param InInitializer		Initializer of depth state
	 */
	CD3D11DepthStateRHI( const DepthStateInitializerRHI& InInitializer );

	/**
	 * @brief Get DirectX 11 depth state info
	 * @return Return DirectX 11 depth state info
	 */
	FORCEINLINE const struct D3D11_DEPTH_STENCIL_DESC& GetInfo() const
	{
		return d3d11DepthStateInfo;
	}

private:
	struct D3D11_DEPTH_STENCIL_DESC		d3d11DepthStateInfo;		/**< DirectX 11 depth state info */
};

/**
 * @ingroup D3D11RHI
 * Class of stencil state DirectX 11
 */
class CD3D11StencilStateRHI : public CBaseStencilStateRHI
{
public:
	/**
	 * Constructor
	 *
	 * @param InInitializer		Initializer of stencil state
	 */
	CD3D11StencilStateRHI( const StencilStateInitializerRHI& InInitializer );

	/**
	 * @brief Get DirectX 11 stencil state info
	 * @return Return DirectX 11 stencil state info
	 */
	FORCEINLINE const struct D3D11_DEPTH_STENCIL_DESC& GetInfo() const
	{
		return d3d11StencilStateInfo;
	}

	/**
	 * @brief Get stencil reference
	 * @return Return stencil reference
	 */
	FORCEINLINE uint32 GetStencilRef() const
	{
		return stencilRef;
	}

private:
	struct D3D11_DEPTH_STENCIL_DESC		d3d11StencilStateInfo;		/**< DirectX 11 stencil state info */
	uint32								stencilRef;					/**< Stencil reference */
};

/**
 * @ingroup D3D11RHI
 * Class of blend state DirectX 11
 */
class CD3D11BlendStateRHI : public CBaseBlendStateRHI
{
public:
	/**
	 * Constructor
	 *
	 * @param InInitializer				Initializer of blend state
	 * @param InIsColorWriteEnable		Is enabled color write
	 */
	CD3D11BlendStateRHI( const BlendStateInitializerRHI& InInitializer, bool InIsColorWriteEnable = true );

	/**
	 * @brief Get DirectX 11 blend state info
	 * @return Return DirectX 11 blend state info
	 */
	FORCEINLINE const struct D3D11_BLEND_DESC& GetInfo() const
	{
		return d3d11blendStateInfo;
	}

	/**
	 * @brief Is color write enabled
	 * @return Return TRUE if color write is enabled
	 */
	FORCEINLINE bool IsColorWrite() const
	{
		return bColorWrite;
	}

private:
	struct D3D11_BLEND_DESC			d3d11blendStateInfo;	/**< DirectX 11 blend state info */
	bool							bColorWrite;			/**< Is enabled color write */
};

/**
 * @ingroup D3D11RHI
 * Class for caching DirectX 11 states
 */
struct D3D11StateCache
{
public:
	/**
	 * Constructor
	 */
	D3D11StateCache();

	/**
	 * Reset state cache to default
	 */
	void Reset();

	struct D3D11_VIEWPORT				viewport;																/**< Viewport */
	struct ID3D11InputLayout*			inputLayout;															/**< Input layout */
	struct ID3D11VertexShader*			vertexShader;															/**< Vertex shader */
	struct ID3D11PixelShader*			pixelShader;															/**< Pixel shader */
	struct ID3D11GeometryShader*		geometryShader;															/**< Geometry shader */
	struct ID3D11HullShader*			hullShader;																/**< Hull shader */
	struct ID3D11DomainShader*			domainShader;															/**< Domain shader */
	CD3D11StateVertexBuffer				vertexBuffers[D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT];				/**< Vertex buffers */
	struct ID3D11RasterizerState*		rasterizerState;														/**< Rasterizer state */
	struct ID3D11SamplerState*			psSamplerStates[D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT];					/**< Sampler state for pixel shader */
	struct ID3D11ShaderResourceView*	psShaderResourceViews[D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT];	/**< Shader resource view for pixel shader */
	D3D11_PRIMITIVE_TOPOLOGY			primitiveTopology;														/**< Primitive topology */
	CD3D11StateIndexBuffer				indexBuffer;															/**< Index buffer */
	struct ID3D11RenderTargetView*		renderTargetViews[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT];				/**< Render target view */
	struct ID3D11DepthStencilView*		depthStencilView;														/**< Depth stencil view */
	struct D3D11_DEPTH_STENCIL_DESC		depthState;																/**< Depth state info */
	struct D3D11_DEPTH_STENCIL_DESC		stencilState;															/**< Stencil state info */
	struct D3D11_BLEND_DESC				blendState;																/**< Blend state */
	uint32								stencilRef;																/**< Stencil reference */
	uint8								colorWriteMasks[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT];				/**< Color write masks for each render target */
};

#endif // !D3D11STATE_H
