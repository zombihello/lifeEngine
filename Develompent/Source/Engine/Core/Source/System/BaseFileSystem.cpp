#include "System/BaseFileSystem.h"

bool FBaseFileSystem::DeleteDirectory( const std::wstring& InPath, bool InIsTree )
{
	// Support code for removing a directory tree.
	check( InIsTree );
	if ( InPath.empty() )
	{
		return false;
	}

	// Delete all files
	std::vector< std::wstring >		list = FindFiles( InPath, true, false );
	for ( uint32 index = 0, count = list.size(); index < count; ++index )
	{
		if ( !Delete( InPath + PATH_SEPARATOR + list[ index ], true ) )
		{
			return false;
		}
	}

	// Delete all directories
	list = FindFiles( InPath, false, true );
	for ( uint32 index = 0, count = list.size(); index < count; ++index )
	{
		if ( !DeleteDirectory( InPath + PATH_SEPARATOR + list[ index ], true ) )
		{
			return false;
		}
	}

	return DeleteDirectory( InPath, false );
}