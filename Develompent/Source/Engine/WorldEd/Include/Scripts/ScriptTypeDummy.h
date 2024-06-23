/**
 * @file
 * @addtogroup WorldEd World editor
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef SCRIPTTYPEDUMMY_H
#define SCRIPTTYPEDUMMY_H

#include "Core.h"

/**
 * @ingroup WorldEd
 * @brief Script dummy type
 */
enum EScriptDummyType
{
	SDT_Void,		/**< Void type */
	SDT_Simple		/**< Identifiers only */
};

/**
 * @ingroup WorldEd
 * @brief Dummy reference to type
 * This is converted to actual type during the type resolve step
 */
class CScriptTypeDummy
{
public:
	/**
	 * @brief Constructor
	 */
	CScriptTypeDummy();

	/**
	 * @brief Constructor
	 * @param InOther	Other object
	 */
	CScriptTypeDummy( const CScriptTypeDummy& InOther );

	/**
	 * @brief Constructor
	 * @param InOther	Other object
	 */
	CScriptTypeDummy( CScriptTypeDummy&& InOther );

	/**
	 * @brief Override operator =
	 * @param InOther	Other object to copy
	 * @return Return reference to self
	 */
	CScriptTypeDummy& operator=( const CScriptTypeDummy& InOther );

	/**
	 * @brief Override operator =
	 * @param InOther	Other object to copy
	 * @return Return reference to self
	 */
	CScriptTypeDummy& operator=( CScriptTypeDummy&& InOther );

	/**
	 * @brief Make void type
	 * @return Return created void type
	 */
	static CScriptTypeDummy MakeVoid();

	/**
	 * @brief Make simple type
	 * @param InName	Type name
	 * @return Return created simple type
	 */
	static CScriptTypeDummy MakeSimple( const std::string_view& InName );

	/**
	 * @brief Convert dummy type into string
	 */
	std::wstring ToString() const;

	/**
	 * @brief Is void type
	 * @return Return TRUE if this dummy type is void
	 */
	FORCEINLINE bool IsVoid() const
	{
		return type == SDT_Void;
	}

	/**
	 * @brief Is simple type
	 * @return Return TRUE if this dummy type is simple
	 */
	FORCEINLINE bool IsSimple() const
	{
		return type == SDT_Simple;
	}

	/**
	 * @brief Get dummy type
	 * @return Return dummy type
	 */
	FORCEINLINE EScriptDummyType GetType() const
	{
		return type;
	}

	/**
	 * @brief Get identifier name (only for simple types)
	 * @return Return identifier name
	 */
	FORCEINLINE const std::wstring& GetIdentName() const
	{
		Assert( type == SDT_Simple );
		return identName;
	}

	/**
	 * @brief Compare dummy type with another
	 * @param InOther	Other type to compare
	 * @return Return TRUE if both is same, otherwise FALSE
	 */
	FORCEINLINE bool Compare( const CScriptTypeDummy& InOther ) const
	{
		return type == InOther.type && identName == InOther.identName;
	}

private:
	EScriptDummyType		type;		/**< Dummy type */
	std::wstring			identName;	/**< Dummy type name */
};

#endif // !SCRIPTTYPEDUMMY_H