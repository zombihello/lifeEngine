#include "Misc/Misc.h"
#include "Misc/CoreGlobals.h"
#include "System/System.h"
#include "System/BaseFileSystem.h"

/*
==================
CFilename::MakeDirectory
==================
*/
bool CBaseFileSystem::MakeDirectory( const std::wstring& InPath, bool InIsTree /* = false */ )
{
	// Support code for making a directory tree
	Assert( InIsTree );
	uint32			createCount = 0;
	std::wstring	fullPath;

	for ( uint32 index = 0, count = InPath.size(); index < count; ++index )
	{
		fullPath += InPath[index];
		if ( ( Sys_IsPathSeparator( InPath[index] ) || index+1 == count ) && !IsDrive( fullPath ) )
		{
			if ( !MakeDirectory( fullPath, false ) )
			{
				return false;
			}
			++createCount;
		}
	}

	return createCount != 0;
}

/*
==================
CFilename::DeleteDirectory
==================
*/
bool CBaseFileSystem::DeleteDirectory( const std::wstring& InPath, bool InIsTree /* = false */ )
{
	// Support code for removing a directory tree.
	Assert( InIsTree );
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

/*
==================
CFilename::IsDrive
==================
*/
bool CBaseFileSystem::IsDrive( const std::wstring& InPath ) const
{
	return InPath.empty() || InPath == TEXT( "\\" ) || InPath == TEXT( "\\\\" ) || InPath == TEXT( "//" ) || InPath == TEXT( "////" );
}


/*
==================
CBaseFileSystem::FindFilesInDirectory
==================
*/
void CBaseFileSystem::FindFilesInDirectory( std::vector<std::wstring>& OutResults, const tchar* InRootDirectory, bool InIsFindPackages, bool InIsFindNonPackages )
{
	// Cache a temp string of the root directory
	std::wstring	rootDirectory = InRootDirectory;

	// Find the directories, not files
	std::vector<std::wstring>	subDirs = FindFiles( rootDirectory, false, true );

	// Recurse through the directories looking for more files/directories
	for ( uint32 subDirIndex = 0, countSubDirCount = subDirs.size(); subDirIndex < countSubDirCount; ++subDirIndex )
	{
		FindFilesInDirectory( OutResults, ( rootDirectory + PATH_SEPARATOR + subDirs[subDirIndex] ).c_str(), InIsFindPackages, InIsFindNonPackages );
	}

	// Look for any files in this directory
	std::vector<std::wstring>	files = FindFiles( rootDirectory, true, false );

	// Go over the file list
	for ( uint32 fileIndex = 0, fileCount = files.size(); fileIndex < fileCount; ++fileIndex )
	{
		// Create a filename out of the found file
		CFilename	filename( files[fileIndex] );

		// This file is a package if its extension is registered as a package extension
		// Warning: If we are doing this before CSsystem is setup, then we can't tell if it's a valid
		bool	bIsPackage = CSystem::Get().IsPackageExtension( filename.GetExtension() );

		// Add this file if its a package and we want packages, or vice versa
		if ( ( InIsFindPackages && bIsPackage ) || ( InIsFindNonPackages && !bIsPackage ) )
		{
			OutResults.push_back( rootDirectory + PATH_SEPARATOR + files[fileIndex] );
		}
	}
}