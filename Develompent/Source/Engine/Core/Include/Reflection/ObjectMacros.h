/**
 * @file
 * @addtogroup Core Core
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef OBJECTMACROS_H
#define OBJECTMACROS_H

#include "Core.h"
#include "Misc/ScriptOpcodes.h"

/**
 * @ingroup Core
 * @brief Macro for declare serializer interface and destructor for a class
 *
 * @param TClass            Class
 *
 * Example usage: @code DECLARE_SERIALIZER_AND_DTOR( CClass ) @endcode
 */
#define DECLARE_SERIALIZER_AND_DTOR( TClass ) \
    public: \
	    friend CArchive& operator<<( CArchive& InArchive, TClass*& InValue ) \
        { \
            return InArchive << *( CObject** )&InValue; \
        } \
    protected: \
        virtual ~TClass() \
        { \
            ConditionalDestroy(); \
        } \
    public:

/**
 * @ingroup Core
 * @brief Base macro for declare class
 * 
 * @param TClass            Class
 * @param TSuperClass       Super class
 * @param TClassFlags       Class flags
 * @param TClassCastFlags   Class cast flags
 * 
 * Example usage: @code DECLARE_BASE_CLASS( CClass, CObject, 0, 0 ) @endcode
 */
#define DECLARE_BASE_CLASS( TClass, TSuperClass, TClassFlags, TClassCastFlags ) \
    private: \
        static class CClass*                    staticClass; \
        static class CClass*                    GetStaticClass(); \
        static void                             InternalInitializeClass(); \
    public: \
        enum { StaticClassFlags=TClassFlags | CLASS_Native }; \
        enum { StaticClassCastFlags=TClassCastFlags }; \
	    typedef TClass		                    ThisClass; \
	    typedef TSuperClass	                    Super; \
        static CObject*                         StaticConstructor( void* InPtr ); \
        static void                             StaticInitializeClass(); \
        static FORCEINLINE class CClass*        StaticClass() \
        { \
            if ( !staticClass ) \
            { \
                staticClass = GetStaticClass(); \
                InternalInitializeClass(); \
            } \
            return staticClass; \
        } \
        void* operator new( size_t InSize, CObject* InOuter = nullptr, const CName& InName = NAME_None, ObjectFlags_t InFlags = OBJECT_None ) \
        { \
            return StaticAllocateObject( StaticClass(), InOuter, InName, InFlags ); \
		} \
        void operator delete( void* InPtr, CObject* InOuter, const CName& InName, ObjectFlags_t InFlags ) \
        { \
            AssertNoEntry(); \
        }

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
    DECLARE_BASE_CLASS( TClass, TSuperClass, TClassFlags, TClassCastFlags ) \
    DECLARE_SERIALIZER_AND_DTOR( TClass )

/**
 * @ingroup Core
 * @brief Macro for declare intrinsic class
 * 
 * @param TClass            Class
 * @param TSuperClass       Super class
 * @param TClassFlags       Class flags
 * @param TClassCastFlags   Class cast flags
 *
 * Example usage: @code DECLARE_CLASS_INTRINSIC( CClass, CObject, 0, 0 ) @endcode
 */
#define DECLARE_CLASS_INTRINSIC( TClass, TSuperClass, TClassFlags, TClassCastFlags ) \
    DECLARE_BASE_CLASS( TClass, TSuperClass, TClassFlags | CLASS_Intrinsic, TClassCastFlags ) \
    DECLARE_SERIALIZER_AND_DTOR( TClass )

/**
 * @ingroup Core
 * @brief Declare that objects of class being defined reside within objects of the specified class
 * @param TWithinClass    Within class
 */
#define DECLARE_WITHIN_CLASS( TWithinClass ) \
	typedef class TWithinClass      WithinClass; \
	FORCEINLINE TWithinClass* GetOuter##TWithinClass() const \
    { \
        return ( TWithinClass* )GetOuter(); \
    }

/**
 * @ingroup Core
 * @brief Macro declare function for callable in/from LifeScript
 *
 * @param TFunction     Function
 */
#define DECLARE_FUNCTION( TFunction ) \
    void TFunction( ScriptFrame& InStack )

/**
 * @ingroup Core
 * @brief Macro declare function for register native functions
 */
#define DECLARE_REGISTER_NATIVES() \
    static void StaticRegisterNatives()

/**
 * @ingroup Core
 * @brief Macro for private default constructor
 * @param TClass    Class
 */
#define NO_DEFAULT_CONSTRUCTOR( TClass ) \
	protected: \
        TClass() {} \
    public:

/**
 * @ingroup Core
 * @brief Macro for implement class
 * 
 * @param TClass    Class
 * 
 * Example usage: @code IMPLEMENT_CLASS( CObject ) @endcode
 */
#define IMPLEMENT_CLASS( TClass ) \
    CClass* TClass::staticClass = nullptr; \
    CObject* TClass::StaticConstructor( void* InPtr ) \
    { \
        return ::new( InPtr ) ThisClass(); \
    } \
    CClass* TClass::GetStaticClass() \
    { \
        CClass*     returnClass = ::new CClass \
        ( \
            NativeConstructor, \
            TEXT( #TClass ), \
            StaticClassFlags, \
            StaticClassCastFlags, \
            sizeof( ThisClass ), \
            alignof( ThisClass ), \
            &ThisClass::StaticConstructor \
        ); \
        Assert( returnClass ); \
        return returnClass; \
    } \
    void TClass::InternalInitializeClass() \
    { \
        staticClass->SetSuperClass( Super::StaticClass() != staticClass ? Super::StaticClass() : nullptr ); \
        staticClass->SetClass( CClass::StaticClass() ); \
        staticClass->SetWithinClass( WithinClass::StaticClass() ); \
        CObjectGC::Get().AddObject( staticClass ); \
        ThisClass::StaticRegisterNatives(); \
        ThisClass::StaticInitializeClass(); \
    } \
    struct Register##TClass \
    { \
        Register##TClass() \
        { \
            CReflectionEnvironment::Get().AddClass( TClass::StaticClass() ); \
        } \
    }; \
    static Register##TClass s_Register##TClass;

/**
 * @ingroup Core
 * @brief Macro for implement default StaticInitializeClass()
 *
 * @param TClass    Class
 *
 * Example usage: @code IMPLEMENT_DEFAULT_INITIALIZE_CLASS( CObject ) @endcode
 */
#define IMPLEMENT_DEFAULT_INITIALIZE_CLASS( TClass ) \
        void TClass::StaticInitializeClass() {}

/**
 * @ingroup Core
 * @brief Object flags type
 */
typedef uint32          ObjectFlags_t;

/**
 * @ingroup Core
 * @brief Enumeration of flags describing a class
 */
enum EClassFlags
{
	CLASS_None			= 0,		/**< None */
	CLASS_Deprecated	= 1 << 0,	/**< Class is deprecated */
	CLASS_Abstract		= 1 << 1,	/**< Class is abstract and can't be instantiated directly  */
    CLASS_Native        = 1 << 2,   /**< Class is native */
    CLASS_Intrinsic     = 1 << 3,   /**< Class has no LifeScript counter-part */
    CLASS_Parsed        = 1 << 4,   /**< Script has been successfully parsed */
    CLASS_Compiled      = 1 << 5    /**< Script has been compiled successfully */
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
    CASTCLASS_CStructProperty       = 1 << 11,	/**< Cast to CStructProperty */
    CASTCLASS_CStringProperty       = 1 << 12,	/**< Cast to CStringProperty */
};

/**
 * @ingroup Core
 * @brief Enumeration of object flags
 */
enum EObjectFlags
{
    OBJECT_None             = 0,        /**< None */
    OBJECT_Native           = 1 << 0,   /**< Native */
    OBJECT_RootSet          = 1 << 1,   /**< Object will not be garbage collected, even if unreferenced */
    OBJECT_DisregardForGC   = 1 << 2,   /**< Object is being disregard for GC */
    OBJECT_BeginDestroyed   = 1 << 3,   /**< BeginDestroy has been called on the object */
    OBJECT_FinishDestroyed  = 1 << 4,   /**< FinishDestroy has been called on the object */
    OBJECT_Unreachable      = 1 << 5,   /**< Object is not reachable on the object graph */
    OBJECT_PendingKill      = 1 << 6,   /**< Objects that are pending destruction */

    // DEPRECATED
    OBJECT_NeedSave         = 1 << 7,   /**< DEPRECATED. Mark a object what need to save it by CObjectPackage */
    OBJECT_NeedDestroy      = 1 << 8    /**< DEPRECATED. Mark a object what need to destroy it */
};

/**
 * @ingroup Core
 * @brief Internal enum for native constructors
 */
enum ENativeConstructor
{
    NativeConstructor
};

/**
 * @ingroup Core
 * @brief Internal enum for C++ properties
 */
enum ECppProperty
{
    CppProperty
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

#endif // !OBJECTMACROS_H