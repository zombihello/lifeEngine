#include <unordered_map>

#include "Logger/LoggerMacros.h"
#include "Reflection/Object.h"
#include "Reflection/ObjectPackage.h"
#include "Reflection/LinkerLoad.h"

/**
 * @ingroup Core
 * @brief Linker index pair
 */
struct LinkerIndexPair
{
	/**
	 * @brief Constructor
	 */
	LinkerIndexPair()
		: linker( nullptr )
		, linkerIndex( INDEX_NONE )
	{
		CheckInvariants();
	}

	/**
	 * @brief Constructor
	 * 
	 * @param InLinker linker to assign
	 * @param InLinkerIndex linker index to assign
	 */
	LinkerIndexPair( CLinkerLoad* InLinker, int32 InLinkerIndex )
		: linker( InLinker )
		, linkerIndex( InLinkerIndex )
	{
		CheckInvariants();
	}

	/**
	 * @brief Determine if this linker pair is the default
	 * @return Return TRUE is this is a default pair. We only check the linker because CheckInvariants rules out bogus combinations
	 */
	FORCEINLINE bool IsDefault() const
	{
		CheckInvariants();
		return linker == nullptr;
	}

	/**
	 * @brief Assert() that either the linker and the index is valid or neither of them are
	 */
	FORCEINLINE void CheckInvariants() const
	{
		Assert( !( ( linker == nullptr ) ^ ( linkerIndex == INDEX_NONE ) ) );	// you need either a valid linker and index or neither valid
	}

	CLinkerLoad*	linker;			/**< Linker that contains the ObjectExport resource corresponding to this object. NULL if this object is native only (i.e. never stored in a LifeEngine package), or if this object has been detached from its linker */
	uint32			linkerIndex;	/**< Index into the linker's ExportMap array for the ObjectExport resource corresponding to this object */
};

//
// GLOBAL
//
/** Map to relate linkers, indices and CObjects  */
static std::unordered_map<const CObject*, LinkerIndexPair>		s_LinkerMap;

/*
==================
CObject::StaticCleanupLinkerMap
==================
*/
void CObject::StaticCleanupLinkerMap()
{
	s_LinkerMap.clear();
}

/*
==================
CObject::SetLinker
==================
*/
void CObject::SetLinker( class CLinkerLoad* InLinkerLoad, uint32 InLinkerIndex, bool InIsShouldDetachExisting /* = true */ )
{
	bool				bExistPairInTheMap = false;
	LinkerIndexPair		existing;
	{
		auto	itPair = s_LinkerMap.find( this );
		if ( itPair != s_LinkerMap.end() )
		{
			existing = itPair->second;
			bExistPairInTheMap = true;
		}
	}
	existing.CheckInvariants();

	// Detach from existing linker
	if ( existing.linker && InIsShouldDetachExisting )
	{
		Warnf( TEXT( "Detaching from existing linker for '%s' for object '%s'\n" ), existing.linker->GetPath().c_str(), GetFullName().c_str() );
		Assert( existing.linker->GetExports()[existing.linkerIndex].object != nullptr );
		Assert( existing.linker->GetExports()[existing.linkerIndex].object == this );
		existing.linker->GetExports()[existing.linkerIndex].object = nullptr;
	}

	// No change so don't call notify
	if ( existing.linker == InLinkerLoad )
	{
		InIsShouldDetachExisting = false;
	}
	
	// If we have a pair in the map and aren't setting a new valid pair, remove the one
	if ( bExistPairInTheMap && !InLinkerLoad && InLinkerIndex == INDEX_NONE )
	{
		s_LinkerMap.erase( this );
	}
	// Otherwise insert into the map a new pair if the new one is valid
	else if ( ( InLinkerLoad || InLinkerIndex != INDEX_NONE ) && ( existing.linker != InLinkerLoad || existing.linkerIndex != InLinkerIndex ) )
	{
		s_LinkerMap.insert( std::make_pair( this, LinkerIndexPair( InLinkerLoad, InLinkerIndex ) ) );
	}

	if ( InIsShouldDetachExisting )
	{
#if WITH_EDITOR
		PostLinkerChange();
#else
		Warnf( TEXT( "It is only legal to change linkers in the editor. Trying to change linker on '%s' from '%s' (existing->linkerRoot=%s) to '%s' (linkerLoad->linkerRoot=%s)\n" ),
			   GetFullName().c_str(),
			   existing.linker->GetPath().c_str(),
			   existing.linker ? existing.linker->GetLinkerRoot()->GetName().c_str() : TEXT( "None" ),
			   InLinkerLoad->GetPath().c_str(),
			   InLinkerLoad ? InLinkerLoad->GetLinkerRoot()->GetName().c_str() : TEXT( "None" ) );
#endif // !WITH_EDITOR
	}
}

/*
==================
CObject::GetLinker
==================
*/
CLinkerLoad* CObject::GetLinker() const
{
	auto	itPair = s_LinkerMap.find( this );
	if ( itPair != s_LinkerMap.end() )
	{
		itPair->second.CheckInvariants();
		return itPair->second.linker;
	}

	return nullptr;
}

/*
==================
CObject::GetLinkerIndex
==================
*/
uint32 CObject::GetLinkerIndex() const
{
	auto	itPair = s_LinkerMap.find( this );
	if ( itPair != s_LinkerMap.end() )
	{
		itPair->second.CheckInvariants();
		return itPair->second.linkerIndex;
	}

	return INDEX_NONE;
}