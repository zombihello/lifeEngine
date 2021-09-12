#include "Windows/WindowsManager.h"

void FWindowsManager::Remove( FWBaseWindowParamRef InWindow )
{
	for ( uint32 index = 0, count = ( uint32 )windows.size(); index < count; ++index )
	{
		WBaseWindow*		window = windows[ index ];
		if ( window == InWindow )
		{
			windows.erase( windows.begin() + index );
			return;
		}
	}
}

void FWindowsManager::Tick()
{
	for ( uint32 index = 0, count = ( uint32 )windows.size(); index < count; ++index )
	{
		windows[ index ]->Tick();
	}
}