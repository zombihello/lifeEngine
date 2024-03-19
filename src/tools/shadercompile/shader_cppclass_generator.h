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

#ifndef SHADER_CPPCLASS_GENERATOR_H
#define SHADER_CPPCLASS_GENERATOR_H

#include "tools/shadercompile/shadercompile.h"

/**
 * @ingroup shadercompile
 * @brief Generator of a helper C++ class for shader
 */
class CShaderCppClassGenerator
{
public:
	/**
	 * @brief Generate a helper C++ class for shader
	 * @param shader	Shader
	 */
	void Generate( const Shader& shader );

	/**
	 * @brief Reset generator
	 */
	FORCEINLINE void Reset()
	{
		buffer.clear();
	}

	/**
	 * @brief Get generated C++ class
	 * @return Return generated C++ class. Returns an empty string if Generate() was not called or Reset() was called
	 */
	FORCEINLINE const std::string& GetBuffer() const
	{
		return buffer;
	}

private:
	/**
	 * @brief Generate var and set function of the one for a shader flag
	 * @param shaderFlag	Shader flag
	 */
	void GenerateVar( const ShaderFlag& shaderFlag );

	/**
	 * @brief Generate class constructor
	 * 
	 * @param pClassName	Class name
	 * @param shader		Shader
	 */
	void GenerateConstructor( const achar* pClassName, const Shader& shader );

	/**
	 * @brief Generate function for calculate a shader index for the flags combination
	 * @param shader	Shader
	 */
	void GenerateGetIndexFunc( const Shader& shader );

	std::string		buffer;		/**< Buffer of generated C++ class */
};

#endif // !SHADER_CPPCLASS_GENERATOR_H