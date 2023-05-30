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
	const achar*	category;		/**< Category */
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

#if WITH_EDITOR
	namespace DataMap
	{
		/**
		 * @ingroup Core
		 * @brief Access to data map in the class
		 * @warning Only for editor
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
		 * @brief Access to all fields in data map, include base classes
		 * @warning Only for editor
		 *
		 * @param InDataMap		Data map in the class
		 * @param OutFields		Output array with all fields
		 */
		template<class TSTLContainer = std::vector<const SFieldDescription*>>
		FORCEINLINE void AllFieldsAccess( const SDataMap* InDataMap, TSTLContainer& OutFields )
		{
			for ( const SDataMap* pDataMap = InDataMap; pDataMap; pDataMap = pDataMap->baseMap )
			{
				for ( uint32 idxField = 0; idxField < pDataMap->numFields; ++idxField )
				{
					// We add to OutFields only valid fields
					const SFieldDescription&		field = pDataMap->fields[idxField];
					if ( field.name )
					{
						OutFields.push_back( &pDataMap->fields[idxField] );
					}
				}
			}
		}

		/**
		 * @ingroup Core
		 * @brief Access to field of the object
		 * @warning Only for editor
		 *
		 * @param InObject		Pointer to object
		 * @param InFieldDesc	Description of the class's field
		 * @return Return pointer to field in the object
		 */
		template<typename TType>
		FORCEINLINE TType* DataFieldAccess( void* InObject, SFieldDescription* InFieldDesc )
		{
			return ( TType* )( ( ( byte* )InObject ) + InFieldDesc->offset );
		}
	}

	/**
	 * @ingroup Core
	 * @brief Macro for declare reflection data map in the class
	 * @warning Only for editor
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
	 * @warning Only for editor
	 * 
	 * @param ClassName		Class name
	 * @param BaseClass		Parent class name
	 */
	#define BEGIN_DATADESC( ClassName, BaseClass ) \
		SDataMap ClassName::staticDataMap					= { nullptr, 0, #ClassName, nullptr }; \
		const SDataMap* ClassName::StaticBaseMap()			{ SDataMap* pResult = nullptr; DataMap::DataMapAccess<BaseClass>( &pResult ); return pResult; } \
		const SDataMap* ClassName::GetDataDescMap()	const	{ return &staticDataMap; } \
		BEGIN_DATADESC_GUTS( ClassName )
	
	/**
	 * @ingroup Core
	 * @brief Macro for begin implement data map without a base class
	 * @warning Only for editor
	 * 
	 * @param ClassName		Class name
	 */
	#define BEGIN_DATADESC_NO_BASE( ClassName ) \
		SDataMap ClassName::staticDataMap					= { nullptr, 0, #ClassName, nullptr }; \
		const SDataMap* ClassName::StaticBaseMap()			{ return nullptr; } \
		const SDataMap* ClassName::GetDataDescMap() const	{ return &staticDataMap; } \
		BEGIN_DATADESC_GUTS( ClassName )	

	/**
	 * @ingroup Core
	 * @brief Macro for begin implement data map, main body
	 * @warning Only for editor
	 * 
	 * @param ClassName		Class name
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
				{ FT_Void, nullptr, nullptr, nullptr, 0, 0, 0 },		/* So you can define "empty" tables */

	/**
	 * @ingroup Core
	 * @brief Macro for end implement data map
	 * @warning Only for editor
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
	 * @warning Only for editor
	 * 
	 * @param Name		Field name
	 * @param Category	Category
	 * @param Desc		Description
	 * @param Type		Type (see EFieldType)
	 */
	#define DEFINE_FIELD( Name, Category, Desc, Type ) \
		{ Type, #Name, Category, Desc, STRUCT_OFFSET( ClassTypedef, Name ), 0, 0 },
#else
	namespace DataMap
	{
		template<typename TType>
		FORCEINLINE void DataMapAccess( SDataMap** OutDataMap )
		{
			OutDataMap = nullptr;
		}

		template<class TSTLContainer = std::vector<const SFieldDescription*>>
		FORCEINLINE void AllFieldsAccess( const SDataMap* InDataMap, TSTLContainer& OutFields )
		{}

		template<typename TType>
		FORCEINLINE TType* DataFieldAccess( void* InObject, SFieldDescription* InFieldDesc )
		{
			AssertMsg( false, TEXT( "This function only for WorldEd" ) );
			return nullptr;
		}
	}

	#define DECLARE_DATADESC()
	#define BEGIN_DATADESC( ClassName, BaseClass )
	#define BEGIN_DATADESC_NO_BASE( ClassName )
	#define BEGIN_DATADESC_GUTS( ClassName )
	#define END_DATADESC()
	#define DEFINE_FIELD( Name, Desc, Type )
#endif // WITH_EDITOR

#endif // !DATAMAP_H