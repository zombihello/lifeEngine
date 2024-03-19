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

#include "pch_materialsystem.h"
#include "materialsystem/materialsystem.h"

EXPOSE_SINGLE_INTERFACE( CMaterialSystem, IMaterialSystem, MATERIALSYSTEM_INTERFACE_VERSION );

// Application systems factory. It used for connect materialsystem's submodules (e.g: stdshaders)
CreateInterfaceFn_t	g_pAppSystemFactory = nullptr;

/*
==================
CEngineLauncher::Connect
==================
*/
bool CMaterialSystem::Connect( CreateInterfaceFn_t pFactory )
{
	// Connect StdLib
	if ( !ConnectStdLib( pFactory ) )
	{
		return false;
	}

	// Get the shader system
	g_pShaderSystem = ( IShaderSystem* )pFactory( SHADERSYSTEM_INTERFACE_VERSION );
	if ( !g_pShaderSystem )
	{
		return false;
	}

	g_pAppSystemFactory = pFactory;
	return true;
}

/*
==================
CMaterialSystem::Disconnect
==================
*/
void CMaterialSystem::Disconnect()
{
	g_pShaderSystem = nullptr;
	g_pAppSystemFactory = nullptr;
	DisconnectStdLib();
}

/*
==================
CMaterialSystem::QueryInterface
==================
*/
void* CMaterialSystem::QueryInterface( const achar* pInterfaceName )
{
	CreateInterfaceFn_t		pFactory = Sys_GetFactoryThis();
	return pFactory( pInterfaceName );
}

/*
==================
CMaterialSystem::Init
==================
*/
bool CMaterialSystem::Init()
{
	// Register cvars
	ConVar_Register();

	// Initialize the shader system
	g_pShaderSystem->Init();
	return true;
}

/*
==================
CMaterialSystem::Shutdown
==================
*/
void CMaterialSystem::Shutdown()
{
	// Unregister cvars
	ConVar_Unregister();

	// Shutdown the shader system
	g_pShaderSystem->Shutdown();
}