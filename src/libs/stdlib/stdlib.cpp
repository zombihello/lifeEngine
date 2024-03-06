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
#include "interfaces/interfaces.h"
#include "filesystem/ifilesystem.h"
#include "engine/icvar.h"
#include "stdlib/stdlib.h"

// Is StdLib already connected
static bool		s_bConnected = false;

/*
==================
ConnectStdLib
==================
*/
bool ConnectStdLib( CreateInterfaceFn_t pFactory )
{
	// Do nothing if StdLib already is connected
	if ( s_bConnected )
	{
		return true;
	}

	// Try connect file system
	if ( !g_pFileSystem )
	{
		g_pFileSystem = ( IFileSystem* )pFactory( FILESYSTEM_INTERFACE_VERSION );
		if ( !g_pFileSystem )
		{
			return false;
		}
	}

	// Try connect cvar system
	if ( !g_pCvar )
	{
		g_pCvar = ( ICvar* )pFactory( CVAR_INTERFACE_VERSION );
		if ( !g_pCvar )
		{
			return false;
		}
	}

	// StdLib was successfully connected!
	s_bConnected = true;
	return true;
}

/*
==================
DisconnectStdLib
==================
*/
void DisconnectStdLib()
{
	// Do nothing if StdLib already is disconnected
	if ( !s_bConnected )
	{
		return;
	}

	// Reset all app systems and variables
	s_bConnected = false;
	g_pFileSystem = nullptr;
	g_pCvar = nullptr;
}