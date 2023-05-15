/**
 * @file
 * @addtogroup Core Core
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef DATAMAP_H
#define DATAMAP_H

#include "Misc/Types.h"
#include "Misc/Template.h"
#include "Core.h"

/**
 * @ingroup Core
 * @brief Enumeration of field types in the reflection system
 */
enum EFieldType
{
	FT_Void,			/**< No type or value */
	FT_Integer,			/**< Any integer or enum */
	FT_Float,			/**< Any floating point value */
	FT_Boolean,			/**< Boolean, implemented as an int */

	// Place here your new field types

	FT_Max,				/**< Count filed types, must be last */
};

/**
 * @ingroup Core
 * @brief Enumeration of field flags
 */
enum EFieldFlags
{
	FF_None		= 0		/**< No flags */
};

/**
 * @ingroup Core
 * @brief Field description
 */
struct SFieldDescription
{
	EFieldType		type;			/**< Type */
	const achar*	name;			/**< Name */
	const achar*	description;	/**< Description */
	uint32			offset;			/**< Offset */
	uint32			size;			/**< Size */
	uint16			flags;			/**< Flags (see EFieldFlags) */
};

/**
 * @ingroup Core
 * @brief Reflection data map
 * 
 * Stores the list of objects in the hierarchy
 * used to iterate through an object's data descriptions
 */
struct SDataMap
{
	SFieldDescription*		fields;			/**< Array of data descriptions */
	uint32					numFields;		/**< Number of fields */
	const achar*			className;		/**< Class name */
	SDataMap*				baseMap;		/**< Pointer to data map of base class */
};

/**
 * @ingroup Core
 * @brief Access to data map in the class
 * 
 * @param OutDataMap	Output data map in the class
 */
template<typename TType>
FORCEINLINE void DataMapAccess( SDataMap** OutDataMap )
{
	*OutDataMap = &TType::staticDataMap;
}

/**
 * @ingroup Core
 * @brief Access to field of the object
 * 
 * @param InObject		Pointer to object
 * @param InFieldDesc	Description of the class's field
 * @return Return pointer to field in the object
 */
template<typename TType>
FORCEINLINE TType* FieldAccess( void* InObject, SFieldDescription* InFieldDesc )
{
	return ( TType* )( ( ( byte* )InObject ) + InFieldDesc->offset );
}

/**
 * @ingroup Core
 * @brief Macro for declare reflection data map in the class 
 */
#define DECLARE_DATADESC() \
	static SDataMap								staticDataMap; \
	static const SDataMap*						StaticBaseMap(); \
	virtual const SDataMap*						GetDataDescMap() const; \
	template<typename TType> friend void		DataMapAccess( SDataMap** OutDataMap ); \
	template<typename TType> friend SDataMap*	DataMapInit();

/**
 * @ingroup Core
 * @brief Macro for begin implement data map
 */
#define BEGIN_DATADESC( ClassName, BaseClass ) \
	SDataMap ClassName::staticDataMap					= { nullptr, 0, #ClassName, nullptr }; \
	const SDataMap* ClassName::StaticBaseMap()			{ SDataMap* pResult = nullptr; DataMapAccess<BaseClass>( &pResult ); return pResult; } \
	const SDataMap* ClassName::GetDataDescMap()	const	{ return &staticDataMap; } \
	BEGIN_DATADESC_GUTS( ClassName )
	
/**
 * @ingroup Core
 * @brief Macro for begin implement data map without a base class
 */
#define BEGIN_DATADESC_NO_BASE( ClassName ) \
	SDataMap ClassName::staticDataMap					= { nullptr, 0, #ClassName, nullptr }; \
	const SDataMap* ClassName::StaticBaseMap()			{ return nullptr; } \
	const SDataMap* ClassName::GetDataDescMap() const	{ return &staticDataMap; } \
	BEGIN_DATADESC_GUTS( ClassName )	

/**
 * @ingroup Core
 * @brief Macro for begin implement data map, main body
 */
#define BEGIN_DATADESC_GUTS( ClassName ) \
	template<> SDataMap* DataMapInit<ClassName>(); \
	namespace ClassName##_DataMapInit \
	{ \
		static SDataMap*	s_DataMapHolder = DataMapInit<ClassName>(); \
	} \
	template<> \
	SDataMap* DataMapInit<ClassName>() \
	{ \
		typedef ClassName			ClassTypedef; \
		ClassName::staticDataMap.baseMap = ( SDataMap* )ClassName::StaticBaseMap(); \
		static SFieldDescription	fieldDesc[] = \
		{ \
			{ FT_Void, nullptr, nullptr, 0, 0, 0 },		/* So you can define "empty" tables */

/**
 * @ingroup Core
 * @brief Macro for end implement data map
 */
#define END_DATADESC() \
		}; \
		\
		if ( sizeof( fieldDesc ) > sizeof( fieldDesc[0] ) ) \
		{ \
			ClassTypedef::staticDataMap.numFields	= ARRAY_COUNT( fieldDesc ) - 1; \
			ClassTypedef::staticDataMap.fields		= &fieldDesc[1]; \
		} \
		else \
		{ \
			ClassTypedef::staticDataMap.numFields	= 1; \
			ClassTypedef::staticDataMap.fields		= fieldDesc; \
		} \
		return &ClassTypedef::staticDataMap; \
	}

/**
 * @ingroup Core
 * @brief Macro for define class's field into data map
 */
#define DEFINE_FIELD( Name, Desc, Type ) \
	{ Type, #Name, Desc, STRUCT_OFFSET( ClassTypedef, Name ), 0, 0 },

#endif // !DATAMAP_H