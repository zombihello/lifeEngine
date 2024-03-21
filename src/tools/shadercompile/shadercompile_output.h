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

#ifndef SHADERCOMPILE_OUTPUT_H
#define SHADERCOMPILE_OUTPUT_H

#include <string>
#include <vector>

#include "tools/shadercompile/ishadercompile_backend.h"

/**
 * @ingroup shadercompile
 * @brief The output of the shader compile
 */
class CShaderCompileOutput : public IShaderCompileOutput
{
public:
	/**
	 * @brief Constructor
	 */
	CShaderCompileOutput();

	/**
	 * @brief Set compiled bytecode of the shader
	 * 
	 * @param pData		Data
	 * @param size		Size
	 */
	virtual void SetBytecode( const byte* pData, uint64 size ) override;

	/**
	 * @brief Set number instructions in the shader
	 * @param numInstructions		Number instructions
	 */
	virtual void SetNumInstructions( uint32 numInstructions ) override;

	/**
	 * @brief Set error message
	 * @param pMsg		Error message
	 */
	virtual void SetErrorMsg( const achar* pMsg ) override;

	/**
	 * @brief Add parameter allocation
	 *
	 * @param pParameterName	Parameter name
	 * @param bufferIndex		Buffer index
	 * @param baseIndex			Base index
	 * @param size				Size
	 * @param samplerIndex		Sampler index
	 */
	virtual void AddParameterAllocation( const achar* pParameterName, uint32 bufferIndex, uint32 baseIndex, uint32 size, uint32 samplerIndex ) override;

	/**
	 * @brief Get bytecode
	 * @return Return bytecode
	 */
	FORCEINLINE const std::vector<byte>& GetBytecode() const
	{
		return bytecode;
	}

	/**
	 * @brief Get error message
	 * @return Return error message. If not exist returns empty string
	 */
	FORCEINLINE const std::string& GetErrorMsg() const
	{
		return errorMsg;
	}

	/**
	 * @brief Get number of instructions
	 * @return Return number of instructions
	 */
	FORCEINLINE uint32 GetNumInstructions() const
	{
		return numInstructions;
	}

private:
	std::vector<byte>	bytecode;			/**< Output compiled code of shader */
	uint32				numInstructions;	/**< Number of instructions in shader */
	std::string			errorMsg;			/**< Error message. Compile putting to this field message when shader compiled is fail */
};

#endif // !SHADERCOMPILE_OUTPUT_H