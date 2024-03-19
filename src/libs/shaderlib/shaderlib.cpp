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

#include "pch_shaderlib.h"
#include "studiorender/studioapi/istudioapi.h"
#include "shaderlib/shaderlib.h"

/*
==================
CShaderLib::Connect
==================
*/
bool CShaderLib::Connect( CreateInterfaceFn_t pFactory )
{
	// Connect StdLib
	if ( !ConnectStdLib( pFactory ) )
	{
		return false;
	}

	// Get StudioAPI
	g_pStudioAPI = ( IStudioAPI* )pFactory( STUDIOAPI_INTERFACE_VERSION );
	if ( !g_pStudioAPI )
	{
		return false;
	}

	return true;
}

/*
==================
CShaderLib::Disconnect
==================
*/
void CShaderLib::Disconnect()
{
	// Disconnect StdLib
	DisconnectStdLib();
	g_pStudioAPI = nullptr;
}

/*
==================
CShaderLib::GetNumShaders
==================
*/
uint32 CShaderLib::GetNumShaders() const
{
	return ( uint32 )shaders.size();
}

/*
==================
CShaderLib::GetShader
==================
*/
IShader* CShaderLib::GetShader( uint32 index ) const
{
	Assert( index < ( uint32 )shaders.size() );
	return shaders[index];
}


/*
==================
GetShaderLib
==================
*/
CShaderLib* GetShaderLib()
{
	static CShaderLib*		s_pShaderLib = nullptr;
	if ( !s_pShaderLib )
	{
		s_pShaderLib = new CShaderLib();
	}

	return s_pShaderLib;
}