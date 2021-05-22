/**
 * @file
 * @addtogroup D3D11RHI D3D11RHI
 *
 * Copyright BSOD-Games, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef D3D11SHADER_H
#define D3D11SHADER_H

#include "RHI/BaseShaderRHI.h"

 /**
  * @ingroup D3D11RHI
  * @brief Class for work with DirectX shader
  */
class FD3D11ShaderRHI : public FBaseShaderRHI
{
public:
	/**
	 * @brief Constructor
	 * 
	 * @param[in] InFrequency Frequency of shader
	 * @param[in] InData Data of shader code
	 * @param[in] InSize Size of data
	 */
	FD3D11ShaderRHI( EShaderFrequency InFrequency, const byte* InData, uint32 InSize );

private:
	/**
	 * @brief Union of shaders
	 */
	union UShaderHandle
	{
		ID3D11VertexShader*			vertexShader;		/**< Pointer to DirectX 11 vertex shader */
		ID3D11HullShader*			hullShader;			/**< Pointer to DirectX 11 hull shader */
		ID3D11DomainShader*			domainShader;		/**< Pointer to DirectX 11 domain shader */
		ID3D11PixelShader*			pixelShader;		/**< Pointer to DirectX 11 pixel shader */
		ID3D11GeometryShader*		geometryShader;		/**< Pointer to DirectX 11 geometry shader */
	};

	UShaderHandle			shader;			/**< Union of shader */
};

/**
 * @ingroup D3D11RHI
 * @brief Class of DirectX vertex shader
 */
class FD3D11VertexShaderRHI : public FD3D11ShaderRHI
{
public:
	/**
	 * @brief Constructor
	 *
	 * @param[in] InData Data of shader code
	 * @param[in] InSize Size of data
	 */
	FD3D11VertexShaderRHI( const byte* InData, uint32 InSize ) :
		FD3D11ShaderRHI( SF_Vertex, InData, InSize )
	{}
};

/**
 * @ingroup D3D11RHI
 * @brief Class of DirectX hull shader
 */
class FD3D11HullShaderRHI : public FD3D11ShaderRHI
{
public:
	/**
	 * @brief Constructor
	 *
	 * @param[in] InData Data of shader code
	 * @param[in] InSize Size of data
	 */
	FD3D11HullShaderRHI( const byte* InData, uint32 InSize ) :
		FD3D11ShaderRHI( SF_Hull, InData, InSize )
	{}
};

/**
 * @ingroup D3D11RHI
 * @brief Class of DirectX domain shader
 */
class FD3D11DomainShaderRHI : public FD3D11ShaderRHI
{
public:
	/**
	 * @brief Constructor
	 *
	 * @param[in] InData Data of shader code
	 * @param[in] InSize Size of data
	 */
	FD3D11DomainShaderRHI( const byte* InData, uint32 InSize ) :
		FD3D11ShaderRHI( SF_Domain, InData, InSize )
	{}
};

/**
 * @ingroup D3D11RHI
 * @brief Class of DirectX pixel shader
 */
class FD3D11PixelShaderRHI : public FD3D11ShaderRHI
{
public:
	/**
	 * @brief Constructor
	 *
	 * @param[in] InData Data of shader code
	 * @param[in] InSize Size of data
	 */
	FD3D11PixelShaderRHI( const byte* InData, uint32 InSize ) :
		FD3D11ShaderRHI( SF_Pixel, InData, InSize )
	{}
};

/**
 * @ingroup D3D11RHI
 * @brief Class of DirectX geometry shader
 */
class FD3D11GeometryShaderRHI : public FD3D11ShaderRHI
{
public:
	/**
	 * @brief Constructor
	 *
	 * @param[in] InData Data of shader code
	 * @param[in] InSize Size of data
	 */
	FD3D11GeometryShaderRHI( const byte* InData, uint32 InSize ) :
		FD3D11ShaderRHI( SF_Geometry, InData, InSize )
	{}
};

#endif // !D3D11SHADER_H
