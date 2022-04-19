#include "Misc/EngineGlobals.h"
#include "Misc/EngineMisc.h"

#if WITH_EDITOR
ESurfaceType appTextToESurfaceType( const std::wstring& InString )
{
	for ( uint32 index = 0, count = GSurfaceTypeNames.size(); index < count; ++index )
	{
		const std::pair< std::wstring, ESurfaceType >&		surfaceInfo = GSurfaceTypeNames[ index ];
		if ( surfaceInfo.first == InString )
		{
			return surfaceInfo.second;
		}
	}

	return ST_Default;
}
#endif // WITH_EDITOR