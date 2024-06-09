#include "Misc/FileTools.h"

/*
==================
L_GetCurrentDirectory
==================
*/
void L_GetCurrentDirectory( std::wstring& OutDestStr, bool InIsShrinkToFit /*= true*/ )
{
	// Pre-allocate memory for current directory
	OutDestStr.resize( 1024 );

	// Try get the current directory
	while ( !L_GetCurrentDirectory( OutDestStr.data(), ( uint32 )OutDestStr.size() ) )
	{
		OutDestStr.resize( OutDestStr.size() * 2 );
	}

	// Remove all extra null terminator characters
	OutDestStr = OutDestStr.c_str();

	// Shrink to fit if it need
	if ( InIsShrinkToFit )
	{
		OutDestStr.shrink_to_fit();
	}
}

/*
==================
L_MakeAbsolutePath
==================
*/
bool L_MakeAbsolutePath( const tchar* InSrcPath, tchar* OutDestPath, uint32 InMaxLen, const tchar* InStartingDir /*= nullptr*/ )
{
	Assert( OutDestPath );
	Assert( InMaxLen >= 1 );

	// Do nothing if InSrcPath isn't valid
	if ( !InSrcPath || !InSrcPath[0] )
	{
		*OutDestPath = 0;
		return true;
	}

	// If InSrcPath isn't relative just copy it
	if ( L_IsAbsolutePath( InSrcPath ) )
	{
		// Calculate count of chars to copy
		uint32	length = L_Strlen( InSrcPath );

		// If length of source path too big do nothing
		if ( length >= InMaxLen )
		{
			*OutDestPath = 0;
			return false;
		}

		// Otherwise copy InSrcPath to OutDestPath and fix path separators
		L_Strncpy( OutDestPath, InSrcPath, length );
		OutDestPath[length] = 0;
		L_RemoveDotPathSeparators( OutDestPath );
		return true;
	}
	else
	{
		// Length of the final string
		uint32		finalLength = 0;

		// Make sure the starting directory is absolute
		if ( InStartingDir && L_IsAbsolutePath( InStartingDir ) )
		{
			// Calculate count of chars to copy
			uint32	length = L_Strlen( InStartingDir );

			// If length of InStartingDir too big do nothing
			if ( length > InMaxLen )
			{
				*OutDestPath = 0;
				return false;
			}

			// Otherwise copy InStartingDir to OutDestPath
			finalLength = length;
			L_Strncpy( OutDestPath, InStartingDir, InMaxLen );
		}
		else
		{
			// Copy to OutDestPath current directory. If L_GetCurrentDirectory return FALSE, it's mean what OutDestPath too small
			if ( !L_GetCurrentDirectory( OutDestPath, InMaxLen ) )
			{
				*OutDestPath = 0;
				return false;
			}
			finalLength = L_Strlen( OutDestPath );

			// If we have InStartingDir copy it
			if ( InStartingDir )
			{
				// If L_AppendPathSeparator return FALSE, it's mean what OutDestPath too small
				if ( !L_AppendPathSeparator( OutDestPath, InMaxLen ) )
				{
					*OutDestPath = 0;
					return false;
				}
				finalLength += 1;

				// Make sure what size of OutDestPath enough to append InStartingDir
				uint32	length = L_Strlen( InStartingDir );
				if ( length > InMaxLen - finalLength )
				{
					*OutDestPath = 0;
					return false;
				}

				finalLength += length;
				L_Strncat( OutDestPath, InStartingDir, InMaxLen, COPY_ALL_CHARACTERS );
			}
		}

		// If L_AppendPathSeparator return FALSE, it's mean what OutDestPath too small
		if ( !L_AppendPathSeparator( OutDestPath, InMaxLen ) )
		{
			*OutDestPath = 0;
			return false;
		}
		finalLength += 1;

		// Make sure what size of OutDestPath enough to append InSrcPath
		uint32	length = L_Strlen( InSrcPath );
		if ( length > InMaxLen - finalLength )
		{
			*OutDestPath = 0;
			return false;
		}

		// Copy InSrcPath and fix path separators
		L_Strncat( OutDestPath, InSrcPath, InMaxLen, COPY_ALL_CHARACTERS );
		L_RemoveDotPathSeparators( OutDestPath );
		return true;
	}
}

/*
==================
L_MakeAbsolutePath
==================
*/
void L_MakeAbsolutePath( const std::wstring& InSrcPath, std::wstring& OutDestPath, const std::wstring& InStartingDir /*= TEXT( "" )*/, bool InIsShrinkToFit /*= true*/ )
{
	// Pre-allocate memory for an absolute path
	OutDestPath.resize( 1024 );

	// Try to make an absolute path
	while ( !L_MakeAbsolutePath( InSrcPath.c_str(), OutDestPath.data(), ( uint32 )OutDestPath.size(), InStartingDir.c_str() ) )
	{
		OutDestPath.resize( OutDestPath.size() * 2 );
	}

	// Remove all extra null terminator characters
	OutDestPath = OutDestPath.c_str();

	// Shrink to fit if it need
	if ( InIsShrinkToFit )
	{
		OutDestPath.shrink_to_fit();
	}
}

/*
==================
L_MakeRelativePath
==================
*/
EMakeRelativePathResult L_MakeRelativePath( const tchar* InFullPath, const tchar* InDirPath, tchar* OutRelativePath, uint32 InMaxLen )
{
	Assert( OutRelativePath );
	Assert( InMaxLen >= 1 );
	*OutRelativePath = 0;

	// Strip out common parts of the path
	const tchar*		lastCommonPath = nullptr;
	const tchar*		lastCommonDir = nullptr;
	{
		const tchar*	curPath = InFullPath;
		const tchar*	curDir = InDirPath;
		while ( *curPath && ( L_ToLower( *curPath ) == L_ToLower( *curDir ) ||
				  ( L_IsPathSeparator( *curPath ) && ( L_IsPathSeparator( *curDir ) || ( *curDir == 0 ) ) ) ) )
		{
			if ( L_IsPathSeparator( *curPath ) )
			{
				lastCommonPath = curPath + 1;
				lastCommonDir = curDir + 1;
			}

			if ( *curDir == 0 )
			{
				--lastCommonDir;
				break;
			}

			++curDir; 
			++curPath;
		}
	}

	// Nothing in common
	if ( !lastCommonPath )
	{
		return MRPR_ErrorOnSeparateDrives;
	}

	// For each path separator remaining in the directory, need a ../
	uint32	finalLength = 0;
	bool	bLastCharWasSeparator = true;
	for ( ; *lastCommonDir; ++lastCommonDir )
	{
		if ( L_IsPathSeparator( *lastCommonDir ) )
		{
			// Make sure what size of OutRelativePath enough to append ../
			if ( InMaxLen - finalLength < 3 )
			{
				*OutRelativePath = 0;
				return MRPR_ErrorDestTooSmall;
			}

			OutRelativePath[finalLength++] = TEXT( '.' );
			OutRelativePath[finalLength++] = TEXT( '.' );
			OutRelativePath[finalLength++] = PATH_SEPARATOR[0];
			bLastCharWasSeparator = true;
		}
		else
		{
			bLastCharWasSeparator = false;
		}
	}

	// Deal with relative paths not specified with a trailing slash
	if ( !bLastCharWasSeparator )
	{
		// Make sure what size of OutRelativePath enough to append ../
		if ( InMaxLen - finalLength < 3 )
		{
			*OutRelativePath = 0;
			return MRPR_ErrorDestTooSmall;
		}

		OutRelativePath[finalLength++] = TEXT( '.' );
		OutRelativePath[finalLength++] = TEXT( '.' );
		OutRelativePath[finalLength++] = PATH_SEPARATOR[0];
	}

	// Copy the remaining part of the relative path over, fixing the path separators
	for ( ; *lastCommonPath; ++lastCommonPath )
	{
		// Make sure what size of OutRelativePath enough to append a new char
		if ( InMaxLen - finalLength < 1 )
		{
			*OutRelativePath = 0;
			return MRPR_ErrorDestTooSmall;
		}

		if ( L_IsPathSeparator( *lastCommonPath ) )
		{
			OutRelativePath[finalLength++] = PATH_SEPARATOR[0];
		}
		else
		{
			OutRelativePath[finalLength++] = *lastCommonPath;
		}
	}

	// Make sure what size of OutRelativePath enough to append a null terminator
	if ( InMaxLen - finalLength < 1 )
	{
		*OutRelativePath = 0;
		return MRPR_ErrorDestTooSmall;
	}

	OutRelativePath[finalLength] = 0;
	return MRPR_Ok;
}

/*
==================
L_MakeRelativePath
==================
*/
bool L_MakeRelativePath( const std::wstring& InFullPath, const std::wstring& InDirPath, std::wstring& OutRelativePath, bool InIsShrinkToFit /*= true*/ )
{
	// Pre-allocate memory for an relative path
	OutRelativePath.resize( 1024 );

	// Try to make an relative path
	EMakeRelativePathResult		result = MRPR_ErrorDestTooSmall;
	while ( result == MRPR_ErrorDestTooSmall )
	{
		result = L_MakeRelativePath( InFullPath.c_str(), InDirPath.c_str(), OutRelativePath.data(), ( uint32 )OutRelativePath.size() );
		if ( result == MRPR_ErrorDestTooSmall )
		{
			OutRelativePath.resize( OutRelativePath.size() * 2 );
		}
	}

	// Remove all extra null terminator characters
	if ( result == MRPR_Ok )
	{
		OutRelativePath = OutRelativePath.c_str();

		// Shrink to fit if it need
		if ( InIsShrinkToFit )
		{
			OutRelativePath.shrink_to_fit();
		}
	}

	return result == MRPR_Ok;
}

/*
==================
L_AppendPathSeparator
==================
*/
bool L_AppendPathSeparator( tchar* InOutStr, uint32 InStrSize )
{
	// Do nothing if pStr isn't valid
	if ( !InOutStr || !InOutStr[0] )
	{
		return false;
	}

	// Append a path separator
	uint32		length = L_Strlen( InOutStr );
	Assert( length > 0 );
	if ( !L_IsPathSeparator( InOutStr[length - 1] ) )
	{
		if ( length + 1 >= InStrSize )
		{
			return false;
		}

		InOutStr[length] = PATH_SEPARATOR[0];
		InOutStr[length+1] = 0;
	}

	return true;
}

/*
==================
L_AppendPathSeparator
==================
*/
void L_AppendPathSeparator( std::wstring& InStr )
{
	if ( InStr.empty() || !L_IsPathSeparator( InStr[InStr.size() - 1] ) )
	{
		InStr += PATH_SEPARATOR;
	}
}

/*
==================
L_FixPathSeparators
==================
*/
void L_FixPathSeparators( tchar* InOutPath )
{
	while ( *InOutPath )
	{
		if ( L_IsPathSeparator( *InOutPath ) )
		{
			*InOutPath = PATH_SEPARATOR[0];
		}
		++InOutPath;
	}
}

/*
==================
L_GetFileExtension
==================
*/
const tchar* L_GetFileExtension( const tchar* InPath, bool InIsIncludeDot /*= false*/ )
{
	// By default we at end of the path
	const tchar*	result = InPath + L_Strlen( InPath ) - 1;

	// Back up until a . or the start of path
	while ( result != InPath && *( result - 1 ) != TEXT( '.' ) )
	{
		--result;
	}

	// Check to see if the '.' is part of a path name
	if ( result == InPath || L_IsPathSeparator( *result ) )
	{
		// If true it's mean what we no have extension
		return nullptr;
	}

	// Step back if we need include a dot
	if ( InIsIncludeDot )
	{
		--result;
	}

	// Otherwise we return our extension
	return result;
}

/*
==================
L_GetFileBaseName
==================
*/
bool L_GetFileBaseName( const tchar* InPath, tchar* OutFileName, uint32 InMaxLen )
{
	Assert( OutFileName );
	Assert( InMaxLen >= 1 );

	// Do nothing if InPath isn't valid
	if ( !InPath || !InPath[0] )
	{
		*OutFileName = 0;
		return true;
	}

	// Get string length
	uint32	length = L_Strlen( InPath );
	int32	startId = 0;
	int32	endId = 0;

	// Scan backward for '.'
	endId = length ? length - 1 : 0;
	while ( endId && InPath[endId] != TEXT( '.' ) && !L_IsPathSeparator( InPath[endId] ) )
	{
		--endId;
	}

	// We no have '.' then copy to the end
	if ( InPath[endId] != TEXT( '.' ) )
	{
		endId = length ? length - 1 : 0;
	}
	// Otherwise we found '.' then copy to left of '.'
	else
	{
		--endId;
	}

	// Scan backward for path separator
	startId = length ? length - 1 : 0;
	while ( startId >= 0 && !L_IsPathSeparator( InPath[startId] ) )
	{
		--startId;
	}

	// We no have a path separator then copy from the start
	if ( startId < 0 || !L_IsPathSeparator( InPath[startId] ) )
	{
		startId = 0;
	}
	// Otherwise we found a path separator then copy for right of the one
	else
	{
		++startId;
	}

	// Calculate length of a new string
	length = endId - startId + 1;

	// If length of a new string too big do nothing
	if ( length >= InMaxLen )
	{
		*OutFileName = 0;
		return false;
	}

	// Copy partial string
	uint32	maxCharsToCopy = Min<uint32>( length, InMaxLen );
	L_Strncpy( OutFileName, &InPath[startId], maxCharsToCopy );
	OutFileName[length] = 0;
	return true;
}

/*
==================
L_GetFileBaseName
==================
*/
void L_GetFileBaseName( const std::wstring& InPath, std::wstring& OutFileName, bool InIsShrinkToFit /*= true*/ )
{
	// Pre-allocate memory for filename
	OutFileName.resize( 1024 );

	// Try copy the base file name
	while ( !L_GetFileBaseName( InPath.c_str(), OutFileName.data(), ( uint32 )OutFileName.size() ) )
	{
		OutFileName.resize( OutFileName.size() * 2 );
	}

	// Remove all extra null terminator characters
	OutFileName = OutFileName.c_str();

	// Shrink to fit if it need
	if ( InIsShrinkToFit )
	{
		OutFileName.shrink_to_fit();
	}
}

/*
==================
L_GetFileName
==================
*/
const tchar* L_GetFileName( const tchar* InPath )
{
	// Back up until the character after the first path separator we find,
	// or the beginning of the string
	const tchar*	result = InPath + L_Strlen( InPath ) - 1;
	while ( result > InPath && !L_IsPathSeparator( *( result -1 ) ) )
	{
		--result;
	}
	return result;
}

/*
==================
L_GetFilePath
==================
*/
bool L_GetFilePath( const tchar* InSrcPath, tchar* OutDestPath, uint32 InMaxLen )
{
	Assert( OutDestPath );
	Assert( InMaxLen >= 1 );

	// Do nothing if InSrcPath isn't valid
	if ( !InSrcPath || !InSrcPath[0] )
	{
		*OutDestPath = 0;
		return true;
	}

	// Get string length
	uint32			length = L_Strlen( InSrcPath );
	const tchar*	src = InSrcPath + ( length ? length-1 : 0 );

	// Back up until a path separator or the start
	while ( src != InSrcPath && !L_IsPathSeparator( *( src - 1 ) ) )
	{
		--src;
	}

	// Calculate length of a new string
	length = ( uint32 )( src - InSrcPath );

	// If length of a new string too big do nothing
	if ( length >= InMaxLen )
	{
		*OutDestPath = 0;
		return false;
	}

	// Copy partial string
	uint32	maxCharsToCopy = Min<uint32>( length, InMaxLen );
	L_Strncpy( OutDestPath, InSrcPath, maxCharsToCopy );
	OutDestPath[length] = 0;
	return true;
}

/*
==================
L_GetFilePath
==================
*/
void L_GetFilePath( const std::wstring& InSrcPath, std::wstring& OutDestPath, bool InIsShrinkToFit /*= true*/ )
{
	// Pre-allocate memory for a new string
	OutDestPath.resize( 1024 );

	// Try copy the path
	while ( !L_GetFilePath( InSrcPath.c_str(), OutDestPath.data(), ( uint32 )OutDestPath.size() ) )
	{
		OutDestPath.resize( OutDestPath.size() * 2 );
	}

	// Remove all extra null terminator characters
	OutDestPath = OutDestPath.c_str();

	// Shrink to fit if it need
	if ( InIsShrinkToFit )
	{
		OutDestPath.shrink_to_fit();
	}
}

/*
==================
L_RemoveDotPathSeparators
==================
*/
void L_RemoveDotPathSeparators( tchar* InOutPath, bool InIsRemoveDoubleSeparators /*= true*/ )
{
	// Read and write position in the string
	tchar*	read = InOutPath;
	tchar*	write = InOutPath;
	bool	bBoundary = true;

	// Remove all dot path separators ("./", "../")
	while ( *read )
	{
		// Get rid of /../ or trailing /.. by backing write up to previous separator
		if ( bBoundary && read[0] == TEXT( '.' ) && read[1] == TEXT( '.' ) && ( L_IsPathSeparator( read[2] ) || !read[2] ) )
		{
			// Eat the last separator (or repeated separators) we wrote out
			while ( write != InOutPath && write[-1] == PATH_SEPARATOR[0] )
			{
				--write;
			}

			while ( true )
			{
				if ( write == InOutPath )
				{
					break;
				}
				--write;
				if ( *write == PATH_SEPARATOR[0] )
				{
					break;
				}
			}

			// Skip the '..' but not the slash, next loop iteration will handle separator
			read		+= 2;
			bBoundary	= ( write == InOutPath );
		}
		// Handle "./" by simply skipping this sequence. bBoundary is unchanged
		else if ( bBoundary && read[0] == TEXT( '.' ) && ( L_IsPathSeparator( read[1] ) || !read[1] ) )
		{
			if ( L_IsPathSeparator( read[1] ) )
			{
				read += 2;
			}
			else
			{
				// Special case: if trailing "." is preceded by separator, eg "path/.",
				// then the final separator should also be stripped. bBoundary may then
				// be in an incorrect state, but we are at the end of processing anyway
				// so we don't really care (the processing loop is about to terminate)
				if ( write != InOutPath && write[-1] == PATH_SEPARATOR[0] )
				{
					--write;
				}
				read += 1;
			}
		}
		// Handle "/"
		else if ( L_IsPathSeparator( read[0] ) )
		{
			*write		= PATH_SEPARATOR[0];
			write		+= 1 - ( bBoundary & InIsRemoveDoubleSeparators & ( write != InOutPath ) );
			read		+= 1;
			bBoundary	= true;
		}
		// Otherwise iterate over the path
		else
		{
			if ( write != read )
			{
				*write = *read;
			}
			write		+= 1;
			read		+= 1;
			bBoundary	= false;
		}
	}

	// We are done!
	*write = 0;
}


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
	L_FixPathSeparators( path );
}

/*
==================
CFilename::GetLocalizedFilename
==================
*/
std::wstring CFilename::GetLocalizedFileName( const std::wstring& InLanguage /* = TEXT( "" ) */ ) const
{
	// Prepend path and path separator
	std::wstring	localizedPath;
	L_GetFilePath( path, localizedPath, false );
	if ( !localizedPath.empty() )
	{
		L_AppendPathSeparator( localizedPath );
	}

	// Append _LANG to filename
	localizedPath += GetBaseFileName() + TEXT( "_" ) + ( InLanguage.empty() ? TEXT( "INT" ) : InLanguage );

	// Append extension if used
	std::wstring	extension = GetExtension( true );
	if ( !extension.empty() )
	{
		localizedPath += extension;
	}

	return localizedPath;
}