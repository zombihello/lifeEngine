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
	CD3D11RasterizerStateRHI( const SRasterizerStateInitializerRHI& InInitializer );

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
	CD3D11DepthStateRHI( const SDepthStateInitializerRHI& InInitializer );

	/**
	 * Destructor
	 */
	~CD3D11DepthStateRHI();

	/**
	 * @brief Get DirectX resource
	 * @return Pointer to DirectX resource
	 */
	FORCEINLINE struct ID3D11DepthStencilState* GetResource()
	{
		return d3d11DepthState;
	}

private:
	struct ID3D11DepthStencilState*		d3d11DepthState;		/**< Pointer to DirectX 11 depth state */
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
	CD3D11BlendStateRHI( const SBlendStateInitializerRHI& InInitializer, bool InIsColorWriteEnable = true );

	/**
	 * Destructor
	 */
	~CD3D11BlendStateRHI();

	/**
	 * @brief Get DirectX resource
	 * @return Pointer to DirectX resource
	 */
	FORCEINLINE struct ID3D11BlendState* GetResource()
	{
		return d3d11BlendState;
	}

	/**
	 * @brief Get blend state info
	 * @return Return blend state info
	 */
	FORCEINLINE const SBlendStateInitializerRHI& GetInfo() const
	{
		return blendStateInfo;
	}

	/**
	 * @brief Get hash
	 * @return Return hash of blend state
	 */
	FORCEINLINE uint64 GetHash() const
	{
		return hash;
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
	uint64								hash;					/**< Hash */
	SBlendStateInitializerRHI			blendStateInfo;			/**< Blend state info */
	struct ID3D11BlendState*			d3d11BlendState;		/**< Point to DirectX 11 blend state */
	bool								bColorWrite;			/**< Is enabled color write */
};

/**
 * @ingroup D3D11RHI
 * Class for caching DirectX 11 states
 */
struct SD3D11StateCache
{
public:
	/**
	 * Constructor
	 */
	SD3D11StateCache();

	struct D3D11_VIEWPORT				viewport;																/**< Viewport */
	struct ID3D11InputLayout*			inputLayout;															/**< Input layout */
	struct ID3D11VertexShader*			vertexShader;															/**< Vertex shader */
	struct ID3D11PixelShader*			pixelShader;															/**< Pixel shader */
	struct ID3D11GeometryShader*		geometryShader;															/**< Geometry shader */
	struct ID3D11HullShader*			hullShader;																/**< Hull shader */
	struct ID3D11DomainShader*			domainShader;															/**< Domain shader */
	CD3D11StateVertexBuffer				vertexBuffers[ D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT ];				/**< Vertex buffers */
	struct ID3D11RasterizerState*		rasterizerState;														/**< Rasterizer state */
	struct ID3D11SamplerState*			psSamplerStates[ D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT ];				/**< Sampler state for pixel shader */
	struct ID3D11ShaderResourceView*	psShaderResourceViews[ D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT ];	/**< Shader resource view for pixel shader */
	D3D11_PRIMITIVE_TOPOLOGY			primitiveTopology;														/**< Primitive topology */
	CD3D11StateIndexBuffer				indexBuffer;															/**< Index buffer */
	struct ID3D11RenderTargetView*		renderTargetViews[ D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT ];			/**< Render target view */
	struct ID3D11DepthStencilView*		depthStencilView;														/**< Depth stencil view */
	struct ID3D11DepthStencilState*		depthStencilState;														/**< Depth stencil state */
	TRefCountPtr<CD3D11BlendStateRHI>	blendState;																/**< Blend state */
	bool								bColorWrite;															/**< Is enabled color write */
};

#endif // !D3D11STATE_H
