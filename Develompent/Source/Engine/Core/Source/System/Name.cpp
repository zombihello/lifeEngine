#include <list>

#include "Misc/Misc.h"
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
static FORCEINLINE void AllocateNameEntry( const std::wstring& InName, uint64 InHash )
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
	std::wstring		tmpBuffer;
	#define REGISTER_NAME( InNum, InName )	\
	{ \
		Assert( InNum == GetGlobalNameTable().size() ); \
		L_Strupr( TEXT( #InName ), tmpBuffer ); \
		AllocateNameEntry( TEXT( #InName ), FastHash( tmpBuffer ) ); \
	}
	#include "Misc/Names.h"
}

/*
==================
CName::EndsWith
==================
*/
bool CName::EndsWith( const CName& InSuffix ) const
{
	if ( *this == InSuffix )
	{
		return true;
	}

	if ( number != InSuffix.number )
	{
		return false;
	}

	std::vector<CName::NameEntry>&		globalNameTable = GetGlobalNameTable();
	std::wstring*						name = IsValid() ? &globalNameTable[index].name : &globalNameTable[NAME_None].name;
	std::wstring*						suffix = InSuffix.IsValid() ? &globalNameTable[InSuffix.index].name : &globalNameTable[NAME_None].name;
	return name->size() >= suffix->size() && L_Strnicmp( name->data() + name->size() - suffix->size(), suffix->data(), suffix->size() );
}

/*
==================
CName::Init
==================
*/
void CName::Init( const std::wstring& InString, uint32 InStringSize, uint32 InNumber, EFindName InFindType )
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
	Assert( InStringSize <= InString.size() );
	uint64	hash = FastHash( L_Strupr( InString ).data(), ( uint64 )InStringSize * sizeof( std::wstring::value_type ), 0 );
	
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
	AllocateNameEntry( std::wstring( InString.data(), InString.data() + InStringSize ), hash);
}

/*
==================
CName::ParseNumber
==================
*/
uint32 CName::ParseNumber( const tchar* InString, uint32& OutIdStartNumber )
{
	uint32		result = NAME_NO_NUMBER;
	uint32		strLength = L_Strlen( InString );

	// Find the last '_' in InOther
	OutIdStartNumber = INDEX_NONE;
	for ( uint32 index = 0; index < strLength; ++index )
	{
		if ( InString[index] == TEXT( '_' ) )
		{
			OutIdStartNumber = index;
		}
		else if ( OutIdStartNumber != INDEX_NONE && !L_IsDigit( InString[index] ) )
		{
			OutIdStartNumber = INDEX_NONE;
		}
	}

	// Get number from InOther
	if ( OutIdStartNumber != INDEX_NONE && OutIdStartNumber + 1 < strLength )
	{
		result = L_Atoi( InString + OutIdStartNumber + 1 );
	}

	// We are done
	return result;
}

/*
==================
CName::AppendString
==================
*/
void CName::AppendString( std::wstring& OutResult, bool InIsWithoutNumber /* = false */ ) const
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

	if ( !InIsWithoutNumber && number != NAME_NO_NUMBER )
	{
		OutResult += L_Sprintf( TEXT( "_%i" ), number );
	}
}

/*
==================
CName::operator==
==================
*/
bool CName::operator==( const tchar* InOther ) const
{
	const tchar*	tempName			= InOther;
	uint32			idStartNumber		= INDEX_NONE;
	uint32			tempNumber			= ParseNumber( InOther, idStartNumber );
	uint32			numCharsToCompare	= L_Strlen( InOther );
	if ( idStartNumber != INDEX_NONE )
	{
		numCharsToCompare = idStartNumber;
	}

	// Compare
	std::vector<CName::NameEntry>&	globalNameTable = GetGlobalNameTable();
	return tempNumber == number && !L_Strnicmp( tempName, globalNameTable[IsValid() ? index : NAME_None].name.c_str(), numCharsToCompare );
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
				InValue.Init( name, name.size(), number, CNAME_Add );
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