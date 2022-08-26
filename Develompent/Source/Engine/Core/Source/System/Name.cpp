#include <list>

#include "Misc/Misc.h"
#include "System/Name.h"

static std::list<TSharedPtr<CName::SNameEntry>>& GetGlobalNameTable()
{
	static std::list<TSharedPtr<CName::SNameEntry>>		globalNameTable;
	return globalNameTable;
}

static FORCEINLINE TSharedPtr<CName::SNameEntry> AllocateNameEntry( const std::wstring& InName, uint32 InIndex )
{
	TSharedPtr<CName::SNameEntry>		newNameEntry = MakeSharedPtr<CName::SNameEntry>( InName, InIndex );
	GetGlobalNameTable().push_back( newNameEntry );
	return newNameEntry;
}

void CName::StaticInit()
{
	check( !GetIsInitialized() );
	GetIsInitialized() = true;

	// Register all hardcoded names
	#define REGISTER_NAME( InNum, InName )	AllocateNameEntry( TEXT( #InName ), InNum );
	#include "Misc/Names.h"
}

void CName::Init( const std::wstring& InString )
{
	// Initialize engine names if necessary
	if ( !GetIsInitialized() )
	{
		StaticInit();
	}

	// Transform string to upper case
	std::wstring		upperString = InString;
	for ( uint32 index = 0, count = upperString.size(); index < count; ++index )
	{
		upperString[index] = std::toupper( upperString[index] );
	}

	// Calculate hash for string
	uint32		index = appCalcHash( upperString );

	// Try find already exist name in global table
	std::list<TSharedPtr<CName::SNameEntry>>&	globalNameTable = GetGlobalNameTable();
	for ( auto itBegin = globalNameTable.begin(), itEnd = globalNameTable.end(); itBegin != itEnd; ++itBegin )
	{
		if ( ( *itBegin )->index == index )
		{
			nameEntry = *itBegin;
			return;
		}
	}

	// Allocate new name entry
	AllocateNameEntry( InString, index );
}

void CName::ToString( std::wstring& OutString ) const
{
	if ( nameEntry )
	{
		OutString = nameEntry->name;
	}
	else
	{
		OutString = CName( NAME_None ).ToString();
	}
}

std::wstring CName::ToString() const
{
	std::wstring	result;
	ToString( result );
	return result;
}

bool CName::operator==( const std::wstring& InOther ) const
{
	if ( !nameEntry )
	{
		return false;
	}

	// Transform string to upper case
	std::wstring		upperString = InOther;
	for ( uint32 index = 0, count = upperString.size(); index < count; ++index )
	{
		upperString = std::toupper( upperString[index] );
	}

	// Calculate hash for string
	uint32		index = appCalcHash( upperString );

	return nameEntry->index == index;
}

CName& CName::operator=( EName InOther )
{
	// Try find engine name in global table
	std::list<TSharedPtr<CName::SNameEntry>>&	globalNameTable = GetGlobalNameTable();
	for ( auto itBegin = globalNameTable.begin(), itEnd = globalNameTable.end(); itBegin != itEnd; ++itBegin )
	{
		if ( ( *itBegin )->index == InOther )
		{
			nameEntry = *itBegin;
			return *this;
		}
	}

	appErrorf( TEXT( "Engine name 0x%X not founded. Need call CName::StaticInit before using" ), InOther );
	return *this;
}