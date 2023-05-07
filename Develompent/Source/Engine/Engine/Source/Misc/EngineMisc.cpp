#include "Misc/EngineGlobals.h"
#include "Misc/EngineMisc.h"

#if WITH_EDITOR
/*
==================
Sys_TextToESurfaceType
==================
*/
ESurfaceType Sys_TextToESurfaceType( const std::wstring& InString )
{
	for ( uint32 index = 0, count = g_SurfaceTypeNames.size(); index < count; ++index )
	{
		const std::pair< std::wstring, ESurfaceType >&		surfaceInfo = g_SurfaceTypeNames[ index ];
		if ( surfaceInfo.first == InString )
		{
			return surfaceInfo.second;
		}
	}

	return ST_Default;
}
#endif // WITH_EDITOR