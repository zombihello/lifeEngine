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

#include "Engine/Misc/EngineGlobals.h"
#include "Engine/System/ConCmd.h"
#include "Engine/System/ConsoleSystem.h"

/*
==================
CConCmd::CConCmd
==================
*/
CConCmd::CConCmd( const std::wstring& InName, const std::wstring& InHelpText, const COnExecCmd::DelegateType_t& InExecDelegate )
	: name( InName )
	, helpText( InHelpText )
{
	onExecCmd.Bind( InExecDelegate );
	GetGlobalConCmds().push_back( this );
}

/*
==================
CConCmd::~CConCmd
==================
*/
CConCmd::~CConCmd()
{
	std::vector<CConCmd*>&		cmds = GetGlobalConCmds();
	for ( uint32 index = 0, count = cmds.size(); index < count; ++index )
	{
		if ( cmds[index] == this )
		{
			cmds.erase( cmds.begin() + index );
			break;
		}
	}
}