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

/**
 * @ingroup Core
 * @brief The base class of all objects
 */
class CObject
{
    DECLARE_BASE_CLASS( CObject, CObject, CLASS_Abstract, 0 )
    DECLARE_REGISTER_NATIVES() {}

public:
	template<typename TClass>   friend bool IsA( CObject* InObject );
    
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
    CObject( const CName& InName, ObjectFlags_t InFlags = OBJECT_None );

    /**
     * @brief Destructor
     */
    virtual ~CObject() {}

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

private:
    /**
     * @brief Whether the object is of the specified class
     * @param[in] InClass Checking class
     * 
     * @return Return true if object is a class InClass, else returning false
     */
    bool InternalIsA( const CClass* InClass ) const;

    /**
     * @brief Execute a script function
     * @param InFunction    Function to call
     * @param InStack       Script stack
     */
    void ExecScriptFunction( class CFunction* InFunction, struct ScriptFrame& InStack );

    CName           name;       /**< Name object */ 
    CObject*        outer;      /**< Object this object resides in */
    ObjectFlags_t   flags;      /**< Object flags */ 
    CClass*         theClass;   /**< Class of this object */
};

/**
 * @ingroup Core
 * @brief Is the object a class TClass
 * 
 * @param InObject  Object
 * @return Return TRUE if InObject is a class TClass, else returning FALSE
 */
template<typename TClass>
FORCEINLINE bool IsA( CObject* InObject )
{
    if ( InObject )
    {
        return InObject->InternalIsA( TClass::StaticClass() );
    }

    return false;
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
