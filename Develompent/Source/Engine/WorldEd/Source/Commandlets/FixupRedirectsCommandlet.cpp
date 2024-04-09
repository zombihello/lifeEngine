#include <unordered_set>

#include "Misc/FileTools.h"
#include "Misc/CoreGlobals.h"
#include "Reflection/Class.h"
#include "Reflection/ObjectPackage.h"
#include "Reflection/ObjectRedirector.h"
#include "Reflection/LinkerLoad.h"
#include "Reflection/ObjectIterator.h"
#include "Reflection/ObjectGlobals.h"
#include "System/PackageFileCache.h"
#include "System/Package.h"
#include "System/BaseFileSystem.h"
#include "System/World.h"
#include "Commandlets/FixupRedirectsCommandlet.h"

IMPLEMENT_CLASS( CFixupRedirectsCommandlet )
IMPLEMENT_DEFAULT_INITIALIZE_CLASS( CFixupRedirectsCommandlet )

/** Collect garbage after was loaded N packages */
#define COLLECT_GARBAGE_AFTER_LOADED_PACKAGES		30

/**
 * @ingroup WorldEd
 * @brief Struct that holds info about a redirector that was followed
 */
struct Redirection
{
	/**
	 * @brief Hash function for STL containers
	 */
	struct HashFunction
	{
		/**
		 * @brief Calculate hash of Redirection
		 * @param InRedirection		Redirection
		 */
		FORCEINLINE size_t operator()( const Redirection& InRedirection ) const
		{
			size_t		hash = FastHash( InRedirection.redirectorName );
			hash = FastHash( InRedirection.redirectorPackage, hash );
			hash = FastHash( InRedirection.destinationObjectName, hash );
			return FastHash( InRedirection.package, hash );
		}
	};

	/**
	 * @brief Overrload operator ==
	 * @return Return TRUE if two redirections is same, otherwise returns FALSE
	 */
	FORCEINLINE friend bool operator==( const Redirection& InX, const Redirection& InY )
	{
		return InX.redirectorName == InY.redirectorName &&
			InX.redirectorPackage == InY.redirectorPackage &&
			InX.destinationObjectName == InY.destinationObjectName &&
			InX.package == InY.package;
	}

	/**
	 * @brief Overrload operator !=
	 * @return Return TRUE if two redirections isn't same, otherwise returns FALSE
	 */
	FORCEINLINE friend bool operator!=( const Redirection& InX, const Redirection& InY )
	{
		return InX.redirectorName != InY.redirectorName ||
			InX.redirectorPackage != InY.redirectorPackage ||
			InX.destinationObjectName != InY.destinationObjectName ||
			InX.package != InY.package;
	}

	std::wstring	redirectorName;			/**< Redirector object name */
	std::wstring	redirectorPackage;		/**< Package file name where the redirector object is */
	std::wstring	destinationObjectName;	/**< Object name on that the redirector redirects */
	std::wstring	package;				/**< Package file name that reference to this redirector */
};

/**
 * @ingroup WorldEd
 * @brief Helper class for get redirect followed event and store the information
 */
class CRedirectCollector
{
public:
	/**
	 * @brief Constructor
	 */
	CRedirectCollector()
		: redirectorFollowedDelegate( nullptr )
	{}

	/**
	 * @brief Destructor
	 */
	~CRedirectCollector()
	{
		Shutdown();
	}

	/**
	 * @brief Initialize collector
	 */
	void Init()
	{
		// Subscribe to event when CObjectRedirector was followed to find the destination object
		redirectorFollowedDelegate = CObjectRedirector::onObjectRedirectorFollowed.Add( std::bind( &CRedirectCollector::RedirectorFollowed, this, std::placeholders::_1, std::placeholders::_2 ) );
	}

	/**
	 * @brief Shutdown collector
	 */
	void Shutdown()
	{
		// Describe from the event
		if ( redirectorFollowedDelegate )
		{
			CObjectRedirector::onObjectRedirectorFollowed.Remove( redirectorFollowedDelegate );
			redirectorFollowedDelegate = nullptr;
		}
	}

	/**
	 * @brief Get list of all referenced redirections
	 * @return Return list of all referenced redirections
	 */
	FORCEINLINE const std::unordered_set<Redirection, Redirection::HashFunction>& GetRedirections() const
	{
		return redirections;
	}

private:
	/**
	 * @brief Called when CObjectRedirector was followed to find the destination object
	 * 
	 * @param InPackageName         Name of the package that pointed to the redirect
	 * @param InObjectRedirector    The CObjectRedirector that was followed
	 */
	void RedirectorFollowed( const std::wstring& InPackageName, CObjectRedirector* InObjectRedirector )
	{
		// Save the info
		Redirection							redirection;
		redirection.redirectorName			= InObjectRedirector->GetFullName();
		redirection.redirectorPackage		= InObjectRedirector->GetLinker()->GetFileName();
		redirection.destinationObjectName	= InObjectRedirector->GetDestinationObject()->GetFullName();
		redirection.package					= InPackageName;
		redirections.insert( redirection );
	}

	COnObjectRedirectorFollowed::DelegateType_t*				redirectorFollowedDelegate;		/**< Delegate of redirector followed event */
	std::unordered_set<Redirection, Redirection::HashFunction>	redirections;					/**< A gathered list of all referenced redirections */
};

/*
==================
CCookerSyncCommandlet::Main
==================
*/
bool CFixupRedirectsCommandlet::Main( const CCommandLine& InCommandLine )
{
	// Initialize the redirect collector
	CRedirectCollector		redirectCollector;
	redirectCollector.Init();

	// Get list of all packages
	std::vector<std::wstring>	packageList = CPackageFileCache::Get().GetPackageFileList();
	if ( packageList.empty() )
	{
		return false;
	}

	// Collect all packages which we have to fixup/delete redirections
	bool								bHaveAnyError = false;
	std::unordered_set<std::wstring>	updatePackages;
	std::unordered_set<std::wstring>	redirectorsThatCantBeCleaned;
	Logf( TEXT( "Generating list of packages to fixup\n" ) );
	Logf( TEXT( "------------------------------------\n" ) );

	for ( uint32 index = 0, count = packageList.size(); index < count; ++index )
	{
		std::wstring		filename = packageList[index];

		// Load package to look for redirects there
		Logf( TEXT( "Looking for redirects in '%s'\n" ), filename.c_str() );
		CObjectPackage*		package = CObjectPackage::LoadPackage( nullptr, filename.c_str(), LOAD_None );
		if ( !package )
		{
			bHaveAnyError = true;
			Errorf( TEXT( "  Failed to open '%s'\n" ), filename.c_str() );
			continue;
		}

		// Look for any redirects that were followed that are referenced by this package
		for ( auto itRedirector = redirectCollector.GetRedirections().begin(), itEndRedirector = redirectCollector.GetRedirections().end(); itRedirector != itEndRedirector; ++itRedirector )
		{
			const Redirection&		redirection = *itRedirector;
			if ( redirection.package == filename )
			{
				// Put the source and destination packages in the list to be updated
				Logf( TEXT( "  References '%s' -> '%s'\n" ), redirection.redirectorName.c_str(), redirection.destinationObjectName.c_str() );
				updatePackages.insert( redirection.package );
				updatePackages.insert( redirection.redirectorPackage );
			}
		}

		// Is need to collect garbage
		bool	bNeedToCollectGarbage = false;

		// If this package has any redirectors, make sure we update it
		for ( TObjectIterator<CObjectRedirector> it; it; ++it )
		{
			if ( IsIn( *it, package ) )
			{
				// Make sure this package is in the list of packages to update
				updatePackages.insert( filename );

				// Make sure we GC if we found a redirector
				bNeedToCollectGarbage = true;
				Logf( TEXT( "  Has redirect '%s' -> '%s'\n" ), it->GetPathName().c_str(), it->GetDestinationObject()->GetFullName().c_str() );
			}
		}

		// Collect garbage every N packages or if there was any redirectors in the package
		// (to make sure that redirectors get reloaded properly and followed by the delegate)
		if ( bNeedToCollectGarbage || ( index % COLLECT_GARBAGE_AFTER_LOADED_PACKAGES ) == 0 )
		{
			// Collect garbage to close the package
			CObjectGC::Get().CollectGarbage( OBJECT_None );
			g_PackageManager->GarbageCollector();
		}
	}

	// Shutdown the redirect collector
	redirectCollector.Shutdown();

	// Print list of packages to fixup
	Logf( TEXT( "\n" ) );
	Logf( TEXT( "List of packages to fixup:\n" ) );
	Logf( TEXT( "------------------------------------\n" ) );
	for ( auto it = updatePackages.begin(), itEnd = updatePackages.end(); it != itEnd; ++it )
	{
		Logf( TEXT( "  %s\n" ), ( *it ).c_str() );
	}
	
	// Do nothing if we no have any package to update
	if ( updatePackages.empty() )
	{
		Logf( TEXT( "  No any package to fixup\n" ) );
		return !bHaveAnyError;
	}
	else
	{
		Logf( TEXT( "\n" ) );
	}

	// Find redirectors that are referenced by read-only packages
	for ( auto it = updatePackages.begin(), itEnd = updatePackages.end(); it != itEnd; ++it )
	{
		// If the checkout failed for any reason, this will still be read-only, so we can't clean it up
		const std::wstring&		filename = *it;
		if ( g_FileSystem->IsReadOnly( filename ) )
		{
			bHaveAnyError = true;
			Errorf( TEXT( "Couldn't check out '%s'\n" ), filename.c_str() );

			// Any redirectors that are pointed to by this read-only package can't be fixed up
			for ( auto itRedirector = redirectCollector.GetRedirections().begin(), itEndRedirector = redirectCollector.GetRedirections().end(); itRedirector != itEndRedirector; ++itRedirector )
			{
				// Any redirectors pointed to by this file we don't clean
				const Redirection&		redirection = *itRedirector;
				if ( redirection.package == filename )
				{
					Errorf( TEXT( "  Can't fixup references to '%s'\n" ), redirection.redirectorName.c_str() );
				}
				// Any redirectors in this file can't be deleted
				else if ( redirection.redirectorPackage == filename )
				{
					Errorf( TEXT( "  Can't delete '%s'\n" ), redirection.redirectorName.c_str() );
				}
			}
		}
		else
		{
			Logf( TEXT( "Checked out '%s'\n" ), filename.c_str() );
		}
	}

	// Load and save packages to save out the proper fixed up references
	Logf( TEXT( "\n" ) );
	Logf( TEXT( "Fixing references to point to proper objects:\n" ) );
	Logf( TEXT( "------------------------------------\n" ) );

	// Keep a list of all packages that have CObjectRedirects in them
	uint32					packageIndex = 0;
	std::vector<bool>		packagesWithRedirects;
	packagesWithRedirects.resize( updatePackages.size() );

	// Iterate over all packages, loading and saving to remove all references to CObjectRedirectors (can't delete the Redirects yet)
	for ( auto it = updatePackages.begin(), itEnd = updatePackages.end(); it != itEnd; ++it, ++packageIndex )
	{
		// Skip read only packages
		const std::wstring&		filename = *it;
		if ( g_FileSystem->IsReadOnly( filename ) )
		{
			continue;
		}

		// Load the package
		Logf( TEXT( "Cleaning '%s'\n" ), filename.c_str() );
		CObjectPackage*		package = CObjectPackage::LoadPackage( nullptr, filename.c_str(), LOAD_None );
		if ( !package )
		{
			continue;
		}

		// Look for any redirects in the package
		for ( TObjectIterator<CObjectRedirector> it; it; ++it )
		{
			if ( IsIn( *it, package ) )
			{
				packagesWithRedirects[packageIndex] = true;
				break;
			}
		}

		// Save the package
		CWorld*		world = FindObject<CWorld>( package, TEXT( "TheWorld" ) );
		if ( world )
		{
			CObjectPackage::SavePackage( package, world, OBJECT_None, filename.c_str(), SAVE_None );
		}
		else
		{
			CObjectPackage::SavePackage( package, nullptr, OBJECT_None, filename.c_str(), SAVE_None );
		}

		// Collect garbage to close the package
		CObjectGC::Get().CollectGarbage( OBJECT_None );
		g_PackageManager->GarbageCollector();
	}

	// Delete all redirects that are no longer referenced
	Logf( TEXT( "\n" ) );
	Logf( TEXT( "Deleting redirector objects:\n" ) );
	Logf( TEXT( "------------------------------------\n" ) );

	// Again, iterate over all packages, loading and saving to and this time deleting all
	packageIndex = 0;
	for ( auto it = updatePackages.begin(), itEnd = updatePackages.end(); it != itEnd; ++it, ++packageIndex )
	{
		// Skip packages that no have any redirects
		const std::wstring&			filename = *it;
		if ( !packagesWithRedirects[packageIndex] )
		{
			continue;
		}

		// Load package again
		Logf( TEXT( "Wiping '%s'\n" ), filename.c_str() );
		CObjectPackage*		package = CObjectPackage::LoadPackage( nullptr, filename.c_str(), LOAD_None );
		if ( !package )
		{
			continue;
		}

		// Delete all CObjectRedirects now
		bool								bIsDirty = false;
		bool								bIsEmptyPackage = true;
		std::vector<CObjectRedirector*>		redirectsToDelete;
		for ( CObjectIterator it; it; ++it )
		{
			if ( IsIn( *it, package ) )
			{
				if ( !IsA<CObjectRedirector>( *it ) )
				{
					bIsEmptyPackage = false;
				}
				else
				{
					bIsDirty = true;
					redirectsToDelete.emplace_back( ( CObjectRedirector* )( *it ) );
					Logf( TEXT( "  Deleting '%s'\n" ), it->GetFullName().c_str() );
				}
			}
		}

		// Mark them for deletion
		for ( uint32 index = 0, count = redirectsToDelete.size(); index < count; ++index )
		{
			CObjectRedirector*		objectRedirector = redirectsToDelete[index];
			objectRedirector->RemoveObjectFlag( OBJECT_Public );
			objectRedirector->MarkPendingKill();
		}

		// Save package if it isn't empty, otherwise delete
		if ( !bIsEmptyPackage )
		{
			CWorld*		world = FindObject<CWorld>( package, TEXT( "TheWorld" ) );
			if ( world )
			{
				CObjectPackage::SavePackage( package, world, OBJECT_None, filename.c_str(), SAVE_None );
			}
			else
			{
				CObjectPackage::SavePackage( package, nullptr, OBJECT_None, filename.c_str(), SAVE_None );
			}
		}
		else
		{
			g_FileSystem->Delete( filename );
		}

		// Collect garbage to close the package
		CObjectGC::Get().CollectGarbage( OBJECT_None );
		g_PackageManager->GarbageCollector();
	}

	// We are done!
	return !bHaveAnyError;
}