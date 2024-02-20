/**
 * @file
 * @addtogroup appframework appframework
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

#ifndef IAPPSYSTEM_H
#define IAPPSYSTEM_H

#include "stdlib/interface.h"

/**
 * @ingroup appframework
 * @brief Application system interface
 */
class IAppSystem
{
public:
	/**
	 * @brief Connect application system
	 * 
	 * @param pFactory		Pointer to interface factory
	 * @return Return TRUE if successes application system is connected, otherwise return FALSE 
	 */
	virtual bool Connect( CreateInterfaceFn_t pFactory ) = 0;

	/**
	 * @brief Disconnect application system
	 */
	virtual void Disconnect() = 0;

	/**
	 * @brief Query interface
	 * 
	 * Here's where systems can access other interfaces implemented by this object
	 * 
	 * @param pInterfaceName	Interface name
	 * @return Return pointer to interface, if doesn't implement the requested interface return NULL
	 */
	virtual void* QueryInterface( const achar* pInterfaceName ) = 0;

	/**
	 * @brief Init application system
	 * @return Return TRUE if application system is inited
	 */
	virtual bool Init() = 0;

	/**
	 * @brief Shutdown application system
	 */
	virtual void Shutdown() = 0;
};

/**
 * @ingroup appframework
 * @brief Helper empty implementation of an IAppSystem
 */
template<class TInterface>
class CBaseAppSystem : public TInterface
{
public:
	/**
	 * @brief Connect application system
	 *
	 * @param pFactory		Pointer to interface factory
	 * @return Return TRUE if successes application system is connected, otherwise return FALSE
	 */
	virtual bool Connect( CreateInterfaceFn_t pFactory ) override { return true; }

	/**
	 * @brief Disconnect application system
	 */
	virtual void Disconnect() override {}

	/**
	 * @brief Query interface
	 * 
	 * Here's where systems can access other interfaces implemented by this object
	 *
	 * @param pInterfaceName	Interface name
	 * @return Return pointer to interface, if doesn't implement the requested interface return NULL
	 */
	virtual void* QueryInterface( const achar* pInterfaceName ) override { return nullptr; }

	/**
	 * @brief Init application system
	 * @return Return TRUE if application system is inited
	 */
	virtual bool Init() override { return true; }

	/**
	 * @brief Shutdown application system
	 */
	virtual void Shutdown() override {}
};

#endif // !IAPPSYSTEM_H