/**
 * @file
 * @addtogroup materialsystem materialsystem
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

#ifndef ISHADERSYSTEM_H
#define ISHADERSYSTEM_H

#include "materialsystem/ivertexfactory.h"

// Forward delcarations
class IShader;

/**
 * @ingroup materialsystem
 * @brief Shader system interface version
 */
#define SHADERSYSTEM_INTERFACE_VERSION "LShaderSystem001"

/**
 * @ingroup materialsystem
 * @brief Shader system interface
 */
class IShaderSystem
{
public:
	/**
	 * @brief Initialize
	 * This function initialize the system and loads all engine shader DLLs
	 */
	virtual void Init() = 0;

	/**
	 * @brief Shutdown
	 * This function shutdown the system and unloads all engine shader DLLs
	 */
	virtual void Shutdown() = 0;

	/**
	 * @brief Initialize for the game
	 * This function loads all game specific shader DLLs
	 */
	virtual void GameInit() = 0;

	/**
	 * @brief Shutdown for the game
	 * This function unloads all game specific shader DLLs
	 */
	virtual void GameShutdown() = 0;

	/**
	 * @brief Create vertex factory instance for vfType
	 * 
	 * @param vfType	Vertex factory type (see EVertexFactoryType)
	 * @return Return a new vertex factory instance. If this type isn't implemented by the system returns NULL
	 */
	virtual TRefPtr<IVertexFactory> CreateVertexFactory( EVertexFactoryType vfType ) const = 0;
};

/**
 * @ingroup materialsystem
 * @brief Shader library interface version
 */
#define SHADERLIB_INTERFACE_VERSION "LShaderLib001"

/**
 * @ingroup materialsystem
 * @brief Shader library interface
 */
class IShaderLib
{
public:
	/**
	 * @brief Connect shader library
	 *
	 * @param pFactory		Pointer to interface factory
	 * @return Return TRUE if successes the library is connected, otherwise return FALSE
	 */
	virtual bool Connect( CreateInterfaceFn_t pFactory ) = 0;

	/**
	 * @brief Disconnect shader library
	 */
	virtual void Disconnect() = 0;

	/**
	 * @brief Get number of shaders in this library
	 * @return Return number of shaders in this library
	 */
	virtual uint32 GetNumShaders() const = 0;

	/**
	 * @brief Get shader by index
	 * 
	 * @param index		Index
	 * @return Return shader at index
	 */
	virtual IShader* GetShader( uint32 index ) const = 0;
};

#endif // !ISHADERSYSTEM_H