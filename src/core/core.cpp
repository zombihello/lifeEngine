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

#include "core/globals.h"
#include "core/coreprivate.h"
#include "core/debug.h"
#include "core/version.h"
#include "stdlib/strtools.h"

/*
==================
Sys_Error
==================
*/
void Sys_Error( const achar* pFormat, ... )
{
	// Get formated string
	va_list			params;
	va_start( params, pFormat );
	std::string		message = L_vsprintf( pFormat, params );
	va_end( params );

	// Print message and show message box
	Error( message.c_str() );
	Sys_ShowMessageBox( ENGINE_NAME " Error", message.c_str(), MESSAGE_BOX_ERROR );

	// Shutdown application
	Sys_RequestExit( true );
}

/*
==================
Sys_IsRequestingExit
==================
*/
bool Sys_IsRequestingExit()
{
	return g_bRequestingExit;
}