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

#include "System/BaseArchive.h"

#define DECLARE_CLASS( TClass, TSuperClass ) \
    private: \
        static class LClass*        staticClass; \
    public: \
	    typedef TClass		        ThisClass; \
	    typedef TSuperClass	        Super; \
        static LObject*             StaticConstructor(); \
        static class LClass*        StaticClass(); \
        virtual class LClass*       GetClass() const;

#define IMPLEMENT_CLASS( TClass ) \
    LClass*     TClass::staticClass = nullptr; \
    LObject*    TClass::StaticConstructor() \
    { \
        return new ThisClass(); \
    } \
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
    LClass*     TClass::GetClass() const \
    { \
        return StaticClass(); \
    } \
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
    virtual void                    Serialize( BaseArchive& InArchive );

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
    static FORCEINLINE TClass*      StaticNewObject( const LClass* InClass )
    {
        return InClass->CreateObject< TClass >();
    }

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

#endif // !OBJECT_H
