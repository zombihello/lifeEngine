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

#include "pch_stdlib.h"
#include "core/debug.h"
#include "stdlib/filetools.h"

/*
==================
L_GetCurrentDirectory
==================
*/
void L_GetCurrentDirectory( std::string& destStr, bool bShrinkToFit /*= true*/ )
{
	// Pre-allocate memory for current directory
	destStr.resize( 1024 );

	// Try get the current directory
	while ( !L_GetCurrentDirectory( destStr.data(), ( uint32 )destStr.size() ) )
	{
		destStr.resize( destStr.size() * 2 );
	}

	// Remove all extra null terminator characters (if it need)
	if ( bShrinkToFit )
	{
		destStr = destStr.c_str();
		destStr.shrink_to_fit();
	}
}

/*
==================
L_MakeAbsolutePath
==================
*/
bool L_MakeAbsolutePath( const achar* pSrcPath, achar* pDestPath, uint32 maxLen, const achar* pStartingDir /*= nullptr*/ )
{
	Assert( pDestPath );
	Assert( maxLen >= 1 );

	// Do nothing if pSrcPath isn't valid
	if ( !pSrcPath || !pSrcPath[0] )
	{
		*pDestPath = 0;
		return true;
	}

	// If pSrcPath isn't relative just copy it
	if ( L_IsAbsolutePath( pSrcPath ) )
	{
		// Calculate count of chars to copy
		uint32	length = L_Strlen( pSrcPath );

		// If length of source path too big do nothing
		if ( length > maxLen )
		{
			*pDestPath = 0;
			return false;
		}

		// Otherwise copy pSrcPath to pDestPath and fix path separators
		L_Strncpy( pDestPath, pSrcPath, length );
		L_RemoveDotPathSeparators( pDestPath );
		return true;
	}
	else
	{
		// Length of the final string
		uint32		finalLength = 0;

		// Make sure the starting directory is absolute
		if ( pStartingDir && L_IsAbsolutePath( pStartingDir ) )
		{
			// Calculate count of chars to copy
			uint32	length = L_Strlen( pStartingDir );

			// If length of pStartingDir too big do nothing
			if ( length > maxLen )
			{
				*pDestPath = 0;
				return false;
			}

			// Otherwise copy pStartingDir to pDestPath
			finalLength = length;
			L_Strncpy( pDestPath, pStartingDir, maxLen );
		}
		else
		{
			// Copy to pDestPath current directory. If L_GetCurrentDirectory return FALSE, it's mean what pDestPath too small
			if ( !L_GetCurrentDirectory( pDestPath, maxLen ) )
			{
				*pDestPath = 0;
				return false;
			}
			finalLength = L_Strlen( pDestPath );

			// If we have pStartingDir copy it
			if ( pStartingDir )
			{
				// If L_AppendPathSeparator return FALSE, it's mean what pDestPath too small
				if ( !L_AppendPathSeparator( pDestPath, maxLen ) )
				{
					*pDestPath = 0;
					return false;
				}
				finalLength += 1;

				// Make sure what size of pDestPath enough to append pStartingDir
				uint32	length = L_Strlen( pStartingDir );	
				if ( length > maxLen - finalLength )
				{
					*pDestPath = 0;
					return false;
				}

				finalLength += length;
				L_Strncat( pDestPath, pStartingDir, maxLen, COPY_ALL_CHARACTERS );
			}
		}

		// If L_AppendPathSeparator return FALSE, it's mean what pDestPath too small
		if ( !L_AppendPathSeparator( pDestPath, maxLen ) )
		{
			*pDestPath = 0;
			return false;
		}
		finalLength += 1;

		// Make sure what size of pDestPath enough to append pSrcPath
		uint32	length = L_Strlen( pSrcPath );
		if ( length > maxLen - finalLength )
		{
			*pDestPath = 0;
			return false;
		}

		// Copy pSrcPath and fix path separators
		L_Strncat( pDestPath, pSrcPath, maxLen, COPY_ALL_CHARACTERS );
		L_RemoveDotPathSeparators( pDestPath );
		return true;
	}
}

/*
==================
L_MakeAbsolutePath
==================
*/
void L_MakeAbsolutePath( const std::string& srcPath, std::string& destPath, const std::string& startingDir /*= ""*/, bool bShrinkToFit /*= true*/ )
{
	// Pre-allocate memory for an absolute path
	destPath.resize( 1024 );

	// Try to make an absolute path
	while ( !L_MakeAbsolutePath( srcPath.c_str(), destPath.data(), ( uint32 )destPath.size(), startingDir.c_str() ) )
	{
		destPath.resize( destPath.size() * 2 );
	}

	// Remove all extra null terminator characters (if it need)
	if ( bShrinkToFit )
	{
		destPath = destPath.c_str();
		destPath.shrink_to_fit();
	}
}

/*
==================
L_MakeRelativePath
==================
*/
int32 L_MakeRelativePath( const achar* pFullPath, const achar* pDirPath, achar* pRelativePath, uint32 maxLen )
{
	enum
	{
		RESULT_OK						= 0,
		RESULT_ERR_DEST_TOO_SMALL		= 1,
		RESULT_ERR_ON_SEPARATE_DRIVES	= 2
	};

	Assert( pRelativePath );
	Assert( maxLen >= 1 );
	*pRelativePath = 0;

	// Strip out common parts of the path
	const achar*	pLastCommonPath = nullptr;
	const achar*	pLastCommonDir = nullptr;
	{
		const achar*	pCurPath = pFullPath;
		const achar*	pCurDir = pDirPath;
		while ( *pCurPath && ( L_ToLower( *pCurPath ) == L_ToLower( *pCurDir ) ||
				  ( L_IsPathSeparator( *pCurPath ) && ( L_IsPathSeparator( *pCurDir ) || ( *pCurDir == 0 ) ) ) ) )
		{
			if ( L_IsPathSeparator( *pCurPath ) )
			{
				pLastCommonPath = pCurPath + 1;
				pLastCommonDir = pCurDir + 1;
			}

			if ( *pCurDir == 0 )
			{
				--pLastCommonDir;
				break;
			}

			++pCurDir; 
			++pCurPath;
		}
	}

	// Nothing in common
	if ( !pLastCommonPath )
	{
		return RESULT_ERR_ON_SEPARATE_DRIVES;
	}

	// For each path separator remaining in the directory, need a ../
	uint32	finalLength = 0;
	bool	bLastCharWasSeparator = true;
	for ( ; *pLastCommonDir; ++pLastCommonDir )
	{
		if ( L_IsPathSeparator( *pLastCommonDir ) )
		{
			// Make sure what size of pRelativePath enough to append ../
			if ( maxLen - finalLength < 3 )
			{
				*pRelativePath = 0;
				return RESULT_ERR_DEST_TOO_SMALL;
			}

			pRelativePath[finalLength++] = '.';
			pRelativePath[finalLength++] = '.';
			pRelativePath[finalLength++] = PATH_SEPARATOR;
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
		// Make sure what size of pRelativePath enough to append ../
		if ( maxLen - finalLength < 3 )
		{
			*pRelativePath = 0;
			return RESULT_ERR_DEST_TOO_SMALL;
		}

		pRelativePath[finalLength++] = '.';
		pRelativePath[finalLength++] = '.';
		pRelativePath[finalLength++] = PATH_SEPARATOR;
	}

	// Copy the remaining part of the relative path over, fixing the path separators
	for ( ; *pLastCommonPath; ++pLastCommonPath )
	{
		// Make sure what size of pRelativePath enough to append a new char
		if ( maxLen - finalLength < 1 )
		{
			*pRelativePath = 0;
			return RESULT_ERR_DEST_TOO_SMALL;
		}

		if ( L_IsPathSeparator( *pLastCommonPath ) )
		{
			pRelativePath[finalLength++] = PATH_SEPARATOR;
		}
		else
		{
			pRelativePath[finalLength++] = *pLastCommonPath;
		}
	}

	// Make sure what size of pRelativePath enough to append a null terminator
	if ( maxLen - finalLength < 1 )
	{
		*pRelativePath = 0;
		return RESULT_ERR_DEST_TOO_SMALL;
	}

	pRelativePath[finalLength] = 0;
	return RESULT_OK;
}

/*
==================
L_MakeRelativePath
==================
*/
bool L_MakeRelativePath( const std::string& fullPath, const std::string& dirPath, std::string& relativePath, bool bShrinkToFit /*= true*/ )
{
	// Pre-allocate memory for an relative path
	relativePath.resize( 1024 );

	// Try to make an relative path
	int32	result = 1;
	while ( result == 1 )
	{
		result = L_MakeRelativePath( fullPath.c_str(), dirPath.c_str(), relativePath.data(), ( uint32 )relativePath.size() );
		if ( result == 1 )
		{
			relativePath.resize( relativePath.size() * 2 );
		}
	}

	// Remove all extra null terminator characters (if it need)
	if ( result == 0 && bShrinkToFit )
	{
		relativePath = relativePath.c_str();
		relativePath.shrink_to_fit();
	}

	return result == 0;
}

/*
==================
L_AppendPathSeparator
==================
*/
bool L_AppendPathSeparator( achar* pStr, uint32 strSize )
{
	// Do nothing if pStr isn't valid
	if ( !pStr || !pStr[0] )
	{
		return false;
	}

	// Append a path separator
	uint32		length = L_Strlen( pStr );
	Assert( length > 0 );
	if ( !L_IsPathSeparator( pStr[length - 1] ) )
	{
		if ( length + 1 >= strSize )
		{
			return false;
		}

		pStr[length] = PATH_SEPARATOR;
		pStr[length+1] = 0;
	}

	return true;
}

/*
==================
L_AppendPathSeparator
==================
*/
void L_AppendPathSeparator( std::string& str )
{
	if ( str.empty() || !L_IsPathSeparator( str[str.size() - 1] ) )
	{
		str += PATH_SEPARATOR;
	}
}

/*
==================
L_FixPathSeparators
==================
*/
void L_FixPathSeparators( achar* pPath )
{
	while ( *pPath )
	{
		if ( L_IsPathSeparator( *pPath ) )
		{
			*pPath = PATH_SEPARATOR;
		}
		++pPath;
	}
}

/*
==================
L_GetFileExtension
==================
*/
const achar* L_GetFileExtension( const achar* pPath, bool bIncludeDot /*= false*/ )
{
	// By default we at end of the path
	const achar*	pResult = pPath + L_Strlen( pPath ) - 1;

	// Back up until a . or the start of path
	while ( pResult != pPath && *( pResult - 1 ) != '.' )
	{
		--pResult;
	}

	// Check to see if the '.' is part of a path name
	if ( pResult == pPath || L_IsPathSeparator( *pResult ) )
	{
		// If true it's mean what we no have extension
		return nullptr;
	}

	// Step back if we need include a dot
	if ( bIncludeDot )
	{
		--pResult;
	}

	// Otherwise we return our extension
	return pResult;
}

/*
==================
L_GetFileBaseName
==================
*/
bool L_GetFileBaseName( const achar* pPath, achar* pFileName, uint32 maxLen )
{
	Assert( pFileName );
	Assert( maxLen >= 1 );

	// Do nothing if pPath isn't valid
	if ( !pPath || !pPath[0] )
	{
		*pFileName = 0;
		return true;
	}

	// Get string length
	uint32	length = L_Strlen( pPath );
	int32	startId = 0;
	int32	endId = 0;

	// Scan backward for '.'
	endId = length ? length - 1 : 0;
	while ( endId && pPath[endId] != '.' && !L_IsPathSeparator( pPath[endId] ) )
	{
		--endId;
	}

	// We no have '.' then copy to the end
	if ( pPath[endId] != '.' )
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
	while ( startId >= 0 && !L_IsPathSeparator( pPath[startId] ) )
	{
		--startId;
	}

	// We no have a path separator then copy from the start
	if ( startId < 0 || !L_IsPathSeparator( pPath[startId] ) )
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
	if ( length > maxLen )
	{
		*pFileName = 0;
		return false;
	}

	// Copy partial string
	uint32	maxCharsToCopy = Min<uint32>( length, maxLen );
	L_Strncpy( pFileName, &pPath[startId], maxCharsToCopy );
	return true;
}

/*
==================
L_GetFileBaseName
==================
*/
void L_GetFileBaseName( const std::string& path, std::string& fileName, bool bShrinkToFit /*= true*/ )
{
	// Pre-allocate memory for filename
	fileName.resize( 1024 );

	// Try copy the base file name
	while ( !L_GetFileBaseName( path.c_str(), fileName.data(), ( uint32 )fileName.size() ) )
	{
		fileName.resize( fileName.size() * 2 );
	}

	// Remove all extra null terminator characters (if it need)
	if ( bShrinkToFit )
	{
		fileName = fileName.c_str();
		fileName.shrink_to_fit();
	}
}

/*
==================
L_GetFileName
==================
*/
const achar* L_GetFileName( const achar* pPath )
{
	// Back up until the character after the first path separator we find,
	// or the beginning of the string
	const achar*	pResult = pPath + L_Strlen( pPath ) - 1;
	while ( pResult > pPath && !L_IsPathSeparator( *( pResult-1 ) ) )
	{
		--pResult;
	}
	return pResult;
}

/*
==================
L_GetFilePath
==================
*/
bool L_GetFilePath( const achar* pSrcPath, achar* pDestPath, uint32 maxLen )
{
	Assert( pDestPath );
	Assert( maxLen >= 1 );

	// Do nothing if pSrcPath isn't valid
	if ( !pSrcPath || !pSrcPath[0] )
	{
		*pDestPath = 0;
		return true;
	}

	// Get string length
	uint32			length = L_Strlen( pSrcPath );
	const achar*	pSrc = pSrcPath + ( length ? length-1 : 0 );

	// Back up until a path separator or the start
	while ( pSrc != pSrcPath && !L_IsPathSeparator( *( pSrc - 1 ) ) )
	{
		--pSrc;
	}

	// Calculate length of a new string
	length = ( uint32 )( pSrc - pSrcPath );

	// If length of a new string too big do nothing
	if ( length > maxLen )
	{
		*pDestPath = 0;
		return false;
	}

	// Copy partial string
	uint32	maxCharsToCopy = Min<uint32>( length, maxLen );
	L_Strncpy( pDestPath, pSrcPath, maxCharsToCopy );
	return true;
}

/*
==================
L_GetFilePath
==================
*/
void L_GetFilePath( const std::string& srcPath, std::string& destPath, bool bShrinkToFit /*= true*/ )
{
	// Pre-allocate memory for a new string
	destPath.resize( 1024 );

	// Try copy the path
	while ( !L_GetFilePath( srcPath.c_str(), destPath.data(), ( uint32 )destPath.size() ) )
	{
		destPath.resize( destPath.size() * 2 );
	}

	// Remove all extra null terminator characters (if it need)
	if ( bShrinkToFit )
	{
		destPath = destPath.c_str();
		destPath.shrink_to_fit();
	}
}

/*
==================
L_RemoveDotPathSeparators
==================
*/
void L_RemoveDotPathSeparators( achar* pPath, bool bRemoveDoubleSeparators /*= true*/ )
{
	// Read and write position in the string
	achar*		pRead = pPath;
	achar*		pWrite = pPath;
	bool		bBoundary = true;

	// Remove all dot path separators ("./", "../")
	while ( *pRead )
	{
		// Get rid of /../ or trailing /.. by backing pWrite up to previous separator
		if ( bBoundary && pRead[0] == '.' && pRead[1] == '.' && ( L_IsPathSeparator( pRead[2] ) || !pRead[2] ) )
		{
			// Eat the last separator (or repeated separators) we wrote out
			while ( pWrite != pPath && pWrite[-1] == PATH_SEPARATOR )
			{
				--pWrite;
			}

			while ( true )
			{
				if ( pWrite == pPath )
				{
					break;
				}
				--pWrite;
				if ( *pWrite == PATH_SEPARATOR )
				{
					break;
				}
			}

			// Skip the '..' but not the slash, next loop iteration will handle separator
			pRead		+= 2;
			bBoundary	= ( pWrite == pPath );
		}
		// Handle "./" by simply skipping this sequence. bBoundary is unchanged
		else if ( bBoundary && pRead[0] == '.' && ( L_IsPathSeparator( pRead[1] ) || !pRead[1] ) )
		{
			if ( L_IsPathSeparator( pRead[1] ) )
			{
				pRead += 2;
			}
			else
			{
				// Special case: if trailing "." is preceded by separator, eg "path/.",
				// then the final separator should also be stripped. bBoundary may then
				// be in an incorrect state, but we are at the end of processing anyway
				// so we don't really care (the processing loop is about to terminate)
				if ( pWrite != pPath && pWrite[-1] == PATH_SEPARATOR )
				{
					--pWrite;
				}
				pRead += 1;
			}
		}
		// Handle "/"
		else if ( L_IsPathSeparator( pRead[0] ) )
		{
			*pWrite		= PATH_SEPARATOR;
			pWrite		+= 1 - ( bBoundary & bRemoveDoubleSeparators & ( pWrite != pPath ) );
			pRead		+= 1;
			bBoundary	= true;
		}
		// Otherwise iterate over the path
		else
		{
			if ( pWrite != pRead )
			{
				*pWrite = *pRead;
			}
			pWrite		+= 1;
			pRead		+= 1;
			bBoundary	= false;
		}
	}

	// We are done!
	*pWrite = 0;
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
CFilename::CFilename( const std::string& path )
	: path( path )
{
	L_FixPathSeparators( CFilename::path );
}