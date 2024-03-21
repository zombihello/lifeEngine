/**
 * @file
 * @addtogroup shadercompile shadercompile
 *
 * ************************************************************
 *                  This file is part of:
 *                      LIFEENGINE
 *          https://github.com/zombihello/lifeEngine
 * ************************************************************
 * Copyright (C) 2024 Yehor Pohuliaka.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef ISHADERCOMPILE_BACKEND_H
#define ISHADERCOMPILE_BACKEND_H

#include "core/types.h"
#include "stdlib/interface.h"
#include "studiorender/studioapi/istudioshader.h"

/**
 * @ingroup shadercompile
 * @brief Enumeration of flags for compile shader
 */
enum EShaderCompileFlags
{
	SHADERCOMPILE_NONE						= 0,		/**< No flags */
	SHADERCOMPILE_PREFER_FLOW_CONTROL		= 1 << 0,	/**< Prefer flow control */
	SHADERCOMPILE_DEBUG						= 1 << 1,	/**< Debug of shader */
	SHADERCOMPILE_AVOID_FLOW_CONTROL		= 1 << 2,	/**< Avoid flow control*/
	SHADERCOMPILE_SKIP_VALIDATION			= 1 << 3	/**< Disable shader validation */
};

/**
 * @ingroup shadercompile
 * @brief Interface of the environment used to compile shader
 */
class IShaderCompileEnvironment
{
public:
	/**
	 * @brief Get vertex factory file name
	 * @return Return vertex factory file name. If not set will return empty string
	 */
	virtual const achar* GetVertexFactoryFileName() const = 0;

	/**
	 * @brief Get compile flags
	 * @return Return compile flags (see EShaderCompileFlags)
	 */
	virtual uint32 GetCompileFlags() const = 0;

	/**
	 * @brief Get number of include directories
	 * @return Return number of include directories
	 */
	virtual uint32 GetNumIncludeDirs() const = 0;

	/**
	 * @brief Get number of defines
	 * @return Return number of defines
	 */
	virtual uint32 GetNumDefines() const = 0;

	/**
	 * @brief Get include directory by index
	 * 
	 * @param index			Include directory index
	 * @return Return path to include directory at index
	 */
	virtual const achar* GetIncludeDir( uint32 index ) const = 0;

	/**
	 * @brief Get define by index
	 *
	 * @param index			Define index
	 * @param pName			Output name
	 * @param pValue		Output value
	 */
	virtual void GetDefine( uint32 index, const achar*& pName, const achar*& pValue ) const = 0;
};

/**
 * @ingroup shadercompile
 * @brief Interface of the output shader compile
 */
class IShaderCompileOutput
{
public:
	/**
	 * @brief Set compiled bytecode of the shader
	 * 
	 * @param pData		Data
	 * @param size		Size
	 */
	virtual void SetBytecode( const byte* pData, uint64 size ) = 0;

	/**
	 * @brief Set number instructions in the shader
	 * @param numInstructions		Number instructions
	 */
	virtual void SetNumInstructions( uint32 numInstructions ) = 0;

	/**
	 * @brief Set error message
	 * @param pMsg		Error message
	 */
	virtual void SetErrorMsg( const achar* pMsg ) = 0;

	/**
	 * @brief Add parameter allocation
	 *
	 * @param pParameterName	Parameter name
	 * @param bufferIndex		Buffer index
	 * @param baseIndex			Base index
	 * @param size				Size
	 * @param samplerIndex		Sampler index
	 */
	virtual void AddParameterAllocation( const achar* pParameterName, uint32 bufferIndex, uint32 baseIndex, uint32 size, uint32 samplerIndex ) = 0;
};

/**
 * @ingroup shadercompile
 * @brief Shader compile backend interface version
 */
#define SHADERCOMPILEBACKEND_INTERFACE_VERSION "LShaderCompileBackendRHI001"

/**
 * @ingroup shadercompile
 * @brief Interface of shader compile backend
 */
class IShaderCompileBackend
{
public:
	/**
	 * @brief Connect shader compile backend
	 *
	 * @param pFactory		Pointer to interface factory
	 * @return Return TRUE if successes the backend is connected, otherwise return FALSE
	 */
	virtual bool Connect( CreateInterfaceFn_t pFactory ) = 0;

	/**
	 * @brief Disconnect shader compile backend
	 */
	virtual void Disconnect() = 0;

	/**
	 * @brief Compile shader
	 *
	 * @param pSrcFileName		Path to source file of shader
	 * @param pFunctionName		Main function in the shader
	 * @param type				Shader type (vertex, pixel, etc)
	 * @param pEnvironment		Environment of shader
	 * @param pOutput			Output data after compiling
	 * @param bDebugDump		Is need create debug dump of shader?
	 * @param pShaderSubDir		SubDir for debug dump
	 * @return Return TRUE if compilation is succeed, otherwise returns FALSE
	 */
	virtual bool CompileShader( const achar* pSrcFileName, const achar* pFunctionName, EStudioShaderType type, IShaderCompileEnvironment* pEnvironment, IShaderCompileOutput* pOutput, bool bDebugDump = false, const achar* pShaderSubDir = "" ) = 0;

	/**
	 * @brief Get shader platform name
	 * @return Return shader platform name (dx11, vulkan, etc)
	 */
	virtual const achar* GetShaderPlatform() const = 0;
};

#endif // !ISHADERCOMPILE_BACKEND_H