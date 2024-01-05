#include <list>

#include "Misc/Misc.h"
#include "Containers/String.h"
#include "System/Name.h"

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
	#include "Misc/Names.h"
}

/*
==================
CName::Init
==================
*/
void CName::Init( const std::wstring& InString, uint32 InNumber, EFindName InFindType )
{
	// If empty or invalid name was specified, return NAME_None
	if ( InString.empty() )
	{
		index = NAME_None;
		number = NAME_NO_NUMBER;
		return;
	}

	// Set the number
	number = InNumber;

	// Calculate hash for string
	uint32				hash = Sys_CalcHash( CString::ToUpper( InString ) );

	// Try find already exist name in global table
	std::vector<CName::NameEntry>&		globalNameTable = GetGlobalNameTable();
	for ( uint32 nameEntryId = 0, numNameEntries = globalNameTable.size(); nameEntryId < numNameEntries; ++nameEntryId )
	{
		const CName::NameEntry&		nameEntry = globalNameTable[nameEntryId];
		if ( nameEntry.hash == hash )
		{
			// Found it in the cache
			index = nameEntryId;
			return;
		}
	}

	// Didn't find name
	if ( InFindType == CNAME_Find )
	{
		index = NAME_None;
		number = NAME_NO_NUMBER;
		return;
	}

	// Getting index of name
	index = globalNameTable.size();

	// Allocate new name entry
	AllocateNameEntry( InString, hash );
}

/*
==================
CName::AppendString
==================
*/
void CName::AppendString( std::wstring& OutResult ) const
{
	std::vector<CName::NameEntry>&		globalNameTable = GetGlobalNameTable();
	if ( !IsValid() )
	{
		OutResult += globalNameTable[NAME_None].name;
	}
	else
	{
		OutResult += globalNameTable[index].name;
	}

	if ( number != NAME_NO_NUMBER )
	{
		OutResult += CString::Format( TEXT( "_%i" ), number );
	}
}

/*
==================
CName::operator==
==================
*/
bool CName::operator==( const tchar* InOther ) const
{
	const tchar*	tempName = InOther;
	uint32			tempNumber = NAME_NO_NUMBER;
	uint32			numCharsToCompare = Sys_Strlen( InOther );

	// Find last '_' in InOther
	uint32			idStartNumber = INDEX_NONE;
	for ( uint32 index = 0; index < numCharsToCompare; ++index )
	{
		if ( InOther[index] == TEXT( '_' ) )
		{
			idStartNumber = index;
		}
		else if ( idStartNumber != INDEX_NONE && !Sys_IsDigit( InOther[index] ) )
		{
			idStartNumber = INDEX_NONE;
		}
	}

	// Get number from InOther
	if ( idStartNumber != INDEX_NONE && idStartNumber + 1 < numCharsToCompare )
	{
		tempNumber = Sys_Atoi( InOther + idStartNumber + 1 );
		numCharsToCompare = idStartNumber - 1;
	}

	// Compare
	std::vector<CName::NameEntry>&	globalNameTable = GetGlobalNameTable();
	return tempNumber == number && !Sys_Strnicmp( tempName, globalNameTable[IsValid() ? index : NAME_None].name.c_str(), numCharsToCompare );
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
		InArchive << InValue.number;
	}
	else
	{
		std::wstring	name;
		uint32			index;
		uint32			number = NAME_NO_NUMBER;
		InArchive << name;
		InArchive << index;
		if ( InArchive.Ver() >= VER_NumberInCName )
		{
			InArchive << number;
		}

		// Is name is not valid, setting to NAME_None
		if ( name == TEXT( "" ) || index == INDEX_NONE )
		{
			InValue = NAME_None;
			number = NAME_NO_NUMBER;
		}

		// Otherwise we init name
		else
		{	
			if ( index < globalNameTable.size() && globalNameTable[index].name == name )
			{
				InValue.index = index;
				InValue.number = number;
			}
			else
			{
				InValue.Init( name, number, CNAME_Add );
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
	const CName::NameEntry&			nameEntry		= globalNameTable[InValue.IsValid() ? InValue.index : NAME_None];

	Assert( InArchive.IsSaving() );
	InArchive << nameEntry.name;
	InArchive << InValue.index;
	InArchive << InValue.number;
	return InArchive;
}