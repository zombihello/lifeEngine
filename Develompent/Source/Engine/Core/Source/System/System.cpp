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
	const CJsonValue*				configPackageExtensionsValue	= CConfig::Get().GetValue( CT_Engine, TEXT( "Core.System" ), TEXT( "PackageExtensions" ) );
	const std::vector<CJsonValue>*	configPackageExtensions			= configPackageExtensionsValue ? configPackageExtensionsValue->GetArray() : nullptr;
	if ( configPackageExtensions )
	{
		for ( uint32 index = 0, count = configPackageExtensions->size(); index < count; ++index )
		{
			const CJsonValue&		configPackageExtension = configPackageExtensions->at( index );
			if ( configPackageExtension.IsA( JVT_String ) )
			{
				packageExtensions.push_back( configPackageExtension.GetString() );
			}
		}
	}

	if ( packageExtensions.empty() )
	{
		Warnf( TEXT( "In 'Engine' config the section 'Core.System:PackageExtensions' is empty or not correct\n" ) );
	}

	// Get from config all directories with packages
	Assert( packagePaths.empty() );
	const CJsonValue*				configPackagePathsValue = CConfig::Get().GetValue( CT_Engine, TEXT( "Core.System" ), TEXT( "PackagePaths" ) );
	const std::vector<CJsonValue>*	configPackagePaths = configPackagePathsValue? configPackagePathsValue->GetArray() : nullptr;
	if ( configPackagePaths )
	{
		for ( uint32 index = 0, count = configPackagePaths->size(); index < count; ++index )
		{
			std::wstring	path = configPackagePaths->at( index ).GetString();
			if ( !path.empty() )
			{
				// If we have in the path placeholder %Game% then we replace it to g_GameName
				if ( path.find( TEXT( "%Game%" ) ) != std::wstring::npos )
				{
					path = L_ReplaceSubString( path, TEXT( "%Game%" ), g_GameName );
				}

				packagePaths.push_back( Sys_BaseDir() + path );
			}
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