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

#include "interfaces/interfaces.h"
#include "stdlib/template.h"
#include "stdlib/filetools.h"
#include "core/debug.h"
#include "filesystem/ifilesystem.h"
#include "gameinfo/gameinfo.h"

/*
==================
CGameInfoDoc::CGameInfoDoc
==================
*/
CGameInfoDoc::CGameInfoDoc()
	: bLoaded( false )
{}

/*
==================
CGameInfoDoc::LoadFromFile
==================
*/
bool CGameInfoDoc::LoadFromFile( const achar* pPath )
{
	// Do nothing if file system isn't valid
	Assert( g_pFileSystem );
	if ( !g_pFileSystem )
	{
		return false;
	}

	// Clear a game info data
	Clear();

	// Try open file
	TRefPtr<IFileReader>	file = g_pFileSystem->CreateFileReader( pPath );
	if ( !file )
	{
		return false;
	}

	// Allocate memory for buffer
	uint64	fileSize	= file->GetSize() + 1;
	byte*	pBuffer		= ( byte* )Mem_MallocZero( fileSize );

	// Serialize data to string buffer
	file->Read( pBuffer, fileSize );

	// Load JSON file and free allocated memory for buffer
	CJsonDoc		jsonGameInfo;
	bool			bResult = jsonGameInfo.LoadFromBuffer( ( const achar* )pBuffer );
	Mem_Free( pBuffer );
	if ( !bResult )
	{
		return false;
	}

	// If all ok grab data from JSON
	std::string		gameinfoPath;
	L_GetFilePath( file->GetPath(), gameinfoPath, false );
	bLoaded = GrabData( jsonGameInfo, gameinfoPath.c_str() );
	return bLoaded;
}

/*
==================
CGameInfoDoc::LoadFromBuffer
==================
*/
bool CGameInfoDoc::LoadFromBuffer( const achar* pBuffer, const achar* pGameInfoPath )
{
	// Clear a game info data
	Clear();

	// Load JSON from buffer
	CJsonDoc		jsonGameInfo;
	if ( !jsonGameInfo.LoadFromBuffer( pBuffer ) )
	{
		return false;
	}

	// If all ok grab data from JSON
	bLoaded = GrabData( jsonGameInfo, pGameInfoPath );
	return bLoaded;
}

/*
==================
CGameInfoDoc::GrabData
==================
*/
bool CGameInfoDoc::GrabData( const CJsonDoc& jsonDoc, const achar* pGameInfoPath )
{
	// Get game name
	{
		CJsonValue		jsonGameVar = jsonDoc.GetValue( "game" );
		if ( jsonGameVar.IsValid() )
		{
			if ( jsonGameVar.IsA( JSONVALUE_TYPE_STRING ) )
			{
				game = jsonGameVar.GetString();
			}
			else
			{
				Warning( "CGameInfoDoc::GrabData: Invalid \"game\", must be string type" );
			}
		}
	}

	// Get search paths
	{
		CJsonValue		jsonSearchPaths = jsonDoc.GetValue( "search_paths" );
		if ( jsonSearchPaths.IsValid() )
		{
			if ( jsonSearchPaths.IsA( JSONVALUE_TYPE_ARRAY ) )
			{
				std::vector<CJsonValue>		jsonVSearchPaths = jsonSearchPaths.GetArray();
				for ( uint32 index = 0, count = ( uint32 )jsonVSearchPaths.size(); index < count; ++index )
				{
					bool					bInvalidElement = true;
					const CJsonValue&		jsonValue		= jsonVSearchPaths[index];
					if ( jsonValue.IsValid() )
					{
						if ( jsonValue.IsA( JSONVALUE_TYPE_OBJECT ) )
						{
							CJsonObject		jsonSearchPath	= jsonValue.GetObject();
							CJsonValue		jsonID			= jsonSearchPath.GetValue( "id" );
							CJsonValue		jsonPath		= jsonSearchPath.GetValue( "path" );
							if ( jsonID.IsValid() && jsonID.IsA( JSONVALUE_TYPE_STRING ) && jsonPath.IsValid() && jsonPath.IsA( JSONVALUE_TYPE_STRING ) )
							{
								GameInfoSearchPath&	searchPath = searchPaths.emplace_back();
								searchPath.id		= jsonID.GetString();
								searchPath.path		= jsonPath.GetString();
								ReplaceMacros( searchPath.path, pGameInfoPath );
								bInvalidElement		= false;
							}
						}
					}

					if ( bInvalidElement )
					{
						Warning( "CGameInfoDoc::GrabData: Invalid element (object type) in \"search_paths\". Must have \"id\" and \"path\", it all string type" );
					}
				}
			}
			else
			{
				Warning( "CGameInfoDoc::GrabData: Invalid \"search_paths\", must be array type" );
			}
		}
	}

	return true;
}

/*
==================
CGameInfoDoc::ReplaceMacros
==================
*/
void CGameInfoDoc::ReplaceMacros( std::string& string, const achar* pGameInfoPath )
{
	enum EGameInfoMacro
	{
		GAMEINFO_MACRO_PLATFORM_SUBDIR,		// |platform_subdir|
		GAMEINFO_MACRO_GAMEINFO_PATH,		// |gameinfo_path|
		GAMEINFO_NUM_MACROS					// Number of macros in gameinfo.txt
	};

	// Table of all macros in gameinfo.txt
	static const achar*		s_pMacrosTable[] =
	{
		"|platform_subdir|",		// GAMEINFO_MACRO_PLATFORM_SUBDIR
		"|gameinfo_path|"			// GAMEINFO_MACRO_GAMEINFO_PATH
	};

	// Table of macros value 
	static const achar*		s_pValueMacroTable[] =
	{
		PLATFORM_SUBDIR,			// |platform_subdir|
		"",							// |gameinfo_path|
	};
	static_assert( ARRAYSIZE( s_pMacrosTable ) == GAMEINFO_NUM_MACROS && ARRAYSIZE( s_pValueMacroTable ) == GAMEINFO_NUM_MACROS, "Need full init s_pMacrosTable and s_pValueMacroTable array" );

	// Update value for |gameinfo_path|
	s_pValueMacroTable[GAMEINFO_MACRO_GAMEINFO_PATH] = pGameInfoPath;

	// Replace all macros in the string
	for ( uint32 macroIndex = 0; macroIndex < GAMEINFO_NUM_MACROS; ++macroIndex )
	{
		const achar*	pCurMacro = s_pMacrosTable[macroIndex];
		const achar*	pCurValue = s_pValueMacroTable[macroIndex];

		size_t		startPos = 0;
		while ( ( startPos = string.find( pCurMacro, startPos ) ) != std::string::npos )
		{
			string.replace( startPos, L_Strlen( pCurMacro ), pCurValue );
			startPos += L_Strlen( pCurValue );
		}
	}
}

/*
==================
CGameInfoDoc::Clear
==================
*/
void CGameInfoDoc::Clear()
{
	game.clear();
	searchPaths.clear();
	bLoaded = false;
}