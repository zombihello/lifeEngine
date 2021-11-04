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

/**
 * @ingroup Core
 * @brief Macro for declare class
 * 
 * @param[in] TClass Class
 * @param[in] TSuperClass Super class
 * 
 * Example usage: @code DECLARE_CLASS( LClass, LObject ) @endcode
 */
#define DECLARE_CLASS( TClass, TSuperClass ) \
    private: \
        static class LClass*        staticClass; \
    public: \
	    typedef TClass		        ThisClass; \
	    typedef TSuperClass	        Super; \
        static LObject*             StaticConstructor(); \
        static class LClass*        StaticClass(); \
        virtual class LClass*       GetClass() const;

/**
 * @ingroup Core
 * @brief Macro for implement class
 * 
 * @param[in] TClass Class
 * 
 * Example usage: @code IMPLEMENT_CLASS( LObject ) @endcode
 */
#define IMPLEMENT_CLASS( TClass ) \
    LClass*     TClass::staticClass = nullptr; \
    LObject*    TClass::StaticConstructor() \
    { \
        return new ThisClass(); \
    } \
    \
    LClass*     TClass::StaticClass() \
    { \
        if ( !staticClass ) \
        { \
            bool        isBaseClass = &ThisClass::StaticClass == &Super::StaticClass; \
            staticClass = new LClass( TEXT( #TClass ), &ThisClass::StaticConstructor, !isBaseClass ? Super::StaticClass() : nullptr ); \
        } \
        \
        return staticClass; \
    } \
    \
    LClass*     TClass::GetClass() const \
    { \
        return StaticClass(); \
    } \
    \
    struct SRegister##TClass \
    { \
        SRegister##TClass() \
        { \
            LClass::StaticRegisterClass( TClass::StaticClass() ); \
        } \
    } GRegister##TClass;

/**
 * @ingroup Core
 * @brief The base class of all objects
 */
class LObject
{
    DECLARE_CLASS( LObject, LObject )

public:
    /**
     * @brief Destructor
     */
    virtual                         ~LObject() {}

    /**
     * @brief Serialize object
     * @param[in] InArchive Archive for serialize
     */
    virtual void                    Serialize( class FArchive& InArchive );

    /**
     * @brief Set name object
     * @param[in] InName Name object
     */
    FORCEINLINE void                SetName( const tchar* InName )
    {
        name = InName;
    }

    /**
     * @brief Is the object a class TClass
     * @return Return true if object is a class TClass, else returning false
     */
    template< typename TClass >
    FORCEINLINE bool                IsA() const
    {
        return InternalIsA( TClass::StaticClass() );
    }

    /**
     * @brief Cast object to class a TClass
     * @return Return pointer with type TClass. If object not is the class TClass returning nullptr
     */
	template< typename TClass >
    FORCEINLINE TClass*             Cast()
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
    FORCEINLINE const TClass*       ConstCast() const
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
    FORCEINLINE const tchar*        GetName() const
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
    static FORCEINLINE LObject*      StaticNewObject( const LClass* InClass )
    {
        check( InClass );
        return InClass->CreateObject< TClass >();
    }

    /**
     * Overload operator of serialization in archive
     */
    friend FArchive& operator<<( FArchive& InArchive, LObject& InObject );

private:
    /**
     * @brief Whether the object is of the specified class
     * @param[in] InClass Checking class
     * 
     * @return Return true if object is a class InClass, else returning false
     */
    bool                            InternalIsA( const LClass* InClass ) const;

    std::wstring            name;               /**< Name object */
};

/**
 * @ingroup Core
 * @brief Overload operator << for serialize LObject
 */
FORCEINLINE FArchive& operator<<( FArchive& InArchive, LObject& InObject )
{
    InObject.Serialize( InArchive );
    return InArchive;
}

#endif // !OBJECT_H
