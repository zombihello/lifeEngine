#include <Windows.h>

#include "Core.h"
#include "WindowsFileSystem.h"
#include "WindowsArchive.h"
#include "Misc/Misc.h"
#include "Logger/LoggerMacros.h"

/*
==================
CWindowsFileSystem::CWindowsFileSystem
==================
*/
CWindowsFileSystem::CWindowsFileSystem()
{}

/*
==================
CWindowsFileSystem::~CWindowsFileSystem
==================
*/
CWindowsFileSystem::~CWindowsFileSystem()
{}

/*
==================
CWindowsFileSystem::CreateFileReader
==================
*/
class CArchive* CWindowsFileSystem::CreateFileReader( const std::wstring& InFileName, uint32 InFlags )
{
	std::ifstream*			inputFile = new std::ifstream();

	// Create file and create archive reader
	inputFile->open( InFileName, std::ios::binary );
	if ( !inputFile->is_open() )
	{
		if ( InFlags & AR_NoFail )
		{
			Sys_Error( TEXT( "Failed to create file: %s, InFlags = 0x%X" ), InFileName.c_str(), InFlags );
		}

		delete inputFile;
		return nullptr;
	}

	return new CWindowsArchiveReading( inputFile, InFileName );
}

/*
==================
CWindowsFileSystem::CreateFileWriter
==================
*/
class CArchive* CWindowsFileSystem::CreateFileWriter( const std::wstring& InFileName, uint32 InFlags )
{
	std::ofstream*			outputFile = new std::ofstream();
	int						flags = 0;

	if ( InFlags & AW_Append )
	{
		flags = std::ios::app;		
	}

	// Create directory for file
	{
		std::wstring			path = InFileName;
		std::size_t				slashIndex = path.find_last_of( TEXT( "/" ) );
		if ( slashIndex == std::wstring::npos )
		{
			slashIndex = path.find_last_of( TEXT( "\\" ) );
		}

		if ( slashIndex != std::wstring::npos )
		{
			path.erase( slashIndex, path.size() );
			CreateDirectoryW( path.c_str(), nullptr );
		}
	}

	// Create file and create archive writer
	outputFile->open( InFileName, flags | std::ios::binary | std::ios::ate );
	if ( !outputFile->is_open() )
	{
		if ( InFlags & AW_NoFail )
		{
			Sys_Error( TEXT( "Failed to create file: %s, InFlags = %X" ), InFileName.c_str(), InFlags );
		}

		delete outputFile;
		return nullptr;
	}

	return new CWindowsArchiveWriter( outputFile, InFileName );
}

/*
==================
CWindowsFileSystem::FindFiles
==================
*/
std::vector< std::wstring > CWindowsFileSystem::FindFiles( const std::wstring& InDirectory, bool InIsFiles, bool InIsDirectories )
{
	HANDLE								handle = nullptr;
	WIN32_FIND_DATAW					data;
	std::vector< std::wstring >			result;

	handle = FindFirstFileW( L_Sprintf( TEXT( "%s/*" ), InDirectory.c_str() ).c_str(), &data );
	if ( handle != INVALID_HANDLE_VALUE )
	{
		do
		{
			if ( wcscmp( data.cFileName, TEXT( "." ) ) && wcscmp( data.cFileName, TEXT( ".." ) ) &&
				 ( ( data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) ? InIsDirectories : InIsFiles ) )
			{
				result.push_back( data.cFileName );
			}
		}
		while ( FindNextFileW( handle, &data ) );
	}
	if ( handle != INVALID_HANDLE_VALUE )
	{
		FindClose( handle );
	}

	return result;
}

/*
==================
CWindowsFileSystem::Delete
==================
*/
bool CWindowsFileSystem::Delete( const std::wstring& InPath, bool InIsEvenReadOnly /* = false */ )
{
	if ( InIsEvenReadOnly )
	{
		SetFileAttributesW( InPath.c_str(), FILE_ATTRIBUTE_NORMAL );
	}

	int32		result	= DeleteFile( InPath.c_str() ) != 0;
	int32		error	= GetLastError();
	result = result || error == ERROR_FILE_NOT_FOUND || error == ERROR_PATH_NOT_FOUND;
	if ( !result )
	{
		if ( g_IsCommandlet )
		{
			// This is not an error while doing commandlets
			Warnf( TEXT( "Could not delete '%s'\n" ), InPath.c_str() );
		}
		else
		{
			Sys_Error( TEXT( "Error deleting file '%s' (GetLastError: %d)" ), InPath.c_str(), error );
		}
	}

	return result != 0;
}

/*
==================
CWindowsFileSystem::MakeDirectory
==================
*/
bool CWindowsFileSystem::MakeDirectory( const std::wstring& InPath, bool InIsTree /* = false */ )
{
	if ( InIsTree )
	{
		return CBaseFileSystem::MakeDirectory( InPath, InIsTree );
	}
	return CreateDirectoryW( InPath.c_str(), nullptr ) != 0 || GetLastError() == ERROR_ALREADY_EXISTS;
}

/*
==================
CWindowsFileSystem::DeleteDirectory
==================
*/
bool CWindowsFileSystem::DeleteDirectory( const std::wstring& InPath, bool InIsTree )
{
	if ( InIsTree )
	{
		return CBaseFileSystem::DeleteDirectory( InPath, InIsTree );
	}

	bool		result = RemoveDirectoryW( InPath.c_str() );
	if ( !result )
	{
		Warnf( TEXT( "Failed deleting directory '%s'. GetLastError() = 0x%X\n" ), InPath.c_str(), GetLastError() );
	}
	return result;
}

/*
==================
CWindowsFileSystem::Copy
==================
*/
ECopyMoveResult CWindowsFileSystem::Copy( const std::wstring& InDstFile, const std::wstring& InSrcFile, bool InIsReplaceExisting /* = false */, bool InIsEvenReadOnly /* = false */ )
{
	if ( InIsEvenReadOnly )
	{
		SetFileAttributesW( InDstFile.c_str(), 0 );
	}

	ECopyMoveResult		result;
	MakeDirectory( CFilename( InDstFile ).GetPath(), true );
	if ( CopyFileW( InSrcFile.c_str(), InDstFile.c_str(), !InIsReplaceExisting ) != 0 )
	{
		result = CMR_OK;
	}
	else
	{
		result = CMR_MiscFail;
	}

	if ( result == CMR_OK )
	{
		SetFileAttributesW( InDstFile.c_str(), 0 );
	}
	return result;
}

/*
==================
CWindowsFileSystem::Move
==================
*/
ECopyMoveResult CWindowsFileSystem::Move( const std::wstring& InDstFile, const std::wstring& InSrcFile, bool InIsReplaceExisting /* = false */, bool InIsEvenReadOnly /* = false */ )
{
	MakeDirectory( CFilename( InDstFile ).GetPath(), true );

	DWORD		moveFlags = ( InIsReplaceExisting ? MOVEFILE_REPLACE_EXISTING : 0x0 ) | MOVEFILE_WRITE_THROUGH;
	int32		result = MoveFileExW( InSrcFile.c_str(), InDstFile.c_str(), moveFlags );
	if ( !result )
	{
		// If the move failed, throw a warning but retry before we throw an error
		DWORD		error = GetLastError();
		Warnf( TEXT( "MoveFileExW was unable to move '%s' to '%s', retrying... (GetLastE-r-r-o-r: %d)\n" ), InSrcFile.c_str(), InDstFile.c_str(), error );

		// Wait just a little bit (i.e. a totally arbitrary amount)...
		Sleep( 500 );

		// Try again
		result = MoveFileExW( InSrcFile.c_str(), InDstFile.c_str(), moveFlags );
		if ( !result )
		{
			error = GetLastError();
			Errorf( TEXT( "Error moving file '%s' to '%s' (GetLastError: %d)\n" ), InSrcFile.c_str(), InDstFile.c_str(), error );
		}
		else
		{
			Warnf( TEXT( "MoveFileExW recovered during retry!\n" ) );
		}
	}

	return result != 0 ? CMR_OK : CMR_MiscFail;
}

/*
==================
CWindowsFileSystem::IsExistFile
==================
*/
bool CWindowsFileSystem::IsExistFile( const std::wstring& InPath, bool InIsDirectory /* = false */ )
{
	DWORD		fileAttributes = GetFileAttributesW( InPath.c_str() );
	if ( fileAttributes == INVALID_FILE_ATTRIBUTES )
	{
		return false;
	}

	if ( InIsDirectory && ( fileAttributes & FILE_ATTRIBUTE_DIRECTORY ) )
	{
		return true;
	}

	return !InIsDirectory;
}

/*
==================
CWindowsFileSystem::IsDirectory
==================
*/
bool CWindowsFileSystem::IsDirectory( const std::wstring& InPath ) const
{
	DWORD		fileAttributes = GetFileAttributesW( InPath.c_str() );
	return fileAttributes != INVALID_FILE_ATTRIBUTES && fileAttributes & FILE_ATTRIBUTE_DIRECTORY;
}

/*
==================
CWindowsFileSystem::IsReadOnly
==================
*/
bool CWindowsFileSystem::IsReadOnly( const std::wstring& InPath ) const
{
	DWORD		fileAttributes = GetFileAttributesW( InPath.c_str() );
	if ( fileAttributes != INVALID_FILE_ATTRIBUTES )
	{
		return fileAttributes & FILE_ATTRIBUTE_READONLY;
	}
	else
	{
		Errorf( TEXT( "Error reading attributes for '%s'\n" ), InPath.c_str() );
		return false;
	}
}