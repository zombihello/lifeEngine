/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef CONVAR_H
#define CONVAR_H

#include <string>

#include "System/Delegate.h"

/**
 * @ingroup Engine
 * @brief Enumerate console variable type
 */
enum EConVarType
{
	CVT_None,		/**< Unknown */
	CVT_Int,		/**< Integer */
	CVT_Float,		/**< Float */
	CVT_Bool,		/**< Bool */
	CVT_String		/**< String */
};

/**
 * @ingroup Engine
 * @brief Console variable
 */
class CConVar
{
public:
	/**
	 * @brief Declare delegate of change variable
	 */
	DECLARE_MULTICAST_DELEGATE( COnChangeVar, CConVar* /*InConVar*/ );

	/**
	 * @brief Construct
	 * 
	 * @param InName			Name variable
	 * @param InDefaultValue	Default value
	 * @param InType			Variable type
	 * @param InHelpText		Help text
	 * @param InIsReadOnly		Is read only variable
	 */
	CConVar( const std::wstring& InName, const std::wstring& InDefaultValue, EConVarType InType, const std::wstring& InHelpText, bool InIsReadOnly = false );

	/**
	 * @brief Construct
	 *
	 * @param InName			Name variable
	 * @param InDefaultValue	Default value
	 * @param InType			Variable type
	 * @param InHelpText		Help text
	 * @param InHasMin			Is has minimum
	 * @param InMin				Minimum value
	 * @param InHasMax			Is has maximum
	 * @param InMax				Maximum value
	 * @param InIsReadOnly		Is read only variable
	 */
	CConVar( const std::wstring& InName, const std::wstring& InDefaultValue, EConVarType InType, const std::wstring& InHelpText, bool InHasMin, float InMin, bool InHasMax, float InMax, bool InIsReadOnly = false );

	/**
	 * @brief Destructor
	 */
	~CConVar();

	/**
	 * @brief Revert value
	 */
	FORCEINLINE void Revert()
	{
		SetValue( defaultValue, type );
	}

	/**
	 * @brief Set minimum value
	 * 
	 * @param InHasMin	Is has minimum
	 * @param InMin		Minimum value
	 */
	FORCEINLINE void SetMin( bool InHasMin, float InMin = 0.f )
	{
		bHasMin = InHasMin;
		minVar = InMin;
	}

	/**
	 * @brief Set maximum value
	 *
	 * @param InHasMax	Is has maximum
	 * @param InMax		Maximum value
	 */
	FORCEINLINE void SetMax( bool InHasMax, float InMax = 0.f )
	{
		bHasMax = InHasMax;
		maxVar = InMax;
	}

	/**
	 * @brief Set value
	 * 
	 * @param InValue	Value in string view
	 * @param InVarType	Variable type
	 */
	void SetValue( const std::wstring& InValue, EConVarType InVarType );

	/**
	 * @brief Set integer value
	 * @param InValue	Value
	 */
	void SetValueInt( int32 InValue );

	/**
	 * @brief Set float value
	 * @param InValue	Value
	 */
	void SetValueFloat( float InValue );

	/**
	 * @brief Set bool value
	 * @param InValue	Value
	 */
	void SetValueBool( bool InValue );

	/**
	 * @brief Set string value
	 * @param InValue	Value
	 */
	void SetValueString( const std::wstring& InValue );

	/**
	 * @brief Get name
	 * @return Return variable name
	 */
	FORCEINLINE const std::wstring& GetName() const
	{
		return name;
	}

	/**
	 * @brief Get help text
	 * @return Return help text
	 */
	FORCEINLINE const std::wstring& GetHelpText() const
	{
		return helpText;
	}

	/**
	 * @brief Get type
	 * @return Return variable type
	 */
	FORCEINLINE EConVarType GetType() const
	{
		return type;
	}

	/**
	 * @brief Get integer value
	 * @return Return value in integer type. If this variable is not CVT_Int will return 0
	 */
	FORCEINLINE int32 GetValueInt() const
	{
		if ( type != CVT_Int )
		{
			return 0;
		}
		else
		{
			return ( *( int32* )data );
		}
	}

	/**
	 * @brief Get float value
	 * @return Return value in float type. If this variable is not CVT_Float will return 0
	 */
	FORCEINLINE float GetValueFloat() const
	{
		if ( type != CVT_Float )
		{
			return 0.f;
		}
		else
		{
			return ( *( float* )data );
		}
	}

	/**
	 * @brief Get bool value
	 * @return Return value in bool type. If this variable is not CVT_Bool will return FALSE
	 */
	FORCEINLINE bool GetValueBool() const
	{
		if ( type != CVT_Bool )
		{
			return 0.f;
		}
		else
		{
			return ( *( bool* )data );
		}
	}

	/**
	 * @brief Get string value
	 * @return Return value in string type. If this variable is not CVT_String will return empty string
	 */
	FORCEINLINE std::wstring GetValueString() const
	{
		if ( type != CVT_String )
		{
			return TEXT( "" );
		}
		else
		{
			return ( *( std::wstring* )data );
		}
	}

	/**
	 * @brief Get default value in string view
	 * @return Return default value in string view
	 */
	FORCEINLINE const std::wstring& GetValueDefault() const
	{
		return defaultValue;
	}

	/**
	 * @brief Is has minimum value
	 * @return Return TRUE if variable has minimum value
	 */
	FORCEINLINE bool HasMin() const
	{
		return bHasMin;
	}

	/**
	 * @brief Is has maximum value
	 * @return Return TRUE if variable has maximum value
	 */
	FORCEINLINE bool HasMax() const
	{
		return bHasMax;
	}

	/**
	 * @brief Is read only variable
	 * @return Return TRUE if this variable is read only
	 */
	FORCEINLINE bool IsReadOnly() const
	{
		return bReadOnly;
	}

	/**
	 * @brief Get minimum value
	 * @return Return minimum value
	 */
	FORCEINLINE float GetMin() const
	{
		return minVar;
	}

	/**
	 * @brief Get maximum value
	 * @return Return maximum value
	 */
	FORCEINLINE float GetMax() const
	{
		return maxVar;
	}

	/**
	 * @brief Get delegate of change variable
	 * @return Return delegate of change variable
	 */
	FORCEINLINE COnChangeVar& OnChangeVar() const
	{
		return onChangeVar;
	}

private:
	/**
	 * @brief Delete value
	 */
	void DeleteValue();

	bool						bHasMin;		/**< Is has minimum */
	bool						bHasMax;		/**< Is has maximum */
	bool						bReadOnly;		/**< Is read only */
	std::wstring				name;			/**< Name */
	std::wstring				helpText;		/**< Help text */
	std::wstring				defaultValue;	/**< Default value */
	EConVarType					type;			/**< Console variable type */
	void*						data;			/**< Pointer to value */
	float						minVar;			/**< Minimum value */
	float						maxVar;			/**< Maximum value */
	mutable COnChangeVar		onChangeVar;	/**< Change variable delegate */
};

#endif // !CONVAR_H