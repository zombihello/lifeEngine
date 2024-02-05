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

#include <list>

#include "Core/Misc/Misc.h"
#include "Core/Containers/String.h"
#include "Core/System/Name.h"

 /*
 ==================
 GetGlobalNameTable
 ==================
 */
static std::vector<CName::NameEntry>& GetGlobalNameTable()
{
	static std::vector<CName::NameEntry>		globalNameTable;
	return globalNameTable;
}

/*
==================
AllocateNameEntry
==================
*/
static FORCEINLINE void AllocateNameEntry( const std::wstring& InName, uint32 InHash )
{
	GetGlobalNameTable().push_back( CName::NameEntry( InName, InHash ) );
}

/*
==================
CName::StaticInit
==================
*/
void CName::StaticInit()
{
	// Initialize engine names if necessary
	if ( GetIsInitialized() )
	{
		return;
	}

	Assert( GetGlobalNameTable().empty() );
	GetIsInitialized() = true;

	// Register all hardcoded names
	#define REGISTER_NAME( InNum, InName )	\
	{ \
		Assert( InNum == GetGlobalNameTable().size() ); \
		AllocateNameEntry( TEXT( #InName ), Sys_CalcHash( CString::ToUpper( TEXT( #InName ) ) ) ); \
	}
	#include "Core/Misc/Names.h"
}

/*
==================
CName::Init
==================
*/
void CName::Init( const std::wstring& InString )
{
	// Calculate hash for string
	uint32				hash = Sys_CalcHash( CString::ToUpper( InString ) );

	// Try find already exist name in global table
	std::vector<CName::NameEntry>&		globalNameTable = GetGlobalNameTable();
	for ( uint32 nameEntryId = 0, numNameEntries = globalNameTable.size(); nameEntryId < numNameEntries; ++nameEntryId )
	{
		const CName::NameEntry&		nameEntry = globalNameTable[nameEntryId];
		if ( nameEntry.hash == hash )
		{
			index = nameEntryId;
			return;
		}
	}

	// Getting index of name
	index = globalNameTable.size();

	// Allocate new name entry
	AllocateNameEntry( InString, hash );
}

/*
==================
CName::ToString
==================
*/
void CName::ToString( std::wstring& OutString ) const
{
	std::vector<CName::NameEntry>&		globalNameTable = GetGlobalNameTable();
	if ( !IsValid() )
	{
		OutString = globalNameTable[NAME_None].name;
	}
	else
	{
		OutString = globalNameTable[index].name;
	}
}

/*
==================
CName::ToString
==================
*/
std::wstring CName::ToString() const
{
	std::wstring	result;
	ToString( result );
	return result;
}

/*
==================
CName::operator==
==================
*/
bool CName::operator==( const std::wstring& InOther ) const
{
	// Calculate hash for string
	std::vector<CName::NameEntry>&		globalNameTable = GetGlobalNameTable();
	uint32								hash = Sys_CalcHash( CString::ToUpper( InOther ) );

	return globalNameTable[IsValid() ? index : NAME_None].hash == hash;
}

/*
==================
CName::operator==
==================
*/
bool CName::operator==( const tchar* InOther ) const
{
	// Calculate hash for string
	std::vector<CName::NameEntry>&		globalNameTable = GetGlobalNameTable();
	uint32								hash = Sys_CalcHash( CString::ToUpper( InOther ) );

	return globalNameTable[IsValid() ? index : NAME_None].hash == hash;
}

/*
==================
CName::operator<<
==================
*/
CArchive& operator<<( CArchive& InArchive, CName& InValue )
{
	std::vector<CName::NameEntry>&		globalNameTable = GetGlobalNameTable();

	if ( InArchive.IsSaving() )
	{
		const CName::NameEntry& nameEntry = globalNameTable[InValue.IsValid() ? InValue.index : NAME_None];
		InArchive << nameEntry.name;
		InArchive << InValue.index;
	}
	else
	{
		std::wstring	name;
		uint32			index;
		InArchive << name;
		InArchive << index;

		// Is name is not valid, setting to NAME_None
		if ( name == TEXT( "" ) || index == INDEX_NONE )
		{
			InValue = NAME_None;
		}

		// Else we init name
		else
		{	
			if ( index < globalNameTable.size() && globalNameTable[index].name == name )
			{
				InValue.index = index;
			}
			else
			{
				InValue.Init( name );
			}
		}
	}

	return InArchive;
}

/*
==================
operator<<
==================
*/
CArchive& operator<<( CArchive& InArchive, const CName& InValue )
{
	std::vector<CName::NameEntry>&	globalNameTable = GetGlobalNameTable();
	const CName::NameEntry&		nameEntry		= globalNameTable[InValue.IsValid() ? InValue.index : NAME_None];

	Assert( InArchive.IsSaving() );
	InArchive << nameEntry.name;
	InArchive << InValue.index;
	return InArchive;
}