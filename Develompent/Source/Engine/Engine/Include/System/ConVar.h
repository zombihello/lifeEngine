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

#include "Misc/Platform.h"
#include "Misc/Types.h"
#include "System/Delegate.h"

/**
 * @ingroup Engine
 * @brief Macro for implement console command
 * 
 * @param Name			Command name
 * @param Description	Command description
 * @param Flags			Command flags (see ECVarFlags)
 * 
 *  Example usage:
 *  @code
 *  CON_COMMAND( mycommand, TEXT( "My console command" ), FCVAR_Cheat )
 *  {
 *		Logf( TEXT( "It's works!\n" ) );
 *	}
 *  @endcode
 */
#define CON_COMMAND( Name, Description, Flags ) \
	static void ConCmdExec_##Name( uint32 InArgc, const tchar** InArgv ); \
	static CConCmd s_cmd##Name( TEXT( #Name ), ConCmdExec_##Name, Description, Flags ); \
	static void ConCmdExec_##Name( uint32 InArgc, const tchar** InArgv )

/**
 * @ingroup Engine
 * @brief Macro for implement console command with custom function name
 * 
 * @param Name			Command name
 * @param FuncName		Function name
 * @param Description	Command description
 * @param Flags			Command flags (see ECVarFlags)
 *
 *  Example usage:
 *  @code
 *  CON_COMMAND_EXTERN( mycommand, CMD_MyCommand, TEXT( "My console command" ), FCVAR_Cheat )
 *  {
 *		Logf( TEXT( "It's works!\n" ) );
 *	}
 *  @endcode
 */
#define CON_COMMAND_EXTERN( Name, FuncName, Description, Flags ) \
	void FuncName( uint32 InArgc, const tchar** InArgv ); \
	static CConCmd s_cmd##Name( TEXT( #Name ), FuncName, Description, Flags ); \
	void FuncName( uint32 InArgc, const tchar** InArgv )

/**
 * @ingroup Engine
 * @brief Macro for implement console command with method of a class
 *
 * @param Name			Command name
 * @param MethodName	Static method name
 * @param Description	Command description
 * @param Flags			Command flags (see ECVarFlags)
 *
 *  Example usage:
 *  @code
 *  // The method CMyClass::MyCommandExec must be static!
 *  CON_COMMAND_METHOD( mycommand, CMyClass::MyCommandExec, TEXT( "My console command" ), FCVAR_Cheat )
 *  {
 *		Logf( TEXT( "It's works!\n" ) );
 *	}
 *  @endcode
 */
#define CON_COMMAND_METHOD( Name, MethodName, Description, Flags ) \
	static CConCmd s_cmd##Name( TEXT( #Name ), MethodName, Description, Flags );

/**
 * @ingroup Engine
 * @brief Type execute function of a console command
 */
typedef void ( *ConCmdExecFn_t )( uint32 InArgc, const tchar** InArgv );

/**
 * @ingroup Engine
 * @brief Type callback function of change console variable
 */
typedef void ( *ConVarChangeCallbackFn_t )( class CConVar* InConVar );

/**
 * @ingroup Engine
 * @brief CConVar and CConCmd flags
 */
enum ECVarFlags
{
	FCVAR_None		= 0,		/**< No flags */
	FCVAR_Cheat		= 1 << 0,	/**< This cvar is a cheat */
	FCVAR_Savable	= 1 << 1	/**< Set to cause it to be saved to a file */
};

/**
 * @ingroup Engine
 * @brief The base console invoked command/variable
 */
class CConCmdBase
{
public:
	friend class CCvar;

	/**
	 * @brief Constructor
	 * 
	 * @param InName		Name. Should be static data
	 * @param InHelpText	Help text. Should be static data
	 * @param InFlags		Flags (see ECVarFlags)
	 */
	CConCmdBase( const tchar* InName, const tchar* InHelpText = TEXT( "" ), uint32 InFlags = FCVAR_None );

	/**
	 * @brief Destructor
	 */
	~CConCmdBase();

	/**
	 * @brief Is command
	 * @return Return TRUE if this interface is a command, otherwise returns FALSE
	 */
	virtual bool IsCommand() const = 0;

	/**
	 * @brief Is set a flag
	 *
	 * @param InFlag	Flag to check (see ECVarFlags)
	 * @return Return TRUE if flag is set, otherwise returns FALSE
	 */
	FORCEINLINE bool IsFlagSet( uint32 InFlag ) const
	{
		return ( flags & InFlag ) ? true : false;
	}

	/**
	 * @brief Get flags
	 * @return Return cvar flags (see ECVarFlags)
	 */
	FORCEINLINE uint32 GetFlags() const
	{
		return flags;
	}

	/**
	 * @brief Add flags
	 * @param InFlags	Flags (see ECVarFlags)
	 */
	FORCEINLINE void AddFlags( uint32 InFlags )
	{
		flags |= InFlags;
	}

	/**
	 * @brief Get name of this cvar
	 * @return Return name
	 */
	FORCEINLINE const tchar* GetName() const
	{
		return name;
	}

	/**
	 * @brief Set help text for cvar
	 * @param InHelpText	Help text. Should be static data
	 */
	FORCEINLINE void SetHelpText( const tchar* InHelpText )
	{
		helpText = InHelpText ? InHelpText : TEXT( "" );
	}

	/**
	 * @brief Get help text for cvar
	 * @return Return help text
	 */
	FORCEINLINE const tchar* GetHelpText() const
	{
		return helpText;
	}

private:
	bool			bRegistered;	/**< Has the cvar been added to the global list? */
	uint32			flags;			/**< Cvar flags (see ECVarFlags) */
	const tchar*	name;			/**< Name. Should be static data */
	const tchar*	helpText;		/**< Help text. Should be static data */
	CConCmdBase*	next;			/**< Next CConVar/CConCmd in the chain */
};

/**
 * @ingroup Engine
 * @brief The console invoked command
 */
class CConCmd : public CConCmdBase
{
public:
	/**
	 * @brief Constructor
	 * 
	 * @param InName		Name. Should be static data
	 * @param InExecFn		Execute function
	 * @param InHelpText	Help text. Should be static data
	 * @param InFlags		Flags (see ECVarFlags)
	 */
	CConCmd( const tchar* InName, ConCmdExecFn_t InExecFn, const tchar* InHelpText = TEXT( "" ), uint32 InFlags = FCVAR_None );

	/**
	 * @brief Execute command
	 *
	 * @param InArgc	Argument count
	 * @param InArgv	Arguments
	 */
	FORCEINLINE void Exec( uint32 InArgc, const tchar** InArgv )
	{
		if ( ExecFn )
		{
			ExecFn( InArgc, InArgv );
		}
	}

	/**
	 * @brief Is command
	 * @return Return TRUE if this interface is a command, otherwise returns FALSE
	 */
	virtual bool IsCommand() const override;

private:
	ConCmdExecFn_t	ExecFn;	/**< Execute function */
};

/**
 * @ingroup Engine
 * @brief The console variable
 */
class CConVar : public CConCmdBase
{
public:
	friend class CCvar;

	/**
	 * @brief Declare delegate of change variable
	 */
	DECLARE_MULTICAST_DELEGATE( COnChangeVar, CConVar* /*InConVar*/ );

	/**
	 * @brief Constructor
	 *
	 * @param InName				Name command. Should be static data
	 * @param InDefaultValue		Default value. Should be static data
	 * @param InHelpText			Help text. Should be static data
	 * @param InFlags				Flags (see EConVarFlags)
	 * @param InChangeCallbackFn	Change callback
	 */
	CConVar( const tchar* InName, const tchar* InDefaultValue, const tchar* InHelpText = TEXT( "" ), uint32 InFlags = FCVAR_None, ConVarChangeCallbackFn_t InChangeCallbackFn = nullptr );

	/**
	 * @brief Constructor
	 *
	 * @param InName				Name command. Should be static data
	 * @param InDefaultValue		Default value. Should be static data
	 * @param InHasMin				Is has minimum
	 * @param InMin					Minimum value
	 * @param InHasMax				Is has maximum
	 * @param InMax					Maximum value
	 * @param InHelpText			Help text. Should be static data
	 * @param InFlags				Flags (see EConVarFlags)
	 * @param InChangeCallbackFn	Change callback
	 */
	CConVar( const tchar* InName, const tchar* InDefaultValue, bool InHasMin, float InMin, bool InHasMax, float InMax, const tchar* InHelpText = TEXT( "" ), uint32 InFlags = FCVAR_None, ConVarChangeCallbackFn_t InChangeCallbackFn = nullptr );

	/**
	 * @brief Is command
	 * @return Return TRUE if this interface is a command, otherwise returns FALSE
	 */
	virtual bool IsCommand() const override;

	/**
	 * @brief Reset value
	 */
	void Reset();

	/**
	 * @brief Set integer value
	 * @param InValue	Value
	 */
	void SetInt( int32 InValue );

	/**
	 * @brief Set float value
	 * @param InValue	Value
	 */
	void SetFloat( float InValue );

	/**
	 * @brief Set bool value
	 * @param InValue	Value
	 */
	void SetBool( bool InValue );

	/**
	 * @brief Set string value
	 * @param InValue	Value
	 */
	void SetString( const tchar* InValue );

	/**
	 * @brief Get integer value
	 * @return Return value as integer type
	 */
	FORCEINLINE int32 GetInt() const
	{
		return intValue;
	}

	/**
	 * @brief Get float value
	 * @return Return value as float type
	 */
	FORCEINLINE float GetFloat() const
	{
		return floatValue;
	}

	/**
	 * @brief Get bool value
	 * @return Return value as bool type
	 */
	FORCEINLINE bool GetBool() const
	{
		return !!GetInt();
	}

	/**
	 * @brief Get string value
	 * @return Return value as string type
	 */
	FORCEINLINE const tchar* GetString() const
	{
		return stringValue.c_str();
	}

	/**
	 * @brief Get default value in string view
	 * @return Return default value in string view
	 */
	FORCEINLINE const tchar* GetDefault() const
	{
		return defaultValue;
	}

	/**
	 * @brief Is has minimum value
	 * @return Return TRUE if variable has minimum value, otherwise returns FALSE
	 */
	FORCEINLINE bool HasMin() const
	{
		return bHasMin;
	}

	/**
	 * @brief Is has maximum value
	 * @return Return TRUE if variable has maximum value, otherwise returns FALSE
	 */
	FORCEINLINE bool HasMax() const
	{
		return bHasMax;
	}

	/**
	 * @brief Get minimum value
	 * @return Return minimum value
	 */
	FORCEINLINE float GetMin() const
	{
		return minValue;
	}

	/**
	 * @brief Get maximum value
	 * @return Return maximum value
	 */
	FORCEINLINE float GetMax() const
	{
		return maxValue;
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
	 * @brief Clamp value
	 *
	 * @param InOutValue	Value to clamp
	 * @return Return TRUE if value changed, otherwise returns FALSE
	 */
	bool ClampValue( float& InOutValue );

	/**
	 * @brief Call change callback
	 */
	void OnChangeValue();

	bool					bHasMin;		/**< Is has minimum */
	bool					bHasMax;		/**< Is has maximum */
	float					minValue;		/**< Minimum value */
	float					maxValue;		/**< Maximum value */
	int32					intValue;		/**< Integer value */
	float					floatValue;		/**< Float value */
	std::wstring			stringValue;	/**< String value */
	const tchar*			defaultValue;	/**< Default value */
	mutable COnChangeVar	onChangeVar;	/**< Change variable delegate */
};

/**
 * @ingroup Engine
 * @brief Reference to CConVars that already exist (replaces the FindVar method)
 */
class CConVarRef
{
public:
	/**
	 * @brief Constructor
	 * 
	 * @param InName				Cvar name
	 * @param InIsIgnoreMissing		Is need ignore missing cvar
	 */
	FORCEINLINE CConVarRef( const tchar* InName, bool InIsIgnoreMissing = false )
		: conVar( nullptr )
	{
		Init( InName, InIsIgnoreMissing );
	}

	/**
	 * @brief Constructor
	 * @param InConVar	Console variable
	 */
	FORCEINLINE CConVarRef( CConVar* InConVar )
		: conVar( InConVar )
	{}

	/**
	 * @brief Initialize reference by cvar name
	 * 
	 * @param InName				Cvar name
	 * @param InIsIgnoreMissing		Is need ignore missing cvar
	 */
	void Init( const tchar* InName, bool InIsIgnoreMissing = false );

	/**
	 * @brief Overloaded operator =
	 * 
	 * @param InConVar	Pointer to cvar
	 * @return Return reference to current object
	 */
	FORCEINLINE CConVarRef& operator=( CConVar* InConVar )
	{
		conVar = InConVar;
		return *this;
	}

	/**
	 * @brief Overloaded operator =
	 * 
	 * @param InCopy	Copy
	 * @return Return reference to current object
	 */
	FORCEINLINE CConVarRef& operator=( const CConVarRef& InCopy )
	{
		conVar = InCopy.conVar;
		return *this;
	}

	/**
	 * @brief Overloaded operator ==
	 * 
	 * @param InRight		Right operand
	 * @return Return TRUE if pointers is equal, FALSE otherwise
	 */
	FORCEINLINE bool operator==( const CConVarRef& InRight ) const
	{
		return conVar == InRight.conVar;
	}

	/**
	 * @brief Overloaded operator ==
	 * 
	 * @param InRight	Right operand
	 * @return Return TRUE if pointers is equal, FALSE otherwise
	 */
	FORCEINLINE bool operator==( CConVar* InRight ) const
	{
		return conVar == InRight;
	}

	/**
	 * @brief Overloaded operator !=
	 * 
	 * @param InRight	Right operand
	 * @return Return TRUE if pointers is not equal, FALSE otherwise
	 */
	FORCEINLINE bool operator!=( const CConVarRef& InRight ) const
	{
		return conVar != InRight.conVar;
	}

	/**
	 * @brief Overloaded operator !=
	 * 
	 * @param InRight	Right operand
	 * @return Return TRUE if pointers is not equal, FALSE otherwise
	 */
	FORCEINLINE bool operator!=( CConVar* InRight ) const
	{
		return conVar != InRight;
	}

	/**
	 * @brief Is valid reference
	 * @return Return TRUE if reference is valid, otherwise returns FALSE
	 */
	FORCEINLINE bool IsValid() const
	{
		return !!conVar;
	}

	/**
	 * @brief Overloaded operator bool
	 * @return Return TRUE if pointer isn't NULL, FALSE otherwise
	 */
	FORCEINLINE operator bool() const
	{
		return IsValid();
	}

	/**
	 * @brief Overloaded operator CConVar*
	 * @return Return pointer to cvar
	 */
	FORCEINLINE operator CConVar*() const
	{
		return conVar;
	}

	/**
	 * @brief Overloaded operator CConVar*&
	 * @return Return reference to pointer cvar
	 */
	FORCEINLINE operator CConVar*&()
	{
		return conVar;
	}

	/**
	 * @brief Overloaded operator ->
	 * @return Return pointer to cvar
	 */
	FORCEINLINE CConVar* operator->() const
	{
		Assert( conVar );
		return conVar;
	}

private:
	CConVar*	conVar;	/**< Found cvar */
};

#endif // !CONVAR_H