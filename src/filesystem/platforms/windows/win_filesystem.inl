/**
 * @file
 * @addtogroup filesystem filesystem
 *
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

#ifndef WIN_FILESYSTEM_INL
#define WIN_FILESYSTEM_INL

/*
==================
Plat_MakeDirectory
==================
*/
FORCEINLINE bool Plat_MakeDirectory( const achar* pPath )
{ 
	return CreateDirectoryA( pPath, nullptr ) != 0 || GetLastError() == ERROR_ALREADY_EXISTS;
}

/*
==================
Plat_DeleteDirectory
==================
*/
FORCEINLINE bool Plat_DeleteDirectory( const achar* pPath, bool bEvenReadOnly /*= false*/ )
{
	return RemoveDirectoryA( pPath );
}

/*
==================
Plat_IsFileExists
==================
*/
FORCEINLINE bool Plat_IsFileExists( const achar* pPath )
{
	// Get file attributes at the path
	DWORD	fileAttributes = GetFileAttributesA( pPath );
	
	// If it was not possible to get the file attributes, then the file does not exist
	if ( fileAttributes == INVALID_FILE_ATTRIBUTES )
	{
		return false;
	}

	// Otherwise all ok
	return true;
}

/*
==================
Plat_IsFileDirectory
==================
*/
FORCEINLINE bool Plat_IsFileDirectory( const achar* pPath )
{
	DWORD	fileAttributes = GetFileAttributesA( pPath );
	return fileAttributes != INVALID_FILE_ATTRIBUTES && fileAttributes & FILE_ATTRIBUTE_DIRECTORY;
}

/*
==================
Plat_MoveFile
==================
*/
FORCEINLINE ECopyMoveResult Plat_MoveFile( const achar* pSrcPath, const achar* pDestPath, bool bReplaceExisting /*= false*/, bool bEvenReadOnly /*= false*/ )
{
	DWORD	moveFlags = ( bReplaceExisting ? MOVEFILE_REPLACE_EXISTING : 0x0 ) | MOVEFILE_WRITE_THROUGH;
	return MoveFileExA( pSrcPath, pDestPath, moveFlags ) != 0 ? COPYMOVE_RESULT_OK : COPYMOVE_RESULT_MISC_FAIL;
}

#endif // !WIN_FILESYSTEM_INL