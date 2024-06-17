/**
 * @file
 * @addtogroup Core Core
 *
 * Copyright BSOD-Games, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef OBJECT_H
#define OBJECT_H

#include <string>

#include "Logger/LoggerMacros.h"
#include "Core.h"
#include "System/Name.h"
#include "Reflection/ObjectMacros.h"
#include "Reflection/ObjectGC.h"
#include "Reflection/ObjectHash.h"
#include "Scripts/ScriptVM.h"

// Forward declarations
void HandleObjectReference( std::vector<CObject*>& InOutObjectArray, CObject*& InOutObject, bool InIsAllowReferenceElimination );

/**
 * @ingroup Core
 * @brief The base class of all objects
 */
class CObject : public CScriptVM
{
    DECLARE_BASE_CLASS( CObject, CObject, CLASS_Abstract, 0, TEXT( "Core" ) )
    DECLARE_REGISTER_NATIVE_FUNCS() {}

public:
    friend bool IsA( CObject* InObject, CClass* InClass );
    friend bool IsIn( CObject* InObject, CObject* InOuter );
    friend bool IsInA( CObject* InObject, CClass* InClass );
    friend class CObjectGC;
    friend class CClass;
    friend class CObjectPackage;

    /**
     * @brief Default within class type
     */
    typedef CObject             WithinClass;

    /**
     * @brief Constructor
     */
    CObject();

    /**
     * @brief Constructor
     * 
     * @param InName            Object name
     * @param InPackageName     Package name
     * @param InFlags           The object flags
     */
    CObject( ENativeConstructor, const CName& InName, const tchar* InPackageName, ObjectFlags_t InFlags = OBJECT_None );

    /**
     * @brief Destructor
     */
    virtual ~CObject();

    /**
     * @brief Create a new instance of an object
     *
     * @param InClass	The class of the object to create
     * @param InOuter   The object to create this object within
     * @param InName	The name to give the new object
     * @param InFlags   The object flags
     * @return Return allocated object, but it not initialized
     */
    static CObject* StaticAllocateObject( class CClass* InClass, CObject* InOuter = nullptr, CName InName = NAME_None, ObjectFlags_t InFlags = OBJECT_None );

    /**
     * @brief Create a new instance of an object
     *
     * @param InClass	The class of the object to create
     * @param InOuter   The object to create this object within
     * @param InName	The name to give the new object
     * @param InFlags   The object flags
     * @return Return allocated object, it will be fully initialized
     */
    static CObject* StaticConstructObject( class CClass* InClass, CObject* InOuter = nullptr, CName InName = NAME_None, ObjectFlags_t InFlags = OBJECT_None );

    /**
     * @brief Initialize CObject system
     */
    static void StaticInit();

    /**
     * @brief Shutdown CObject system
     */
    static void StaticExit();

    /**
     * @brief Is CObject system initialized
     * @return Return TRUE if CObject system is initialized, otherwise returns FALSE
     */
    static bool StaticIsCObjectInitialized();

    /**
     * @brief Create a unique name by combining a base name and an arbitrary number string
     * The object name returned is guaranteed not to exist
     * 
     * @param InOuter       The outer for the object that needs to be named
     * @param InClass       The class for the object
     * @param InBaseName    Optional base name to use when generating the unique object name. If not specified, the class's name is used
     * @return Return name in the form BaseName_##, where ## is the number of objects of this type that have been created since the last time the class was garbage collected
     */
    static CName MakeUniqueObjectName( CObject* InOuter, CClass* InClass, CName InBaseName = NAME_None );

    /**
     * @brief Add function of static class initialize to auto registrants
     * @param InStaticInitializeFn      Static class initialize function
     */
    static void AddToAutoInitializeRegistrants( CObject* ( *InStaticInitializeFn )() );

    /**
     * @brief Serialize object
     * @param InArchive     Archive for serialize
     */
    virtual void Serialize( class CArchive& InArchive );

#if WITH_EDITOR
    /**
     * @brief Function called by the editor when property is changed
     * @param InPropertyChangedEvenet    Property changed event
     */
    virtual void PostEditChangeProperty( const PropertyChangedEvenet& InPropertyChangedEvenet );

    /**
     * @brief Called by the editor to query whether a property of this object is allowed to be modified
     * 
     * @param InProperty    Property
     * @return Return TRUE if the property can be modified in the editor, otherwise FALSE
     */
    virtual bool CanEditProperty( class CProperty* InProperty ) const;

    /**
     * @brief Called in response to the linker changing, this can only happen in the editor
     */
    virtual void PostLinkerChange();
#endif // WITH_EDITOR

    /**
     * @brief Called before destroying the object
     * This is called immediately upon deciding to destroy the object, to allow the object to begin an
     * asynchronous cleanup process
     */
    virtual void BeginDestroy();

    /**
     * @brief Called to finish destroying the object 
     * @note After CObject::FinishDestroy is called, the object's memory should no longer be accessed
     */
    virtual void FinishDestroy();

    /**
     * @brief Called to check if the object is ready for FinishDestroy
     * This is called after BeginDestroy to check the completion of the
     * potentially asynchronous object cleanup
     * 
     * @return Return TRUE if the object's asynchronous cleanup has completed and it is ready for FinishDestroy to be called
     */
    virtual bool IsReadyForFinishDestroy() const;

    /**
	 * @brief Callback used to allow object register its direct object references that are not already covered by
	 * the token stream
     * 
     * @param InOutObjectArray	Array to add referenced objects to via AddReferencedObject
     */
    virtual void AddReferencedObjects( std::vector<CObject*>& InOutObjectArray );

    /**
     * @brief Called from within CObjectPackage::SavePackage on the passed in base/root object
     * This is used to allow objects used as a base to perform required actions before saving and cleanup afterwards
     * 
     * @param InFilename    Name of the file being saved to (includes path)
     * @return Return TRUE whether PostSaveRoot needs to perform internal cleanup, otherwise returns FALSE
     */
    virtual bool PreSaveRoot( const tchar* InFilename );

    /**
     * @brief Called from within CObjectPackage::SavePackage on the passed in base/root object
     * This function is called after the package has been saved and can perform cleanup
     * 
     * @param InIsCleanupIsRequired	    Whether PreSaveRoot dirtied state that needs to be cleaned up
     */
    virtual void PostSaveRoot( bool InIsCleanupIsRequired );

    /**
     * @brief Presave function
     * @warning Objects created from within PreSave won't have PreSave called on them
     *
     * Gets called once before an object gets serialized for saving. This function is necessary
     * for save time computation as Serialize gets called three times per object from within CObjectPackage::SavePackage
     */
    virtual void PreSave();

    /**
     * @brief Registers the native class (constructs a CClass object)
     */
    virtual void Register();

    /**
     * @brief Marks the package containing this object as needing to be saved
     */
    void MarkPackageDirty() const;

    /**
     * @brief Add an object to the root set
     * This prevents the object and all its descendants from being deleted during garbage collection
     */
    FORCEINLINE void AddToRoot()
    {
        AddObjectFlag( OBJECT_RootSet );
    }

    /**
     * @brief Remove an object from the root set
     */
	FORCEINLINE void RemoveFromRoot()
    {
        RemoveObjectFlag( OBJECT_RootSet );
    }

    /**
     * @brief Is an object the root set
     * @return Return TRUE if this object is the root set, otherwise returns FALSE
     */
    FORCEINLINE bool IsRootSet() const
    {
        return HasAnyObjectFlags( OBJECT_RootSet );
    }

    /**
	 * @brief Add object flag
	 * @param InNewFlag		New flag
	 */
    FORCEINLINE void AddObjectFlag( ObjectFlags_t InNewFlag )
    {
        flags |= InNewFlag;
    }

    /**
     * @brief Remove object flag
     * @param InFlag	Flag to remove
     */
    FORCEINLINE void RemoveObjectFlag( ObjectFlags_t InFlag )
    {
        flags &= ~InFlag;
    }

    /**
     * @brief Rename object or change outer
     * 
     * @param InNewName     The new name of the object, if NULL then InNewOuter should be set
     * @param InNewOuter    New outer this object will be placed within, if NULL it will use the current outer
     * @param InFlags       Flags to specify what happens during the rename (see ERenameFlags)
     * @return Return TRUE if object has been renamed/changed outer, otherwise returns FALSE
     */
    virtual bool Rename( const tchar* InNewName = nullptr, CObject* InNewOuter = nullptr, uint32 InFlags = REN_None );

    /**
     * @brief Set object flags
     * @param InFlags   New object flags
     */
    FORCEINLINE void SetObjectFlags( ObjectFlags_t InFlags )
    {
        flags = InFlags;
    }

    /**
     * @brief Get object name
     * @return Return object name
     */
    FORCEINLINE std::wstring GetName() const
    {
		if ( !this )
		{
			return TEXT( "None" );
		}
		else
		{
			if ( index == INDEX_NONE )
			{
				return TEXT( "<uninitialized>" );
			}
			else
			{
				return name.ToString();
			}
		}
    }

    /**
     * @brief Get object name
     * @param OutName	Output object name
     */
    FORCEINLINE void GetName( std::wstring& OutName ) const
    {
		if ( !this )
		{
            OutName = TEXT( "None" );
		}
		else
		{
			if ( index == INDEX_NONE )
			{
                OutName = TEXT( "<uninitialized>" );
			}
			else
			{
				name.ToString( OutName );
			}
		}
    }

    /**
     * @brief Append the object name into OutResult
     * @param OutResult	    Output result string with the object name
     */
    FORCEINLINE void AppendName( std::wstring& OutResult ) const
    {
        if ( !this )
        {
            OutResult += TEXT( "None" );
        }
        else
        {
            if ( index == INDEX_NONE )
            {
                OutResult += TEXT( "<uninitialized>" );
            }
            else
            {
                name.AppendString( OutResult );
            }
        }
    }

    /**
     * @brief Get object name
     * @return Return object name (CName)
     */
    FORCEINLINE CName GetCName() const
    {
        return index != INDEX_NONE ? name : TEXT( "<uninitialized>" );
    }

    /**
     * @brief Get the fully qualified pathname for this object as well as the name of the class
     * @note Safe to call on NULL object pointers
     * 
     * @param InStopOuter      If specified, indicates that the output string should be relative to this object. if InStopOuter
     *                         does not exist in this object's Outer chain, the result would be the same as passing NULL
     * @return Returns the fully qualified pathname for this object as well as the name of the class, in the format 'ClassName Outermost[.Outer].Name'
     */
    std::wstring GetFullName( const CObject* InStopOuter = nullptr ) const;

    /**
     * @brief Get the fully qualified pathname for this object
     * @note Safe to call on NULL object pointers
     * 
	 * @param InStopOuter      If specified, indicates that the output string should be relative to this object. if InStopOuter
	 *                         does not exist in this object's Outer chain, the result would be the same as passing NULL
     * @return Returns the fully qualified pathname for this object, in the format 'Outermost[.Outer].Name'
     */
    std::wstring GetPathName( const CObject* InStopOuter = nullptr ) const;

    /**
     * @brief Set the class for this object
     * @note It is unsafe to use this method for changing the class of an existing object
     *
     * @param InNewClass    A new class for this object
     */
    FORCEINLINE void SetClass( CClass* InNewClass )
    {
        theClass = InNewClass;
    }

    /**
     * @brief Get class of this object
     * @return Return class of this object
     */
    FORCEINLINE CClass* GetClass() const
    {
        return theClass;
    }

    /**
     * @brief Get the object this object is in
     * @return Return the object this object is in (can be NULL)
     */
	FORCEINLINE CObject* GetOuter() const
	{
		return outer;
	}

    /**
     * @brief Walks up the list of outers until it finds the highest one
     * @return Return outermost non NULL outer
     */
    class CObjectPackage* GetOutermost() const;

    /**
     * @brief Traverses the outer chain searching for the next object of a certain type
     * 
     * @param InTargetClass     Target class to search for (must be derived from CObject)
     * @return Return a pointer to the first object in this object's Outer chain which is of the correct type
     */
    CObject* GetTypedOuter( CClass* InTargetClass ) const;

    /**
     * @brief Get object flags
     * @return Return object flags
     */
    FORCEINLINE ObjectFlags_t GetObjectFlags() const
    {
        return flags;
    }

    /**
     * @brief Get the object's index into the global objects array
     * @return Return object's index in the global objects array
     */
    FORCEINLINE uint32 GetIndex() const
    {
        return index;
    }

    /**
     * @brief Set linker
     * 
     * Changes the linker and linker index to the passed in one.
     * A linker of NULL and linker index of INDEX_NONE indicates that the object is without a linker
     * 
     * @param InLinkerLoad              New linker load object to set
     * @param InLinkerIndex             New linker index to set
     * @param InIsShouldDetachExisting  Is should we detach existing linker
     */
    void SetLinker( class CLinkerLoad* InLinkerLoad, uint32 InLinkerIndex, bool InIsShouldDetachExisting = true );

    /**
     * @brief Get linker
     * @return Return a pointer to the linker for this object, or NULL if this object has no linker
     */
    class CLinkerLoad* GetLinker() const;

    /**
     * @brief Get linker index
     * @return Return the index into my linker's ExportMap for the ObjectExport corresponding to this object
     */
    uint32 GetLinkerIndex() const;

    /**
     * @brief Remove all map on exit. This is to prevent issues with the order of static destruction of singletons
     */
    static void CleanupLinkerMap();

    /**
     * @brief Has any object flags
     *
     * @param InCheckFlags		Object flags to check for
     * @return Return TRUE if the passed in flag is set (including no flag passed in), otherwise FALSE
     */
    FORCEINLINE bool HasAnyObjectFlags( ObjectFlags_t InCheckFlags ) const
    {
        return ( flags & InCheckFlags ) != 0;
    }

    /**
     * @brief Has all object flags
     *
     * @param InCheckFlags		Object flags to check for
     * @return Return TRUE if all of the passed in flags are set (including no flags passed in), otherwise FALSE
     */
    FORCEINLINE bool HasAllObjectFlags( ObjectFlags_t InCheckFlags ) const
    {
        return ( flags & InCheckFlags ) == InCheckFlags;
    }

    /**
     * @brief Is the object unreachable
     * @return Return TRUE if the object have flag OBJECT_Unreachable and it isn't root set, otherwise returns FALSE
     */
    FORCEINLINE bool IsUnreachable() const
    {
        return !HasAnyObjectFlags( OBJECT_RootSet ) && HasAnyObjectFlags( OBJECT_Unreachable );
    }

    /**
     * @brief Is disregarded for GC
     * @return Return TRUE if disregarded for GC, otherwise FALSE 
     */
    FORCEINLINE bool IsDisregardedForGC() const
    {
        return GetIndex() < CObjectGC::Get().firstGCIndex;
    }

    /**
     * @brief Is the object pending kill
     * @return Return TRUE if the object is pending kill, otherwise returns FALSE
     */
    FORCEINLINE bool IsPendingKill() const
    {
        return HasAnyObjectFlags( OBJECT_PendingKill );
    }

    /**
     * @brief Mark this object as pending kill
     */
    FORCEINLINE void MarkPendingKill()
    {
        AddObjectFlag( OBJECT_PendingKill );
    }

    /**
     * @brief Is valid object
     * @return Return TRUE if the object is valid, otherwise returns FALSE
     */
    bool IsValid() const;

    /**
     * @brief Call PostLoad if needed
     */
    void ConditionalPostLoad();

protected:
    /**
     * @brief Route BeginDestroy() and EndDestroy() on this object if this object is still valid
     * @warning This method should never be called directly (it is called from the destructor), or the object can be left in a bad state
     */
    void ConditionalDestroy();

    /**
     * @brief Helper function to add a referenced object to the passed in array
     * The function ensures that the item won't be added twice by checking the OBJECT_Unreachable flag
     *
     * @param InOutObjectArray	Array to add object to
     * @param InObject		    Object to add if it isn't already part of the array (is reachable)
     */
    static FORCEINLINE void AddReferencedObject( std::vector<CObject*>& InOutObjectArray, CObject* InObject )
    {
        HandleObjectReference( InOutObjectArray, InObject, false );
    }

    /**
     * @brief Do any object-specific cleanup required immediately after loading an object
     * @note This is not called for newly-created objects, and by default will always execute on the game thread
     */
    virtual void PostLoad();

private:
    /**
     * @brief Called before destroying the object
     * This is called immediately upon deciding to destroy the object, to allow the object to begin an
     * asynchronous cleanup process
     * 
     * @param InIsUnhashObject  Is need unhash this object from table
     */
    FORCEINLINE void ConditionalBeginDestroy( bool InIsUnhashObject = true )
    {
        if ( index != INDEX_NONE && !HasAnyObjectFlags( OBJECT_BeginDestroyed ) )
        {
            AddObjectFlag( OBJECT_BeginDestroyed );
            BeginDestroy();

            // Unhash object, removing it from object hash so it cannot be found from now on
            if ( InIsUnhashObject )
            {
                UnhashObject( this );
            }
        }
    }

    /**
	 * @brief Called to finish destroying the object
	 * @note After CObject::FinishDestroy is called, the object's memory should no longer be accessed
     * 
     * @return Return TRUE if object was destroyed, otherwise returns FALSE
     */
    FORCEINLINE bool ConditionalFinishDestroy()
    {
        if ( index != INDEX_NONE && !HasAnyObjectFlags( OBJECT_FinishDestroyed ) )
        {
            AddObjectFlag( OBJECT_FinishDestroyed );
            FinishDestroy();
            return true;
        }
        else
        {
            return false;
        }
    }

    /**
     * @brief Override operator delete
     * Private delete function so only GC can delete objects
     * 
     * @param InObject  Pointer to allocated object
     * @param InSize    Allocated memory size
     */
    void operator delete( void* InObject, size_t InSize );

    /**
     * @brief Internal version of GetPathName() that eliminates lots of copies
     */
    void GetPathName( const CObject* InStopOuter, std::wstring& OutResult ) const;

    /**
     * @brief Whether the object is of the specified class
     *
     * @param InClass   Checking class
     * @return Return TRUE if the object is a class InClass, otherwise returns FALSE
     */
    bool InternalIsA( const CClass* InClass ) const;

    /**
     * @brief Is the object in InOuter
     * 
     * @param InOuter   Some outer
     * @return Return TRUE if the specified object appears somewhere in this object's outer chain
     */
    bool InternalIsIn( const CObject* InOuter ) const;

    /**
     * @brief Find out if this object is inside that is of the specified class
     * 
     * @param InClass      The base class to compare against
     * @return Return TRUE if this object is in an object of the given type
     */
    bool InternalIsInA( const CClass* InClass ) const;

    uint32          index;              /**< Index of object into global objects array */
    CName           name;               /**< Name object */ 
    CObject*        outer;              /**< Object this object resides in */
    ObjectFlags_t   flags;              /**< Object flags */ 
    CClass*         theClass;           /**< Class of this object */
};

/**
 * @ingroup Core
 * @brief Handles object reference, potentially NULL'ing
 * 
 * @param InOutObjectArray	            Array to add object to
 * @param InOutObject			        Object pointer passed by reference
 * @param InIsAllowReferenceElimination Whether to allow NULL'ing the reference if OBJECT_PendingKill is set
 */
FORCEINLINE void HandleObjectReference( std::vector<CObject*>& InOutObjectArray, CObject*& InOutObject, bool InIsAllowReferenceElimination )
{
    Assert( !InOutObject || InOutObject->IsValid() );
    if ( InOutObject )
    {
        // Remove references to pending kill objects if we're allowed to do so
        if ( InOutObject->HasAnyObjectFlags( OBJECT_PendingKill ) && InIsAllowReferenceElimination )
        {
            // NULL out reference
            InOutObject = nullptr;
        }
        else
        {
            // Add encountered object reference to list of to be serialized objects if it hasn't already been added
            if ( InOutObject->HasAnyObjectFlags( OBJECT_Unreachable ) )
            {
                // Mark it as reachable
                InOutObject->RemoveObjectFlag( OBJECT_Unreachable );

                // Add it to the list of reachable objects
                InOutObjectArray.push_back( InOutObject );
            }
        }
    }
}

/**
 * @ingroup Core
 * @brief Is the object a class TClass
 *
 * @param InObject  Object
 * @param InClass   Class
 * @return Return TRUE if InObject is a class InClass, otherwise returns FALSE
 */
FORCEINLINE bool IsA( CObject* InObject, CClass* InClass )
{
    if ( InObject )
    {
        return InObject->InternalIsA( InClass );
    }

    return false;
}

/**
 * @ingroup Core
 * @brief Is the object in InOuter
 *
 * @param InObject  Object
 * @param InOuter   Some outer
 * @return Return TRUE if the specified object appears somewhere in this object's outer chain
 */
FORCEINLINE bool IsIn( CObject* InObject, CObject* InOuter )
{
    if ( InObject )
    {
        return InObject->InternalIsIn( InOuter );
    }

    return false;
}

/**
 * @ingroup Core
 * @brief Find out if this object is inside that is of the specified class
 *
 * @param InObject  Object
 * @param InClass   The base class to compare against
 * @return Return TRUE if this object is in an object of the given type
 */
FORCEINLINE bool IsInA( CObject* InObject, CClass* InClass )
{
    if ( InObject )
    {
        return InObject->InternalIsInA( InClass );
    }

    return false;
}

/**
 * @ingroup Core
 * @brief Is the object a class TClass
 * 
 * @param InObject  Object
 * @return Return TRUE if InObject is a class TClass, otherwise returns FALSE
 */
template<typename TClass>
FORCEINLINE bool IsA( CObject* InObject )
{
    return IsA( InObject, TClass::StaticClass() );
}

/**
 * @ingroup Core
 * @brief Find out if this object is inside that is of the specified class
 *
 * @param InObject  Object
 * @return Return TRUE if this object is in an object of the given type
 */
template<typename TClass>
FORCEINLINE bool IsInA( CObject* InObject )
{
    return IsInA( InObject, TClass::StaticClass() );
}

/**
 * @ingroup Core
 * @brief Cast object to class a TClass
 * 
 * @param InObject      Object
 * @return Return pointer with type TClass. If InObject not is the class TClass returning NULL
 */
template<typename TClass>
FORCEINLINE TClass* Cast( CObject* InObject )
{
    if ( IsA<TClass>( InObject ) )
    {
        return ( TClass* )InObject;
    }

    return nullptr;
}

/**
 * @ingroup Core
 * @brief Cast object to class a TClass
 * 
 * @param InObject      Object
 * @return Return const pointer with type TClass. If InObject not is the class TClass returning nullptr
 */
template<typename TClass>
FORCEINLINE const TClass* ConstCast( CObject* InObject )
{
    if ( IsA<TClass>( InObject ) )
    {
        return ( const TClass* )InObject;
   }

    return nullptr;
}

/**
 * @ingroup Core
 * @brief Cast object to class a TClass (excluding class parents)
 * 
 * @param InObject      Object
 * @return Return const pointer with type TClass. If InObject not is the class TClass returning nullptr
 */
template<typename TClass>
FORCEINLINE TClass* ExactCast( CObject* InObject )
{
    if ( InObject && InObject->GetClass() == TClass::StaticClass() )
    {
        return ( TClass* )InObject;
    }

    return nullptr;
}

/**
 * @ingroup Core
 * @brief Cast object to class a TClass (excluding class parents)
 *
 * @param InObject      Object
 * @return Return const pointer with type TClass. If InObject not is the class TClass returning nullptr
 */
template<typename TClass>
FORCEINLINE const TClass* ConstExactCast( CObject* InObject )
{
    if ( InObject && InObject->GetClass() == TClass::StaticClass() )
    {
        return ( const TClass* )InObject;
    }

    return nullptr;
}

#endif // !OBJECT_H
