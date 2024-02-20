/**
 * @file
 * @addtogroup core core
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

#ifndef ICOMMANDLINE_H
#define ICOMMANDLINE_H

#include "core/coreapi.h"
#include "core/types.h"

/**
 * @ingroup core
 * @brief Interface command line
 * Holds parsed data from command line like strings
 *
 * Usage:
 * @code
 *		-<Param>=<Value>
 *		/<Param>=<Value>
 *		-<Param>="<Value> <Value>"
 *		/<Param>="<Value> <Value>"
 *		-<Param>
 *		/<Param>
 *		-<Param> <Value>
 *		/<Param> <Value>
 *		-<Param> "<Value> <Value>"
 *		/<Param> "<Value> <Value>"
 * @endcode
 */
class ICommandLine
{
public:
	/**
	 * @brief Destructor
	 */
	virtual ~ICommandLine() {}

	/**
	 * @brief Init command line
	 * @param pCommandLine	Command line
	 */
	virtual void Init( const achar* pCommandLine ) = 0;

	/**
	 * @brief Shutdown command line
	 */
	virtual void Shutdown() = 0;

	/**
	 * @brief Is has a param
	 *
	 * @param pParam	Param to find
	 * @return Return TRUE if command line is containing a param, otherwise will returning FALSE
	 */
	virtual bool HasParam( const achar* pParam ) const = 0;

	/**
	 * @brief Is has a param with value
	 *
	 * @param pParam		Param to find
	 * @param pValue		Value of param
	 * @return Return TRUE if command line is containing a param with value, otherwise will returning FALSE
	 */
	virtual bool HasParam( const achar* pParam, const achar* pValue ) const = 0;

	/**
	 * @brief Get first value from param
	 *
	 * @param pParam	Param
	 * @return Return first value in param. If not exist return empty string
	 */
	virtual const achar* GetFirstValue( const achar* pParam ) const = 0;

	/**
	 * @brief Get values from param
	 *
	 * @param pParam	Param
	 * @param size		Output count values
	 * @return Return array of values by param. If it is not exist in command line will return NULL
	 */
	virtual const achar** GetValues( const achar* pParam, uint32& size ) const = 0;
};

/**
 * @ingroup core
 * @brief Get command line
 * @return Return command line
 */
CORE_INTERFACE ICommandLine* CommandLine();

#endif // !ICOMMANDLINE_H