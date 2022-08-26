#include <list>

#include "Misc/Misc.h"
#include "Containers/String.h"
#include "System/Name.h"

static std::vector<CName::SNameEntry>& GetGlobalNameTable()
{
	static std::vector<CName::SNameEntry>		globalNameTable;
	return globalNameTable;
}

static FORCEINLINE void AllocateNameEntry( const std::wstring& InName, uint32 InHash )
{
	GetGlobalNameTable().push_back( CName::SNameEntry( InName, InHash ) );
}

void CName::StaticInit()
{
	// Initialize engine names if necessary
	if ( GetIsInitialized() )
	{
		return;
	}

	check( GetGlobalNameTable().empty() );
	GetIsInitialized() = true;

	// Register all hardcoded names
	#define REGISTER_NAME( InNum, InName )	\
	{ \
		check( InNum == GetGlobalNameTable().size() ); \
		AllocateNameEntry( TEXT( #InName ), appCalcHash( CString::ToUpper( TEXT( #InName ) ) ) ); \
	}
	#include "Misc/Names.h"
}

void CName::Init( const std::wstring& InString )
{
	// Calculate hash for string
	uint32				hash = appCalcHash( CString::ToUpper( InString ) );

	// Try find already exist name in global table
	std::vector<CName::SNameEntry>&		globalNameTable = GetGlobalNameTable();
	for ( uint32 nameEntryId = 0, numNameEntries = globalNameTable.size(); nameEntryId < numNameEntries; ++nameEntryId )
	{
		const CName::SNameEntry&		nameEntry = globalNameTable[nameEntryId];
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

void CName::ToString( std::wstring& OutString ) const
{
	std::vector<CName::SNameEntry>&		globalNameTable = GetGlobalNameTable();
	if ( !IsValid() )
	{
		OutString = globalNameTable[NAME_None].name;
	}
	else
	{
		OutString = globalNameTable[index].name;
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
	// Calculate hash for string
	std::vector<CName::SNameEntry>&		globalNameTable = GetGlobalNameTable();
	uint32								hash = appCalcHash( CString::ToUpper( InOther ) );

	return globalNameTable[IsValid() ? index : NAME_None].hash == hash;
}

CArchive& operator<<( CArchive& InArchive, CName& InValue )
{
	std::vector<CName::SNameEntry>&		globalNameTable = GetGlobalNameTable();

	if ( InArchive.IsSaving() )
	{
		const CName::SNameEntry& nameEntry = globalNameTable[InValue.IsValid() ? InValue.index : NAME_None];
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

CArchive& operator<<( CArchive& InArchive, const CName& InValue )
{
	std::vector<CName::SNameEntry>&	globalNameTable = GetGlobalNameTable();
	const CName::SNameEntry&		nameEntry		= globalNameTable[InValue.IsValid() ? InValue.index : NAME_None];

	check( InArchive.IsSaving() );
	InArchive << nameEntry.name;
	InArchive << InValue.index;
	return InArchive;
}