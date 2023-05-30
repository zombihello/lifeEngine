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

#include "Misc/Class.h"

#if WITH_EDITOR
#include "Misc/DataMap.h"
#endif // WITH_EDITOR

/**
 * @ingroup Core
 * @brief Macro for declare class
 * 
 * @param[in] TClass Class
 * @param[in] TSuperClass Super class
 * 
 * Example usage: @code DECLARE_CLASS( CClass, CObject ) @endcode
 */
#define DECLARE_CLASS( TClass, TSuperClass ) \
    private: \
        static class CClass*        staticClass; \
    public: \
	    typedef TClass		        ThisClass; \
	    typedef TSuperClass	        Super; \
        static CObject*             StaticConstructor(); \
        static class CClass*        StaticClass(); \
        virtual class CClass*       GetClass() const;

/**
 * @ingroup Core
 * @brief Macro for implement class
 * 
 * @param[in] TClass Class
 * 
 * Example usage: @code IMPLEMENT_CLASS( CObject ) @endcode
 */
#define IMPLEMENT_CLASS( TClass ) \
    CClass*     TClass::staticClass = nullptr; \
    CObject*    TClass::StaticConstructor() \
    { \
        return new ThisClass(); \
    } \
    \
    CClass*     TClass::StaticClass() \
    { \
        if ( !staticClass ) \
        { \
            bool        isBaseClass = &ThisClass::StaticClass == &Super::StaticClass; \
            staticClass = new CClass( TEXT( #TClass ), &ThisClass::StaticConstructor, !isBaseClass ? Super::StaticClass() : nullptr ); \
        } \
        \
        return staticClass; \
    } \
    \
    CClass*     TClass::GetClass() const \
    { \
        return StaticClass(); \
    } \
    \
    struct SRegister##TClass \
    { \
        SRegister##TClass() \
        { \
            CClass::StaticRegisterClass( TClass::StaticClass() ); \
        } \
    } g_Register##TClass;

/**
 * @ingroup Core
 * @brief The base class of all objects
 */
class CObject
{
    DECLARE_CLASS( CObject, CObject )
    DECLARE_DATADESC()

public:
    /**
     * @brief Destructor
     */
    virtual ~CObject() {}

    /**
     * @brief Serialize object
     * @param[in] InArchive Archive for serialize
     */
    virtual void Serialize( class CArchive& InArchive );

    /**
     * @brief Set name object
     * @param[in] InName Name object
     */
    FORCEINLINE void SetName( const tchar* InName )
    {
        name = InName;
    }

    /**
     * @brief Is the object a class TClass
     * @return Return true if object is a class TClass, else returning false
     */
    template< typename TClass >
    FORCEINLINE bool IsA() const
    {
        return InternalIsA( TClass::StaticClass() );
    }

    /**
     * @brief Cast object to class a TClass
     * @return Return pointer with type TClass. If object not is the class TClass returning nullptr
     */
	template< typename TClass >
    FORCEINLINE TClass* Cast()
	{
		if ( IsA< TClass >() )
		{
			return ( TClass* )this;
		}

		return nullptr;
	}

    /**
     * @brief Cast object to class a TClass
     * @return Return const pointer with type TClass. If object not is the class TClass returning nullptr
     */
	template< typename TClass >
    FORCEINLINE const TClass* ConstCast() const
	{
		if ( IsA< TClass >() )
		{
			return ( const TClass* )this;
		}

		return nullptr;
	}

    /**
     * @brief Get name object
     * @return Return name object
     */
    FORCEINLINE const tchar* GetName() const
    {
        return name.c_str();
    }

    /**
     * @brief Create new instance of object with setted class
     * @param[in] InClass Class of object
     * 
     * @return Return pointer to new object
     */
    template< typename TClass >
    static FORCEINLINE CObject* StaticNewObject( const CClass* InClass )
    {
        Assert( InClass );
        return InClass->CreateObject< TClass >();
    }

    /**
     * Overload operator of serialization in archive
     */
    friend CArchive& operator<<( CArchive& InArchive, CObject& InObject );

private:
    /**
     * @brief Whether the object is of the specified class
     * @param[in] InClass Checking class
     * 
     * @return Return true if object is a class InClass, else returning false
     */
    bool InternalIsA( const CClass* InClass ) const;

    std::wstring            name;               /**< Name object */
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
        return InObject->IsA<TClass>();
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

#endif // !OBJECT_H
