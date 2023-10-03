/**
 * @file
 * @addtogroup Core Core
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef STRUCT_H
#define STRUCT_H

#include <vector>
#include "Misc/Object.h"

/**
 * @ingroup Core
 * @brief Macro for declare struct
 * 
 * @param TStruct           Struct
 * @param TSuperStruct      Super struct
 * 
 * Example usage: @code DECLARE_STRUCT( MyStruct, MyStruc ) @endcode
 */
#define DECLARE_STRUCT( TStruct, TSuperStruct ) \
    private: \
        static class CStruct*       staticStruct; \
    public: \
	    typedef TStruct		        ThisStruct; \
	    typedef TSuperStruct	    Super; \
        static void                 StaticInitializeStruct(); \
        static class CStruct*       StaticStruct();

/**
 * @ingroup Core
 * @brief Macro for implement struct
 * 
 * @param TStruct    Struct
 * 
 * Example usage: @code IMPLEMENT_STRUCT( MyStruct ) @endcode
 */
#define IMPLEMENT_STRUCT( TStruct ) \
    CStruct*    TStruct::staticStruct = nullptr; \
    CStruct*    TStruct::StaticStruct() \
    { \
        if ( !staticStruct ) \
        { \
            bool        bBaseStruct = &ThisStruct::StaticStruct == &Super::StaticStruct; \
            staticStruct = ::new CStruct \
            ( \
                TEXT( #TStruct ), \
                sizeof( ThisStruct ), \
                alignof( ThisStruct ), \
                !bBaseStruct ? Super::StaticStruct() : nullptr \
            ); \
            TStruct::StaticInitializeStruct(); \
        } \
        \
        Assert( staticStruct ); \
        return staticStruct; \
    }

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
 * @brief Structure description for reflection
 */
class CStruct : public CObject
{
	DECLARE_CLASS( CStruct, CObject, 0, 0 )

public:
	/**
	 * @brief Constructor
	 */
	FORCEINLINE CStruct()
		: propertiesSize( 0 )
		, minAlignment( 1 )
		, superStruct( nullptr )
	{}

	/**
	 * @brief Constructor
	 *
	 * @param InStructName			Struct name
	 * @param InPropertiesSize		Properties size in bytes
	 * @param InMinAlignment		Minimum alignment for the struct
	 * @param InSuperStruct			Pointer to super struct
	 */
	FORCEINLINE CStruct( const CName& InStructName, uint32 InPropertiesSize, uint32 InMinAlignment, CStruct* InSuperStruct = nullptr )
		: CObject( InStructName )
		, propertiesSize( InPropertiesSize )
		, minAlignment( InMinAlignment )
		, superStruct( InSuperStruct )
	{}

	/**
	 * @brief Get super struct
	 * @return Return pointer to super struct. If it is not it will return NULL
	 */
	FORCEINLINE CStruct* GetSuperStruct() const
	{
		return superStruct;
	}

	/**
	 * @brief Get properties size in bytes
	 * @return Return properties size in bytes
	 */
	FORCEINLINE uint32 GetPropertiesSize() const
	{
		return propertiesSize;
	}

	/**
	 * @brief Get minimum alignment for the struct
	 * @return Return minimum alignment for the struct
	 */
	FORCEINLINE uint32 GetMinAlignment() const
	{
		return minAlignment;
	}

	/**
	 * @brief Is a struct
	 *
	 * @param InStruct	Checking struct
	 * @return Return TRUE if object is a struct InStruct, otherwise returns FALSE
	 */
	FORCEINLINE bool IsA( CStruct* InStruct ) const
	{
		// If struct is not valid we return false
		if ( !InStruct )
		{
			return false;
		}

		// Iterate over each struct ancestor
		for ( const CStruct* tempStruct = this; tempStruct; tempStruct = tempStruct->superStruct )
		{
			// Success! This struct contains the struct being checked
			if ( tempStruct == InStruct )
			{
				return true;
			}
		}

		// Otherwise the struct does not contain the struct being checked 
		return false;
	}

	/**
	 * @brief Add struct property
	 * @param InProperty	Property
	 */
	virtual void AddProperty( class CProperty* InProperty ) override;

	/**
	 * @brief Get array of struct properties
	 *
	 * @param OutArrayProperties		Output array properties
	 * @param InPropertiesInParents		Take into account the properties of the parents
	 */
	void GetProperties( std::vector<class CProperty*>& OutArrayProperties, bool InPropertiesInParents = true ) const;

	/**
	 * @brief Get number properties in the struct
	 *
	 * @param InPropertiesInParents		Take into account the properties of the parents
	 * @return Return number properties in the struct
	 */
	uint32 GetNumProperties( bool InPropertiesInParents = true ) const;

	/**
	 * @brief Find property
	 *
	 * @param InName		Property name
	 * @return Return pointer to struct property. If not found return NULL
	 */
	template<typename TPropertyClass>
	FORCEINLINE TPropertyClass* FindProperty( const CName& InName ) const
	{
		return Cast<TPropertyClass>( InternalFindProperty( InName ) );
	}

private:
	/**
	 * @brief Find property
	 *
	 * @param InName		Property name
	 * @return Return pointer to struct property, if not found return NULL
	 */
	class CProperty* InternalFindProperty( const CName& InName ) const;

	uint32								propertiesSize;	/**< Properties size in bytes */
	uint32								minAlignment;	/**< Minimum alignment for the struct */
	CStruct*							superStruct;	/**< Pointer to super struct */
	std::vector<class CProperty*>		properties;		/**< Struct properties */
};

#endif // !STRUCT_H