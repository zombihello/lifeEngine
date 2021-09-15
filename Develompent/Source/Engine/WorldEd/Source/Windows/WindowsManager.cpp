#include "Windows/WindowsManager.h"

void FWindowsManager::Remove( FWBaseWindowParamRef InWindow )
{
	for ( uint32 index = 0, count = ( uint32 )windows.size(); index < count; ++index )
	{
		FWindowInfo&		windowInfo = windows[ index ];
		if ( windowInfo.window == InWindow )
		{
			windowInfo.isNeedRemove = true;
			return;
		}
	}
}

void FWindowsManager::Tick()
{
	for ( uint32 index = 0; index < ( uint32 )windows.size(); )
	{
		const FWindowInfo&		windowInfo = windows[ index ];
		if ( windowInfo.isNeedRemove )
		{
			windows.erase( windows.begin() + index );
			--index;
			continue;
		}

		windowInfo.window->Tick();
		++index;
	}
}