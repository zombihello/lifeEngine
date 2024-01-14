/**
 * @file
 * @addtogroup Core Core
 *
 * Copyright BSOD-Games, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef CLASS_H
#define CLASS_H

#include "Reflection/ObjectMacros.h"
#include "Reflection/ObjectGC.h"
#include "Reflection/Struct.h"

/**
 * @ingroup Core
 * @brief Class description for reflection
 */
class CClass : public CStruct
{
	DECLARE_CLASS( CClass, CStruct, 0, 0, TEXT( "Core" ) )

public:
	friend CObject;

	/**
	 * @brief Constructor
	 */
	FORCEINLINE CClass() 
		: bHasAssembledReferenceTokenStream( false )
		, ClassConstructor( nullptr )
		, classFlags( CLASS_None )
		, classCastFlags( CASTCLASS_None )
		, classUnique( 0 )
		, withinClass( nullptr )
	{}

	/**
	 * @brief Constructor
	 * 
	 * @param InClassName			Class name
	 * @param InPackageName			Package name
	 * @param InClassFlags			Class flags
	 * @param InClassCastFlags		Class cast flags
	 * @param InPropertiesSize		Properties size in bytes
	 * @param InMinAlignment		Minimum alignment for the class
	 * @param InClassConstructor	Pointer to class constructor
	 * @param InSuperClass			Super class
	 * @param InWithinClass			Class where this class is within
	 */
	FORCEINLINE CClass( ENativeConstructor, const CName& InClassName, const tchar* InPackageName, uint32 InClassFlags, uint32 InClassCastFlags, uint32 InPropertiesSize, uint32 InMinAlignment, class CObject*( *InClassConstructor )( void* InPtr ), CClass* InSuperClass = nullptr, CClass* InWithinClass = nullptr )
		: CStruct( NativeConstructor, InClassName, InPropertiesSize, InMinAlignment, InPackageName, InSuperClass )
		, bHasAssembledReferenceTokenStream( false )
		, ClassConstructor( InClassConstructor )
		, classFlags( InClassFlags )
		, classCastFlags( InClassCastFlags )
		, classUnique( 0 )
		, withinClass( InWithinClass )
	{}

	/**
	 * @brief Constructor
	 *
	 * @param InClassFlags			Class flags
	 * @param InClassCastFlags		Class cast flags
	 * @param InPropertiesSize		Properties size in bytes
	 * @param InMinAlignment		Minimum alignment for the class
	 * @param InClassConstructor	Pointer to class constructor
	 * @param InSuperClass			Super class
	 * @param InWithinClass			Class where this class is within
	 */
	FORCEINLINE CClass( uint32 InClassFlags, uint32 InClassCastFlags, uint32 InPropertiesSize, uint32 InMinAlignment, CClass* InSuperClass = nullptr, CClass* InWithinClass = nullptr )
		: CStruct( InPropertiesSize, InMinAlignment, InSuperClass )
		, bHasAssembledReferenceTokenStream( false )
		, ClassConstructor( nullptr )
		, classFlags( InClassFlags )
		, classCastFlags( InClassCastFlags )
		, classUnique( 0 )
		, withinClass( InWithinClass )
	{}

	/**
	 * @brief Serialize object
	 * @param InArchive     Archive for serialize
	 */
	virtual void Serialize( class CArchive& InArchive ) override;

	/**
	 * @brief Find the class's native constructor and assembly the token stream for realtime garbage collection
	 */
	virtual void Bind() override;

	/**
	 * @brief Realtime garbage collection helper function used to emit token containing information about a
	 * direct CObject reference at the passed in offset
	 *
	 * @param InOffset	Offset into object at which object reference is stored
	 */
	FORCEINLINE void EmitObjectReference( uint32 InOffset )
	{
		referenceTokenStream.EmitReferenceInfo( GCReferenceInfo( GCRT_Object, InOffset ) );
	}

	/**
	 * @brief Realtime garbage collection helper function used to emit token containing information about a
	 * an array of CObject references at the passed in offset
	 *
	 * @param InOffset	Offset into object at which array of objects is stored
	 */
	FORCEINLINE void EmitObjectArrayReference( uint32 InOffset )
	{
		referenceTokenStream.EmitReferenceInfo( GCReferenceInfo( GCRT_ArrayObject, InOffset ) );
	}

	/**
	 * @brief Set super class
	 * @param InSuperClass	Super class
	 */
	FORCEINLINE void SetSuperClass( CClass* InSuperClass )
	{
		SetSuperStruct( ( CStruct* )InSuperClass );
	}

	/**
	 * @brief Get super class
	 * @return Return pointer to super class. If it is not, it will return nullptr
	 */
	FORCEINLINE CClass* GetSuperClass() const
	{
		return ( CClass* )GetSuperStruct();
	}

	/**
	 * @brief Get a class where this class is within
	 * @return Return pointer to class where this class is within
	 */
	FORCEINLINE CClass* GetWithinClass() const
	{
		return withinClass;
	}

	/**
	 * @brief Set a class where this class is within
	 * @param InWithinClass		Class where this class is within
	 */
	FORCEINLINE void SetWithinClass( CClass* InWithinClass )
	{
		withinClass = InWithinClass;
	}

	/**
	 * @brief Set class flags
	 * @param InNewFlags	New flags
	 */
	FORCEINLINE void SetClassFlags( uint32 InNewFlags )
	{
		classFlags = InNewFlags;
	}

	/**
	 * @brief Add class flag
	 * @param InNewFlag		New flag
	 */
	FORCEINLINE void AddClassFlag( uint32 InNewFlag )
	{
		classFlags |= InNewFlag;
	}

	/**
	 * @brief Remove class flag
	 * @param InFlag	Flag to remove
	 */
	FORCEINLINE void RemoveClassFlag( uint32 InFlag )
	{
		classFlags &= ~InFlag;
	}

	/**
	 * @brief Get class flags
	 * @return Return class flags
	 */
	FORCEINLINE uint32 GetClassFlags() const
	{
		return classFlags;
	}

	/**
	 * @brief Has any class flags
	 * 
	 * @param InCheckFlags		Class flags to check for
	 * @return Return TRUE if the passed in flag is set (including no flag passed in), otherwise FALSE
	 */
	FORCEINLINE bool HasAnyClassFlags( uint32 InCheckFlags ) const
	{
		return ( classFlags & InCheckFlags ) != 0;
	}

	/**
	 * @brief Has all class flags
	 *
	 * @param InCheckFlags		Class flags to check for
	 * @return Return TRUE if all of the passed in flags are set (including no flags passed in), otherwise FALSE
	 */
	FORCEINLINE bool HasAllClassFlags( uint32 InCheckFlags ) const
	{
		return ( classFlags & InCheckFlags ) == InCheckFlags;
	}

	/**
	 * @brief Has any cast flags
	 *
	 * @param InCheckFlags		Class flags to check for
	 * @return Return TRUE if the passed in flag is set (including no flag passed in), otherwise FALSE
	 */
	FORCEINLINE bool HasAnyCastFlags( uint32 InCheckFlags ) const
	{
		return ( classCastFlags & InCheckFlags ) != 0;
	}

	/**
	 * @brief Has all cast flags
	 *
	 * @param InCheckFlags		Class flags to check for
	 * @return Return TRUE if all of the passed in flags are set (including no flags passed in), otherwise FALSE
	 */
	FORCEINLINE bool HasAllCastFlags( uint32 InCheckFlags ) const
	{
		return ( classCastFlags & InCheckFlags ) == InCheckFlags;
	}

	/**
	 * @brief Get class cast flags
	 * @return Return class cast flags
	 */
	FORCEINLINE uint32 GetClassCastFlags() const
	{
		return classCastFlags;
	}

	/**
	 * @brief Create instance of class object
	 * 
	 * @param InOuter	Object this object resides in
	 * @param InName	Name object
	 * @param InFlags   The object flags
	 * @return Return pointer to instance of class object
	 */
	FORCEINLINE class CObject* CreateObject( class CObject* InOuter = nullptr, const CName& InName = NAME_None, ObjectFlags_t InFlags = OBJECT_None ) const
	{
		return CObject::StaticConstructObject( ( CClass* )this, InOuter, InName, InFlags );
	}

	/**
	 * @brief Create instance of class object
	 * 
	 * @param InOuter	Object this object resides in
	 * @param InName	Name object
	 * @param InFlags   The object flags
	 * @return Return pointer to instance of class object
	 */
	template<typename TObject>
	FORCEINLINE TObject* CreateObject( class CObject* InOuter = nullptr, const CName& InName = NAME_None, ObjectFlags_t InFlags = OBJECT_None ) const
	{
		return ( TObject* )CreateObject( InOuter, InName, InFlags );
	}

	/**
	 * @brief Assembles the token stream for realtime garbage collection
	 * Assembles the token stream for realtime garbage collection by combining the per class only
	 * token stream for each class in the class hierarchy. This is only done once and duplicate
	 * work is avoided by using an object flag
	 */
	void AssembleReferenceTokenStream();

	/**
	 * @brief Is assembled reference token stream
	 * @return Return TRUE if the reference token stream of this class has assembled, otherwise returns FALSE
	 */
	FORCEINLINE bool IsAssembledReferenceTokenStream() const
	{
		return bHasAssembledReferenceTokenStream;
	}

	/**
	 * @brief Get reference token stream
	 * @return Return reference token stream
	 */
	FORCEINLINE const CGCReferenceTokenStream& GetReferenceTokenStream() const
	{
		return referenceTokenStream;
	}

private:
	bool								bHasAssembledReferenceTokenStream;		/**< Indicates whether the reference token stream has assembled*/
	class CObject*( *ClassConstructor )( void* InPtr );							/**< Pointer to constructor of class */
	uint32								classFlags;								/**< Class flags */
	uint32								classCastFlags;							/**< Class cast flags */
	uint32								classUnique;							/**< Class pseudo-unique counter; used to accelerate unique instance name generation */
	CClass*								withinClass;							/**< Class within */
	CGCReferenceTokenStream				referenceTokenStream;					/**< Reference token stream used by realtime garbage collector, finalized in AssembleReferenceTokenStream */
};

#endif // !CLASS_H