/**
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

#include "pch_stdlib.h"
#include "stdlib/interface.h"

/**
 * @ingroup stdlib
 * @brief Global list of registered interfaces
 */
static InterfaceReg*	s_pInterfaceRegs = nullptr;

/*
==================
SInterfaceReg::SInterfaceReg
==================
*/
InterfaceReg::InterfaceReg( InstantiateInterfaceFn_t pFunc, const achar* pName )
	: pCreateFn( pFunc )
	, pName( pName )
	, pNext( nullptr )
{
	pNext = s_pInterfaceRegs;
	s_pInterfaceRegs = this;
}

/*
==================
CreateInterface
==================
*/
void* CreateInterface( const achar* pName )
{
	for ( InterfaceReg* pInterfaceReg = s_pInterfaceRegs; pInterfaceReg; pInterfaceReg = pInterfaceReg->pNext )
	{
		if ( !L_Strcmp( pInterfaceReg->pName, pName ) )
		{
			return pInterfaceReg->pCreateFn();
		}
	}
	return nullptr;
}

/*
==================
Sys_GetFactory
==================
*/
CreateInterfaceFn_t Sys_GetFactory( dllHandle_t dllHandle )
{
	if ( dllHandle )
	{
		return ( CreateInterfaceFn_t )Sys_DLL_GetProcAddress( dllHandle, CREATEINTERFACE_FUNCNAME );
	}
	return NULL;
}

/*
==================
Sys_GetFactoryThis
==================
*/
CreateInterfaceFn_t Sys_GetFactoryThis()
{
	return &CreateInterface;
}