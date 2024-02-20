/**
 * @file
 * @addtogroup stdlib stdlib
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

#ifndef INTERFACE_H
#define INTERFACE_H

#include "core/types.h"
#include "core/platform.h"

/**
 * @ingroup stdlib
 * @brief Macro function name of create interface
 */
#define CREATEINTERFACE_FUNCNAME		"CreateInterface"

/**
 * @ingroup stdlib
 * @brief Type pointer to create interface function
 */
typedef void* ( *CreateInterfaceFn_t )( const achar* pName );

/**
 * @ingroup stdlib
 * @brief Type pointer to instantiate interface function
 */
typedef void* ( *InstantiateInterfaceFn_t )();

/**
 * @ingroup stdlib
 * @brief Used internally to register classes
 */
struct InterfaceReg
{
	/**
	 * @brief Constructor
	 * 
	 * @param pFunc	Pointer to instantiate interface function
	 * @param pName	Interface name
	 */
	InterfaceReg( InstantiateInterfaceFn_t pFunc, const achar* pName );

	InstantiateInterfaceFn_t		pCreateFn;		/**< Pointer to instantiate interface function */
	const achar*					pName;			/**< Interface name */
	InterfaceReg*					pNext;			/**< Pointer to next registered interface. Need for global list */
};

/**
 * @ingroup stdlib
 * @brief Macro for expose interface with custom instantiate function
 * 
 * @param FunctionName		Function name
 * @param InterfaceName		Interface name
 * @param VersionName		Version name
 */
#define EXPOSE_INTERFACE_FN( FunctionName, InterfaceName, VersionName ) \
	static InterfaceReg		__s_Create##InterfaceName##_reg( FunctionName, VersionName );

/**
 * @ingroup stdlib
 * @brief Macro for expose interface
 * 
 * @param ClassName			Class name
 * @param InterfaceName		Interface name
 * @param VersionName		Version name
 */
#define EXPOSE_INTERFACE( ClassName, InterfaceName, VersionName ) \
	static void*			__Create##ClassName##_interface() { return ( InterfaceName* )new ClassName; } \
	static InterfaceReg		__s_Create##InterfaceName##_reg( __Create##ClassName##_interface, VersionName );

/**
 * @ingroup stdlib
 * @brief Macro for expose interface with global variable
 * 
 * @param ClassName			Class name
 * @param InterfaceName		Interface name
 * @param VersionName		Version name
 * @param GlobalVarName		Global variable name
 */
#define EXPOSE_SINGLE_INTERFACE_GLOBALVAR( ClassName, InterfaceName, VersionName, GlobalVarName ) \
	static void*			__Create##ClassName##_interface() { return ( InterfaceName* )&GlobalVarName; } \
	static InterfaceReg		__s_Create##InterfaceName##_reg( __Create##ClassName##_interface, VersionName );

/**
 * @ingroup stdlib
 * @brief Macro for expose single interface
 * 
 * @param ClassName			Class name
 * @param InterfaceName		Interface name
 * @param VersionName		Version name
 */
#define EXPOSE_SINGLE_INTERFACE( ClassName, InterfaceName, VersionName ) \
	static ClassName		__s_##ClassName##_singleton; \
	EXPOSE_SINGLE_INTERFACE_GLOBALVAR( ClassName, InterfaceName, VersionName, __s_##ClassName##_singleton )

/**
 * @ingroup stdlib
 * @brief Function-factory of creating interfaces
 * 
 * This function is automatically exported and allows you to access any interfaces exposed with the above macros
 * 
 * @param pName		Interface name
 * @return Return pointer to created interface. If interfaces not found return NULL
 */
extern "C" DLL_EXPORT void* CreateInterface( const achar* pName );

/**
 * @ingroup stdlib
 * @brief Get factory of interfaces in DLL
 * 
 * @param dllHandle		DLL handle
 * @return Return pointer to function of interface factory in DLL. If fail return NULL
 */
CreateInterfaceFn_t Sys_GetFactory( dllHandle_t dllHandle );

/**
 * @ingroup stdlib
 * @brief Get factory of interfaces this module
 * @return Return pointer to function of interface factory this module
 */
CreateInterfaceFn_t Sys_GetFactoryThis();

#endif // !INTERFACE_H