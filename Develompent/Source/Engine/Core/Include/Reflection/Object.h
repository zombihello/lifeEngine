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

#include "Core.h"
#include "System/Name.h"
#include "Reflection/ObjectMacros.h"
#include "Reflection/ObjectGC.h"

// Forward declarations
void HandleObjectReference( std::vector<CObject*>& InOutObjectArray, CObject*& InOutObject, bool InIsAllowReferenceElimination );

/**
 * @ingroup Core
 * @brief The base class of all objects
 */
class CObject
{
    DECLARE_BASE_CLASS( CObject, CObject, CLASS_Abstract, 0 )
    DECLARE_REGISTER_NATIVES() {}

public:
    friend bool IsA( CObject* InObject, CClass* InClass );
    friend class CObjectGC;

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
     * @param InName    Object name
     * @param InFlags   The object flags
     */
    CObject( ENativeConstructor, const CName& InName, ObjectFlags_t InFlags = OBJECT_None );

    /**
     * @brief Destructor
     */
    virtual ~CObject();

    /**
     * @brief Create a new instance of an object
     *
     * @param Class		The class of the object to create
     * @param InOuter   The object to create this object within
     * @param InName	The name to give the new object
     * @param InFlags   The object flags
     * @return Return allocated object, but it not initialized
     */
    static CObject* StaticAllocateObject( class CClass* InClass, CObject* InOuter = nullptr, CName InName = NAME_None, ObjectFlags_t InFlags = OBJECT_None );

    /**
     * @brief Create a new instance of an object
     *
     * @param Class		The class of the object to create
     * @param InOuter   The object to create this object within
     * @param InName	The name to give the new object
     * @param InFlags   The object flags
     * @return Return allocated object, it will be fully initialized
     */
    static CObject* StaticConstructObject( class CClass* InClass, CObject* InOuter = nullptr, CName InName = NAME_None, ObjectFlags_t InFlags = OBJECT_None );

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
    * @brief Call function
    * @param InFunctionName    Function name
    */
    void CallFunction( const CName& InFunctionName );

    /**
     * @brief Execute a script code
     * @param InStack   Script stack
     */
	void ExecScript( struct ScriptFrame& InStack );

    /**
     * @brief Execute script opcode 'Nop'
     * @param InStack   Script stack
     */
    void scrOpcode_Nop( struct ScriptFrame& InStack );

    /**
     * @brief Execute script opcode 'Call'
     * @param InStack   Script stack
     */
    void scrOpcode_Call( struct ScriptFrame& InStack );

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
     * @brief Set the object this object is in
     * @param InOuter   The object this object is in (can be NULL)
     */
    FORCEINLINE void SetOuter( CObject* InOuter )
    {
        outer = InOuter;
    }

    /**
     * @brief Set name object
     * @param InName    Name object (CName)
     */
    FORCEINLINE void SetCName( const CName& InName )
    {
        name = InName;
    }

    /**
     * @brief Set name object
     * @param InName    Name object
     */
    FORCEINLINE void SetName( const tchar* InName )
    {
        name = InName;
    }

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
        std::wstring	retName;
        name.ToString( retName );
        return retName;
    }

    /**
     * @brief Get object name
     * @param OutName	Output object name
     */
    FORCEINLINE void GetName( std::wstring& OutName ) const
    {
        name.ToString( OutName );
    }

    /**
     * @brief Get object name
     * @return Return object name (CName)
     */
    FORCEINLINE const CName& GetCName() const
    {
        return name;
    }

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

private:
    /**
     * @brief Called before destroying the object
     * This is called immediately upon deciding to destroy the object, to allow the object to begin an
     * asynchronous cleanup process
     */
    FORCEINLINE void ConditionalBeginDestroy()
    {
        if ( index != INDEX_NONE && !HasAnyObjectFlags( OBJECT_BeginDestroyed ) )
        {
            AddObjectFlag( OBJECT_BeginDestroyed );
            BeginDestroy();
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
     * @brief Whether the object is of the specified class
     *
     * @param InClass   Checking class
     * @return Return TRUE if the object is a class InClass, otherwise returns FALSE
     */
    bool InternalIsA( const CClass* InClass ) const;

    /**
     * @brief Execute a script function
     * @param InFunction    Function to call
     * @param InStack       Script stack
     */
    void ExecScriptFunction( class CFunction* InFunction, struct ScriptFrame& InStack );

    uint32          index;          /**< Index of object into global objects array */
    CName           name;           /**< Name object */ 
    CObject*        outer;          /**< Object this object resides in */
    ObjectFlags_t   flags;          /**< Object flags */ 
    CClass*         theClass;       /**< Class of this object */
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
