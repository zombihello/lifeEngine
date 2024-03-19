/**
 * @file
 * @addtogroup shaderlib shaderlib
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

#ifndef SHADERLIB_H
#define SHADERLIB_H

#include <vector>
#include "materialsystem/ishadersystem.h"

/**
 * @ingroup shaderslibrary
 * @brief Shaders library
 */
class CShaderLib : public IShaderLib
{
public:
	/**
	 * @brief Connect shader library
	 *
	 * @param pFactory		Pointer to interface factory
	 * @return Return TRUE if successes the library is connected, otherwise return FALSE
	 */
	virtual bool Connect( CreateInterfaceFn_t pFactory ) override;

	/**
	 * @brief Disconnect shader library
	 */
	virtual void Disconnect() override;

	/**
	 * @brief Get number of shaders in this library
	 * @return Return number of shaders in this library
	 */
	virtual uint32 GetNumShaders() const override;

	/**
	 * @brief Get shader by index
	 *
	 * @param index		Index
	 * @return Return shader at index
	 */
	virtual IShader* GetShader( uint32 index ) const override;

	/**
	 * @brief Adds a shader to the list of shaders
	 * @param pShader	Shader
	 */
	FORCEINLINE void InsertShader( IShader* pShader )
	{
		Assert( pShader );
		shaders.push_back( pShader );
	}

private:
	std::vector<IShader*>		shaders;	/**< Shaders */
};

/**
 * @ingroup shaderslibrary
 * @brief Get singleton instance of the shader library
 * @return Return singleton instance of the shader library
 */
CShaderLib* GetShaderLib();

#endif // !SHADERLIB_H