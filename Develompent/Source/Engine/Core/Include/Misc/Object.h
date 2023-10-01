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

/**
 * @ingroup Core
 * @brief Macro for declare class
 * 
 * @param TClass            Class
 * @param TSuperClass       Super class
 * @param TClassFlags       Class flags
 * @param TClassCastFlags   Class cast flags
 * 
 * Example usage: @code DECLARE_CLASS( CClass, CObject, 0, 0 ) @endcode
 */
#define DECLARE_CLASS( TClass, TSuperClass, TClassFlags, TClassCastFlags ) \
    private: \
        static class CClass*        staticClass; \
    public: \
        enum { StaticClassFlags=TClassFlags }; \
        enum { StaticClassCastFlags=TClassCastFlags }; \
	    typedef TClass		        ThisClass; \
	    typedef TSuperClass	        Super; \
        static CObject*             StaticConstructor( void* InPtr ); \
        static void                 StaticInitializeClass(); \
        static class CClass*        StaticClass(); \
        virtual class CClass*       GetClass() const; \
        void* operator new( size_t InSize, CObject* InOuter = nullptr, const CName& InName = NAME_None ) \
        { \
            return StaticAllocateObject( StaticClass(), InOuter, InName ); \
		} \
        void operator delete( void* InPtr, CObject* InOuter, const CName& InName ) \
        { \
            free( InPtr ); \
        } \
        void operator delete( void* InPtr ) \
        { \
            ::delete InPtr; \
        }

/**
 * @ingroup Core
 * @brief Macro for implement class
 * 
 * @param TClass    Class
 * 
 * Example usage: @code IMPLEMENT_CLASS( CObject ) @endcode
 */
#define IMPLEMENT_CLASS( TClass ) \
    CClass*     TClass::staticClass = nullptr; \
    CObject*    TClass::StaticConstructor( void* InPtr ) \
    { \
        return ::new( InPtr ) ThisClass(); \
    } \
    \
    CClass*     TClass::StaticClass() \
    { \
        if ( !staticClass ) \
        { \
            bool        bBaseClass = &ThisClass::StaticClass == &Super::StaticClass; \
            staticClass = ::new CClass \
            ( \
                TEXT( #TClass ), \
                StaticClassFlags, \
                StaticClassCastFlags, \
                sizeof( ThisClass ), \
                alignof( ThisClass ), \
                &ThisClass::StaticConstructor, \
                !bBaseClass ? Super::StaticClass() : nullptr \
            ); \
            TClass::StaticInitializeClass(); \
        } \
        \
        Assert( staticClass ); \
        return staticClass; \
    } \
    \
    CClass*     TClass::GetClass() const \
    { \
        return StaticClass(); \
    } \
    \
    struct Register##TClass \
    { \
        Register##TClass() \
        { \
            CClass::StaticRegisterClass( TClass::StaticClass() ); \
        } \
    } g_Register##TClass;

/**
 * @ingroup Core
 * @brief Macro for implement default StaticInitializeClass()
 *
 * @param TClass    Class
 *
 * Example usage: @code IMPLEMENT_DEFAULT_INITIALIZE_CLASS( CObject ) @endcode
 */
#define IMPLEMENT_DEFAULT_INITIALIZE_CLASS( TClass ) \
        void TClass::StaticInitializeClass() {} \

/**
 * @ingroup Core
 * @brief Enumeration of flags describing a class
 */
enum EClassFlags
{
	CLASS_None			= 0,		/**< None */
	CLASS_Deprecated	= 1 << 0,	/**< Class is deprecated */
	CLASS_Abstract		= 1 << 1,	/**< Class is abstract and can't be instantiated directly  */
};

/**
 * @ingroup Core
 * @brief Enumeration of flags used for quickly casting classes of certain types; all class cast flags are inherited
 */
enum EClassCastFlags
{
	CASTCLASS_None					= 0,		/**< None */
	CASTCLASS_CProperty				= 1 << 0,	/**< Cast to CProperty */
	CASTCLASS_CByteProperty			= 1 << 1,	/**< Cast to CByteProperty */
	CASTCLASS_CIntProperty			= 1 << 2,	/**< Cast to CIntProperty */
	CASTCLASS_CFloatProperty		= 1 << 3,	/**< Cast to CFloatProperty */
	CASTCLASS_CBoolProperty			= 1 << 4,	/**< Cast to CBoolProperty */
	CASTCLASS_CColorProperty		= 1 << 5,	/**< Cast to CColorProperty */
	CASTCLASS_CObjectProperty		= 1 << 6,	/**< Cast to CObjectProperty */
	CASTCLASS_CVectorProperty		= 1 << 7,	/**< Cast to CVectorProperty */
	CASTCLASS_CRotatorProperty		= 1 << 8,	/**< Cast to CRotatorProperty */
	CASTCLASS_CAssetProperty		= 1 << 9,	/**< Cast to CAssetProperty */
	CASTCLASS_CArrayProperty		= 1 << 10,	/**< Cast to CArrayProperty */
};

/**
 * @ingroup Core
 * @brief Enumeration property change type
 */
enum EPropertyChangeType
{
    PCT_Unspecified,    /**< Default value. Add new enums to add new functionality */
    PCT_ValueSet        /**< Value set */
};

/**
 * @ingroup Core
 * @brief Structure for passing post edit change events
 */
struct PropertyChangedEvenet
{
    /**
     * @brief Constructor
     *
     * @param InProperty	Property
     * @param InChangeType	Change type
     */
    PropertyChangedEvenet( class CProperty* InProperty, EPropertyChangeType InChangeType = PCT_Unspecified )
        : property( InProperty )
        , changeType( InChangeType )
    {}

    class CProperty*        property;	/**< Changed property */
    EPropertyChangeType		changeType;	/**< Change type */
};

/**
 * @ingroup Core
 * @brief The base class of all objects
 */
class CObject
{
    DECLARE_CLASS( CObject, CObject, CLASS_Abstract, 0 )

public:
	template<typename TClass>   friend bool IsA( CObject* InObject );

    /**
     * @brief Constructor
     */
    CObject();

    /**
     * @brief Constructor
     * @param InName    Object name
     */
    CObject( const CName& InName );

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
     * @return Return allocated object, but it not initialized
     */
    static CObject* StaticAllocateObject( class CClass* InClass, CObject* InOuter = nullptr, CName InName = NAME_None );

    /**
     * @brief Create a new instance of an object
     *
     * @param Class		The class of the object to create
     * @param InOuter   The object to create this object within
     * @param InName	The name to give the new object
     * @return Return allocated object, it will be fully initialized
     */
    static CObject* StaticConstructObject( class CClass* InClass, CObject* InOuter = nullptr, CName InName = NAME_None );

    /**
     * @brief Serialize object
     * @param[in] InArchive Archive for serialize
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
     * @brief Add class property
     * @param InProperty	Property
     */
    virtual void AddProperty( class CProperty* InProperty );

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
     * @brief Get the object this object is in
     * @return Return the object this object is in (can be NULL)
     */
	FORCEINLINE CObject* GetOuter() const
	{
		return outer;
	}

private:
    /**
     * @brief Whether the object is of the specified class
     * @param[in] InClass Checking class
     * 
     * @return Return true if object is a class InClass, else returning false
     */
    bool InternalIsA( const CClass* InClass ) const;

    CName           name;       /**< Name object */ 
    CObject*        outer;      /**< Object this object resides in */
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
