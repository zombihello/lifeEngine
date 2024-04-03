/**
 * @file
 * @addtogroup Core Core
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef WINDOWSSTRINGTOOLS_H
#define WINDOWSSTRINGTOOLS_H

/*
==================
L_Stricmp
==================
*/
FORCEINLINE uint32 L_Stricmp( const achar* InString1, const achar* InString2 ) 
{ 
	return stricmp( InString1, InString2 );
}

/*
==================
L_Stricmp
==================
*/
FORCEINLINE uint32 L_Stricmp( const tchar* InString1, const tchar* InString2 )
{ 
	return wcsicmp( InString1, InString2 ); 
}

/*
==================
L_Strnicmp
==================
*/
FORCEINLINE uint32 L_Strnicmp( const achar* InString1, const achar* InString2, uint32 InCount ) 
{ 
	return strnicmp( InString1, InString2, InCount ); 
}

/*
==================
L_Strnicmp
==================
*/
FORCEINLINE uint32 L_Strnicmp( const tchar* InString1, const tchar* InString2, uint32 InCount )
{
	return wcsnicmp( InString1, InString2, InCount );
}

#endif // !WINDOWSSTRINGTOOLS_H