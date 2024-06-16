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
 * @brief Macro for private default constructor
 * @param TClass    Class
 */
#define NO_DEFAULT_CONSTRUCTOR( TClass ) \
	protected: \
        TClass() {} \
    public:

/**
 * @ingroup Core
 * @brief Base macro for declare class
 * 
 * @param TClass            Class
 * @param TSuperClass       Super class
 * @param TClassFlags       Class flags
 * @param TClassCastFlags   Class cast flags
 * @param TPackage          Package
 * 
 * Example usage: @code DECLARE_BASE_CLASS( CClass, CObject, 0, 0, TEXT( "Core" ) ) @endcode
 */
#define DECLARE_BASE_CLASS( TClass, TSuperClass, TClassFlags, TClassCastFlags, TPackage ) \
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
        static FORCEINLINE const tchar*         StaticPackage() \
        { \
            /** Returns the package this class belongs in */ \
            return TPackage; \
        } \
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
 * @brief Macro for declare enum
 *
 * @param TEnum         Enum
 * @param TPackage      Package
 *
 * Example usage: @code DECLARE_ENUM( ESpriteType, TEXT( "Core" ) ) @endcode
 */
#define DECLARE_ENUM( TEnum, TPackage ) \
	namespace Enum \
	{ \
		namespace TEnum \
		{ \
			CEnum* StaticEnum(); \
			FORCEINLINE const tchar* StaticPackage() \
			{ \
				/** Returns the package this enum belongs in */ \
				return TPackage; \
			} \
		} \
	}

/**
 * @ingroup Core
 * @brief Macro for declare struct
 * 
 * @param TStruct           Struct
 * @param TSuperStruct      Super struct
 * @param TPackage          Package
 * 
 * Example usage: @code DECLARE_STRUCT( MyStruct, MyStruc, TEXT( "Core" ) ) @endcode
 */
#define DECLARE_STRUCT( TStruct, TSuperStruct, TPackage ) \
    private: \
        static class CStruct*					staticStruct; \
		static class CStruct*					GetStaticStruct(); \
        static void								InternalInitializeStruct(); \
    public: \
	    typedef TStruct							ThisStruct; \
	    typedef TSuperStruct					Super; \
        static void								StaticInitializeStruct(); \
		static FORCEINLINE const tchar*         StaticPackage() \
        { \
            /** Returns the package this struct belongs in */ \
            return TPackage; \
        } \
        static FORCEINLINE class CStruct*       StaticStruct() \
		{ \
			if ( !staticStruct ) \
            { \
                staticStruct = GetStaticStruct(); \
                InternalInitializeStruct(); \
            } \
            return staticStruct; \
		}

/**
 * @ingroup Core
 * @brief Macro for declare class
 *
 * @param TClass            Class
 * @param TSuperClass       Super class
 * @param TClassFlags       Class flags
 * @param TClassCastFlags   Class cast flags
 * @param TPackage          Package
 *
 * Example usage: @code DECLARE_CLASS( CClass, CObject, 0, 0, TEXT( "Core" ) ) @endcode
 */
#define DECLARE_CLASS( TClass, TSuperClass, TClassFlags, TClassCastFlags, TPackage ) \
    DECLARE_BASE_CLASS( TClass, TSuperClass, TClassFlags, TClassCastFlags, TPackage ) \
    DECLARE_SERIALIZER_AND_DTOR( TClass )

/**
 * @ingroup Core
 * @brief Macro for declare class without default constructor
 *
 * @param TClass            Class
 * @param TSuperClass       Super class
 * @param TClassFlags       Class flags
 * @param TClassCastFlags   Class cast flags
 * @param TPackage          Package
 *
 * Example usage: @code DECLARE_CLASS_NO_CTOR( CClass, CObject, 0, 0, TEXT( "Core" ) ) @endcode
 */
#define DECLARE_CLASS_NO_CTOR( TClass, TSuperClass, TClassFlags, TClassCastFlags, TPackage ) \
    DECLARE_BASE_CLASS( TClass, TSuperClass, TClassFlags, TClassCastFlags, TPackage ) \
    DECLARE_SERIALIZER_AND_DTOR( TClass ) \
    NO_DEFAULT_CONSTRUCTOR( TClass )

/**
 * @ingroup Core
 * @brief Macro for declare intrinsic class
 * 
 * @param TClass            Class
 * @param TSuperClass       Super class
 * @param TClassFlags       Class flags
 * @param TClassCastFlags   Class cast flags
 *
 * Example usage: @code DECLARE_CLASS_INTRINSIC( CClass, CObject, 0, 0, TEXT( "Core" ) ) @endcode
 */
#define DECLARE_CLASS_INTRINSIC( TClass, TSuperClass, TClassFlags, TClassCastFlags, TPackage ) \
    DECLARE_BASE_CLASS( TClass, TSuperClass, TClassFlags | CLASS_Intrinsic, TClassCastFlags, TPackage ) \
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
        return ::new( InPtr ) ThisClass; \
    } \
    CClass* TClass::GetStaticClass() \
    { \
        CClass*     returnClass = ::new CClass \
        ( \
            NativeConstructor, \
            TEXT( #TClass ), \
            StaticPackage(), \
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
        ThisClass::StaticInitializeClass(); \
    } \
    struct Register##TClass \
    { \
        Register##TClass() \
        { \
            CObject::AddToAutoInitializeRegistrants( ( CObject* (*)() )&TClass::StaticClass ); \
        } \
    }; \
    static Register##TClass s_Register##TClass;

/**
 * @ingroup Core
 * @brief Macro for implement enum
 *
 * @param TEnum			Enum
 * @param TForEachEnum	Macro of for each enum
 *
 * Example usage: @code IMPLEMENT_ENUM( ESpriteType, FOREACH_ENUM_SPRITETYPE ) @endcode
 */
#define IMPLEMENT_ENUM( TEnum, TForEachEnum ) \
	namespace Enum \
	{ \
		namespace TEnum \
		{ \
			CEnum* StaticEnum() \
			{ \
				static CEnum*	s_CEnum = nullptr; \
				if ( !s_CEnum ) \
				{ \
					std::vector<CName>		enums; \
					TForEachEnum( INTERNAL_REGISTER_ENUM ) \
					s_CEnum = ::new CEnum( NativeConstructor, TEXT( #TEnum ), StaticPackage(), enums ); \
					s_CEnum->SetClass( CEnum::StaticClass() ); \
				} \
				return s_CEnum; \
			} \
		} \
	} \
    struct Register##TEnum \
    { \
        Register##TEnum() \
        { \
            CObject::AddToAutoInitializeRegistrants( ( CObject* (*)() )&Enum::TEnum::StaticEnum ); \
        } \
    }; \
	static Register##TEnum s_Register##TEnum;

/**
 * @ingroup Core
 * @brief Macro for implement struct
 * 
 * @param TStruct    Struct
 * 
 * Example usage: @code IMPLEMENT_STRUCT( MyStruct ) @endcode
 */
#define IMPLEMENT_STRUCT( TStruct ) \
    CStruct* TStruct::staticStruct = nullptr; \
	CStruct* TStruct::GetStaticStruct() \
	{ \
		CStruct*	returnStruct = ::new CStruct \
		( \
			NativeConstructor, \
            TEXT( #TStruct ), \
            sizeof( ThisStruct ), \
            alignof( ThisStruct ), \
			StaticPackage() \
		); \
		Assert( returnStruct ); \
		return returnStruct; \
	} \
	void TStruct::InternalInitializeStruct() \
	{ \
		staticStruct->SetSuperStruct( Super::StaticStruct() != staticStruct ? Super::StaticStruct() : nullptr ); \
		staticStruct->SetClass( CStruct::StaticClass() ); \
        ThisStruct::StaticInitializeStruct(); \
	} \
	struct Register##TStruct \
    { \
        Register##TStruct() \
        { \
            CObject::AddToAutoInitializeRegistrants( ( CObject* (*)() )&TStruct::StaticStruct ); \
        } \
    }; \
    static Register##TStruct s_Register##TStruct;

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
 * @brief Macro for implement default StaticInitializeStruct()
 *
 * @param TStruct    Struct
 *
 * Example usage: @code IMPLEMENT_DEFAULT_INITIALIZE_STRUCT( MyStruct ) @endcode
 */
#define IMPLEMENT_DEFAULT_INITIALIZE_STRUCT( TStruct ) \
        void TStruct::StaticInitializeStruct() {}

/**
 * @ingroup Core
 * @brief Internal register enum
 * 
 * @param InName	Name
 */
#define INTERNAL_REGISTER_ENUM( InName ) \
	enums.push_back( CName( TEXT( #InName ) ) );

/**
 * @ingroup Core
 * @brief Macro for define Cpp properties
 * 
 * @param InClass	Class name
 * @param InName	Property name
 */
#define CPP_PROPERTY( InClass, InName )	        CppProperty, STRUCT_OFFSET( InClass, InName )

/**
 * @ingroup Core
 * @brief Special canonical package for FindObject
 */
#define ANY_PACKAGE     ( ( CObjectPackage* )-1 )

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
	CLASS_None			    = 0,		                                                    /**< None */
	CLASS_Deprecated	    = 1 << 0,	                                                    /**< Class is deprecated */
	CLASS_Abstract		    = 1 << 1,	                                                    /**< Class is abstract and can't be instantiated directly  */
    CLASS_Native            = 1 << 2,                                                       /**< Class is native */
    CLASS_Transient         = 1 << 3,                                                       /**< This object type can't be saved; null it out at save time */
    CLASS_HasComponents     = 1 << 4,                                                       /**< Class has component properties */
    CLASS_Intrinsic         = 1 << 5,                                                       /**< Class has no LifeScript counter-part */
    CLASS_Parsed            = 1 << 6,                                                       /**< Class script has been successfully parsed */
    CLASS_Exported          = 1 << 7,                                                       /**< Class has been exported to a header file */

    // Combination masks and other combinations
    CLASS_MASK_Inherit      = CLASS_Deprecated | CLASS_Transient | CLASS_HasComponents | CLASS_Intrinsic      /**< Flags to inherit from base class */
};

/**
 * @ingroup Core
 * @brief Enumeration of flags used for quickly casting classes of certain types; all class cast flags are inherited
 */
enum EClassCastFlags
{
	CASTCLASS_None					= 0,		/**< None */
    CASTCLASS_CEnum                 = 1 << 0,   /**< Cast to CEnum */
    CASTCLASS_CStruct               = 1 << 1,   /**< Cast to CStruct */
    CASTCLASS_CClass                = 1 << 2,   /**< Cast to CClass */
	CASTCLASS_CProperty				= 1 << 3,	/**< Cast to CProperty */
	CASTCLASS_CByteProperty			= 1 << 4,	/**< Cast to CByteProperty */
	CASTCLASS_CIntProperty			= 1 << 5,	/**< Cast to CIntProperty */
	CASTCLASS_CFloatProperty		= 1 << 6,	/**< Cast to CFloatProperty */
	CASTCLASS_CBoolProperty			= 1 << 7,	/**< Cast to CBoolProperty */
	CASTCLASS_CColorProperty		= 1 << 8,	/**< Cast to CColorProperty */
	CASTCLASS_CObjectProperty		= 1 << 9,	/**< Cast to CObjectProperty */
	CASTCLASS_CVectorProperty		= 1 << 10,	/**< Cast to CVectorProperty */
	CASTCLASS_CRotatorProperty		= 1 << 11,	/**< Cast to CRotatorProperty */
	CASTCLASS_CAssetProperty		= 1 << 12,	/**< Cast to CAssetProperty */
	CASTCLASS_CArrayProperty		= 1 << 13,	/**< Cast to CArrayProperty */
    CASTCLASS_CStructProperty       = 1 << 14,	/**< Cast to CStructProperty */
    CASTCLASS_CStringProperty       = 1 << 15,	/**< Cast to CStringProperty */
};

/**
 * @ingroup Core
 * @brief Enumeration of object flags
 */
enum EObjectFlags
{
    OBJECT_None                     = 0,        /**< None */
    OBJECT_Native                   = 1 << 0,   /**< Native (CFields only) */
    OBJECT_RootSet                  = 1 << 1,   /**< Object will not be garbage collected, even if unreferenced */
    OBJECT_BeginDestroyed           = 1 << 2,   /**< BeginDestroy has been called on the object */
    OBJECT_FinishDestroyed          = 1 << 3,   /**< FinishDestroy has been called on the object */
    OBJECT_Unreachable              = 1 << 4,   /**< Object is not reachable on the object graph */
    OBJECT_PendingKill              = 1 << 5,   /**< Objects that are pending destruction */
	OBJECT_TagImp                   = 1 << 6,   /**< Temporary import tag in load/save */
	OBJECT_TagExp                   = 1 << 7,   /**< Temporary export tag in load/save */
    OBJECT_Transient                = 1 << 8,   /**< Don't save object */
    OBJECT_NeedLoad                 = 1 << 9,   /**< During load, indicates object needs loading */
    OBJECT_NeedPostLoad             = 1 << 10,  /**< Object needs to be postloaded */
    OBJECT_Public                   = 1 << 11,  /**< Object is visible outside its package */
    OBJECT_WasLoaded                = 1 << 12,  /**< Flagged on CObjects that were loaded */

    // Combination masks and other combinations
    OBJECT_MASK_Load                 = OBJECT_Public | OBJECT_Native,       /**< Flags to load from LifeEngine files */
    OBJECT_MASK_Keep                 = OBJECT_Native | OBJECT_RootSet       /**< Flags to persist across loads */
};

/**
 * @ingroup Core
 * @brief Package flags
 */
enum EPackageFlags
{
    PKG_None                    = 0,                                    /**< No flags */
    PKG_NewlyCreated            = 1 << 0,                               /**< Newly created package, not saved yet */
    PKG_CompiledIn              = 1 << 1,                               /**< This package is from "compiled in" classes */
    PKG_FilterEditorOnly        = 1 << 2,                               /**< Package has editor-only data filtered out */
    PKG_EditorOnly              = 1 << 3,                               /**< This is editor-only package */
    PKG_Cooked                  = 1 << 4,                               /**< Package is cooked */
    PKG_StoreCompressed         = 1 << 5,                               /**< Package is being stored compressed, requires archive support for compression */

    // Combination masks and other combinations
    PKG_MASK_TransientFlags     = PKG_NewlyCreated,                     /**< Transient flags are cleared when serializing to or from PackageFileSummary */
    PKG_MASK_InMemoryOnly       = PKG_NewlyCreated | PKG_CompiledIn     /**< Indicates if this package is a package that exists in memory only */
};

/**
 * @ingroup Core
 * @brief Flags for loading objects/packages
 */
enum ELoadFlags
{
    LOAD_None           = 0,        /**< No flags */
    LOAD_FindIfFail     = 1 << 0    /**< Tries FindObject if a linker cannot be obtained */
};

/**
 * @ingroup Core
 * @brief Flags for saving objects/packages
 */
enum ESaveFlags
{
    SAVE_None       = 0,        /**< No flags */
    SAVE_KeepDirty  = 1 << 0,   /**< Do not clear the dirty flag when saving */
};

/**
 * @ingroup Core
 * @brief Flags for rename object
 */
enum ERenameFlags
{
    REN_None                    = 0,        /**< No flags */
    REN_Test                    = 1 << 0,   /**< Just test to make sure that the rename is guaranteed to succeed if an non test rename immediately follows */
    REN_ForceNoResetLoaders     = 1 << 1,   /**< Rename won't call ResetLoaders. You should pass this if you are renaming a deep subobject and do not need to reset loading for the outer package */
    REN_DoNotDirty              = 1 << 2,   /**< Indicates that the object (and new outer) should not be dirtied */
    REN_DontCreateRedirectors   = 1 << 3    /**< Don't create an object redirector, even if the class is marked OBJECT_Public */
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
 * @brief Enumeration property flags
 */
enum EPropertyFlags
{
	CPF_None			= 0,		/**< None */
	CPF_Const			= 1 << 0,	/**< Property is constant */
	CPF_EditorOnly		= 1 << 1,	/**< Property should only be loaded in the editor */
	CPF_Edit			= 1 << 2,	/**< Property is user-settable in the editor */
	CPF_EditFixedSize	= 1 << 3,	/**< Indicates that elements of an array can be modified, but its size cannot be changed */
	CPF_EditConst		= 1 << 4,	/**< Property is uneditable in the editor */
	CPF_Deprecated		= 1 << 5,	/**< Property is deprecated. Read it from an archive, but don't save it */
	CPF_Transient		= 1 << 6,	/**< Property is transient: shouldn't be saved or loaded */
	CPF_SaveGame		= 1 << 7	/**< Property should be serialized for save games, this is only checked for game-specific archives with CArchive::arIsSaveGame */
};

#if WITH_EDITOR
/**
 * @ingroup Core
 * @brief Enumeration property exporting flags
 */
enum EPropertyPortFlags
{
	PPF_None					= 0,		/**< No special property exporting flags */
	PPF_IncludeTransient		= 1 << 0,	/**< Indicates that non-categorized transient properties should be exported (by default, they would not be) */
	PPF_UseDeprecatedProperties	= 1 << 1,	/**< Ignores CPF_Deprecated flag */
	PPF_PropertyWindow			= 1 << 2	/**< Indicates that we're exporting properties for display in the property window */
};
#endif // WITH_EDITOR

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