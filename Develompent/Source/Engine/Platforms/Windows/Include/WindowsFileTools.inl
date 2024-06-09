/**
 * @file
 * @addtogroup Core Core
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef WINDOWSFILETOOLS_INL
#define WINDOWSFILETOOLS_INL

/*
==================
L_SetCurrentDirectory
==================
*/
FORCEINLINE bool L_SetCurrentDirectory( const tchar* InDirName )
{ 
	return _wchdir( InDirName ) == 0;
}

/*
==================
L_GetCurrentDirectory
==================
*/
FORCEINLINE bool L_GetCurrentDirectory( tchar* OutDestStr, uint32 InMaxLen )
{
	Assert( InMaxLen >= 1 );
	Assert( OutDestStr );
	if ( !OutDestStr || InMaxLen < 1 )
	{
		return false;
	}

	return _wgetcwd( OutDestStr, InMaxLen ) == OutDestStr;
}

/*
==================
L_IsAbsolutePath
==================
*/
FORCEINLINE bool L_IsAbsolutePath( const tchar* InPath )
{
	return ( InPath[0] && InPath[1] == TEXT( ':' ) ) || InPath[0] == TEXT( '/' ) || InPath[0] == TEXT( '\\' );
}

#endif // !WINDOWSFILETOOLS_INL