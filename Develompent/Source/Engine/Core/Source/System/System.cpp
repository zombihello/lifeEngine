#include "Logger/LoggerMacros.h"
#include "Misc/CoreGlobals.h"
#include "Misc/Misc.h"
#include "System/System.h"
#include "System/Config.h"
#include "System/PackageFileCache.h"

/*
==================
CSystem::Init
==================
*/
void CSystem::Init()
{
	// Get from config all package extensions
	Assert( packageExtensions.empty() );
	std::vector<CConfigValue>	configPackageExtensions = g_Config.GetValue( CT_Engine, TEXT( "Core.System" ), TEXT( "PackageExtensions" ) ).GetArray();
	for ( uint32 index = 0, count = configPackageExtensions.size(); index < count; ++index )
	{
		const CConfigValue&		configPackageExtension = configPackageExtensions[index];
		if ( configPackageExtension.IsA( CConfigValue::T_String ) )
		{
			packageExtensions.push_back( configPackageExtension.GetString() );
		}
	}

	if ( packageExtensions.empty() )
	{
		Warnf( TEXT( "In 'Engine' config the section 'Core.System:PackageExtensions' is empty or not correct\n" ) );
	}

	// Get from config all directories with packages
	Assert( packagePaths.empty() );
	std::vector<CConfigValue>	configPackagePaths = g_Config.GetValue( CT_Engine, TEXT( "Core.System" ), TEXT( "PackagePaths" ) ).GetArray();
	for ( uint32 index = 0, count = configPackagePaths.size(); index < count; ++index )
	{
		std::wstring	path = configPackagePaths[index].GetString();
		if ( !path.empty() )
		{
			// If we have in the path placeholder %Game% then we replace it to g_GameName
			std::size_t		idGameDirPlaceHolder = path.find( TEXT( "%Game%" ) );
			while ( idGameDirPlaceHolder != std::wstring::npos )
			{
				path.replace( idGameDirPlaceHolder, 6, g_GameName );	// 6 is the length of "%Game%"
				idGameDirPlaceHolder = path.find( TEXT( "%Game%" ) );
			}

			packagePaths.push_back( Sys_BaseDir() + path );
		}
	}

	if ( packagePaths.empty() )
	{
		Warnf( TEXT( "In 'Engine' config the section 'Core.System:PackagePaths' is empty or not correct\n" ) );
	}

	// Cache file paths
	CPackageFileCache::Get().CachePaths();
}

/*
==================
CSystem::Shutdown
==================
*/
void CSystem::Shutdown()
{
	packageExtensions.clear();
	packagePaths.clear();
	CPackageFileCache::Get().ClearPackageCache();
}