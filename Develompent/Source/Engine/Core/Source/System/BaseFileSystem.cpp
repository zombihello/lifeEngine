#include "Misc/Misc.h"
#include "Misc/CoreGlobals.h"
#include "System/System.h"
#include "System/BaseFileSystem.h"

/*
==================
CFilename::CFilename
==================
*/
CFilename::CFilename()
{}

/*
==================
CFilename::CFilename
==================
*/
CFilename::CFilename( const std::wstring& InPath )
	: path( InPath )
{
	Sys_NormalizePathSeparators( path );
}

/*
==================
CFilename::GetExtension
==================
*/
std::wstring CFilename::GetExtension( bool InIsIncludeDot /* = false */ ) const
{
	std::wstring	result = path;
	std::size_t		dotPos = result.find_last_of( TEXT( "." ) );
	if ( dotPos == std::wstring::npos )
	{
		return TEXT( "" );
	}

	result.erase( 0, dotPos + ( InIsIncludeDot ? 0 : 1 ) );
	return result;
}

/*
==================
CFilename::GetBaseFilename
==================
*/
std::wstring CFilename::GetBaseFilename() const
{
	std::wstring	result = path;
	std::size_t		lastSlashPos = result.find_last_of( PATH_SEPARATOR );
	if ( lastSlashPos != std::wstring::npos )
	{
		result.erase( 0, lastSlashPos + 1 );
	}

	std::size_t		dotPos = result.find_last_of( TEXT( "." ) );
	if ( dotPos != std::wstring::npos )
	{
		result.erase( dotPos, result.size() );
	}

	return result;
}

/*
==================
CFilename::GetPath
==================
*/
std::wstring CFilename::GetPath() const
{
	std::wstring	result = path;
	std::size_t		lastSlashPos	= result.find_last_of( PATH_SEPARATOR );
	if ( lastSlashPos != std::wstring::npos )
	{
		result.erase( lastSlashPos < result.size() ? lastSlashPos+1 : lastSlashPos, result.size() );
	}
	else
	{
		std::size_t		dotPos = result.find_last_of( TEXT( "." ) );
		if ( dotPos != std::wstring::npos )
		{
			result.erase( dotPos, result.size() );
		}
	}

	return result;
}

/*
==================
CFilename::GetLocalizedFilename
==================
*/
std::wstring CFilename::GetLocalizedFilename( const std::wstring& InLanguage /* = TEXT( "" ) */ ) const
{
	// Use default language if none specified
	std::wstring	language = InLanguage;
	if ( language.empty() )
	{
		language = TEXT( "INT" );
	}
	
	// Prepend path and path separator
	std::wstring	localizedPath = GetPath();
	if ( !localizedPath.empty() )
	{
		localizedPath += PATH_SEPARATOR;
	}

	// Append _LANG to filename
	localizedPath += GetBaseFilename() + TEXT( "_" ) + language;

	// Append extension if used
	std::wstring	extension = GetExtension( true );
	if ( !extension.empty() )
	{
		localizedPath += extension;
	}

	return localizedPath;
}

/*
==================
CFilename::IsInDirectory
==================
*/
bool CFilename::IsInDirectory( const std::wstring& InPath ) const
{
	std::wstring	dirPath		= g_FileSystem->ConvertToAbsolutePath( InPath );
	std::wstring	localPath	= g_FileSystem->ConvertToAbsolutePath( path );
	return localPath.substr( 0, dirPath.size() ) == dirPath;
}

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

/*
==================
CBaseFileSystem::ConvertRelativePathToFull
==================
*/
std::wstring CBaseFileSystem::ConvertRelativePathToFull( const std::wstring& InPath )
{
	std::wstring	fullyPathed;
	if ( CString::StartWith( InPath, TEXT( "../" ) ) || CString::StartWith( InPath, TEXT( "..\\" ) ) )
	{
		fullyPathed = Sys_BaseDir();
	}

	return CollapseRelativeDirectories( fullyPathed + InPath );
}

/*
==================
CBaseFileSystem::CollapseRelativeDirectories
==================
*/
std::wstring CBaseFileSystem::CollapseRelativeDirectories( const std::wstring& InPath )
{
	// For each occurrance of "..PATH_SEPARATOR" eat a directory to the left
	std::wstring	resultPath = InPath;
	std::wstring	leftString;
	std::wstring	rightString;
	Sys_NormalizePathSeparators( resultPath );

	const std::wstring	searchString = TEXT( ".." ) PATH_SEPARATOR;
	while ( CString::Split( resultPath, searchString, leftString, rightString ) )
	{
		// Strip the first directory off LeftString
		int32		index = leftString.size()-1;

		// Eat the slash on the end of left if there is one
		if ( index >= 0 && leftString[index] == PATH_SEPARATOR[0] )
		{
			--index;
		}

		// Eat leftwards until a slash is hit
		while ( index >= 0 && leftString[index] != PATH_SEPARATOR[0] )
		{
			leftString[index--] = 0;
		}

		resultPath = leftString + rightString;
	}

	return resultPath;
}