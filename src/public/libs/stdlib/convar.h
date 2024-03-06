/**
 * @file
 * @addtogroup stdlib stdlib
 *
 * ************************************************************
 *                  This file is part of:
 *                      LIFEENGINE
 *          https://github.com/zombihello/lifeEngine
 * ************************************************************
 * Copyright (C) 2024 Yehor Pohuliaka.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef CONVAR_H
#define CONVAR_H

#include <string>

#include "interfaces/interfaces.h"
#include "core/debug.h"
#include "engine/icvar.h"

/**
 * @ingroup stdlib
 * @brief Macro for implement console command
 * 
 * @param Name			Command name
 * @param Description	Command description
 * @param Flags			Command flags
 * 
 *  Example usage:
 *  @code
 *  CON_COMMAND( mycommand, "My console command", FCVAR_CHEAT )
 *  {
 *		Msg( "It's work!\n" );
 *	}
 *  @endcode
 */
#define CON_COMMAND( Name, Description, Flags ) \
	static void ConCmdExec_##Name( uint32 argc, const achar** argv ); \
	static CConCmd s_cmd##Name( #Name, ConCmdExec_##Name, Description, Flags ); \
	static void ConCmdExec_##Name( uint32 argc, const achar** argv )

/**
 * @ingroup stdlib
 * @brief Macro for implement console command with custom function name
 * 
 * @param Name			Command name
 * @param FuncName		Function name
 * @param Description	Command description
 * @param Flags			Command flags
 *
 *  Example usage:
 *  @code
 *  CON_COMMAND_EXTERN( mycommand, CMD_MyCommand, "My console command", FCVAR_CHEAT )
 *  {
 *		Msg( "It's work!\n" );
 *	}
 *  @endcode
 */
#define CON_COMMAND_EXTERN( Name, FuncName, Description, Flags ) \
	void FuncName( uint32 argc, const achar** argv ); \
	static CConCmd s_cmd##Name( #Name, FuncName, Description, Flags ); \
	void FuncName( uint32 argc, const achar** argv )

/**
 * @ingroup stdlib
 * @brief Macro for implement console command with method of a class
 *
 * @param Name			Command name
 * @param MethodName	Static method name
 * @param Description	Command description
 * @param Flags			Command flags
 *
 *  Example usage:
 *  @code
 *  // The method CMyClass::MyCommandExec must be static!
 *  CON_COMMAND_METHOD( mycommand, CMyClass::MyCommandExec, "My console command", FCVAR_CHEAT )
 *  {
 *		Msg( "It's work!\n" );
 *	}
 *  @endcode
 */
#define CON_COMMAND_METHOD( Name, MethodName, Description, Flags ) \
	static CConCmd s_cmd##Name( #Name, MethodName, Description, Flags );

/**
 * @ingroup stdlib
 * @brief Type execute function of a console command
 */
typedef void ( *ConCmdExecFn_t )( uint32 argc, const achar** argv );

/**
 * @ingroup stdlib
 * @brief Any executable that wants to use IConVars/IConCmds need to implement one of these to hook up access to console variables
 */
class ICvarAccessor
{
public:
	/**
	 * @brief Register a command in the console system
	 * @param pCommand	IConVar/IConCmd to register
	 */
	virtual void RegisterCommand( IConCmdBase* pCommand ) = 0;
};

/**
 * @ingroup stdlib
 * @brief Register console invoked command/variable in a local module
 */
class CCvarLocalRegister
{
	friend void ConVar_Register( uint32 flags, ICvarAccessor* pAccessor );
	friend void ConVar_Unregister();

protected:
	static bool					s_bCVarsRegistered;	/**< Is IConVars and IConCmds registered in the engine's system */
	static CVarDLLIdentifier_t	s_dllIdentifier;	/**< DLL identifier */
	static IConCmdBase*			s_pConCmdList;		/**< Statically constructed list of IConCmdBases, used for registering them with the ICvar interface */
	static ICvarAccessor*		s_pAccessor;		/**< IConVars and IConCmds in this executable use this 'global' to access values */
};

/**
 * @ingroup stdlib
 * @brief The base console invoked command/variable
 */
template<class TBaseClass>
class TConCmdBase : public TBaseClass, public CCvarLocalRegister
{
public:
	/**
	 * @brief Constructor
	 * 
	 * @param pName		Name. Should be static data
	 * @param pHelpText	Help text. Should be static data
	 * @param flags		Flags
	 */
	TConCmdBase( const achar* pName, const achar* pHelpText = "", uint32 flags = FCVAR_NONE )
		: bRegistered( false )
		, flags( flags )
		, pName( pName )
		, pHelpText( pHelpText )
		, pNext( nullptr )
	{
		Assert( pName );

		// Register in the local list
		pNext = CCvarLocalRegister::s_pConCmdList;
		CCvarLocalRegister::s_pConCmdList = this;

		// If s_pAccessor is already set, register it
		if ( s_pAccessor )
		{
			s_pAccessor->RegisterCommand( this );
		}
	}

	/**
	 * @brief Destructor
	 */
	~TConCmdBase()
	{
		if ( g_pCvar )
		{
			g_pCvar->UnregisterCommand( this );
		}
	}

	/**
	 * @brief Is command
	 * @return Return TRUE if this interface is a command, otherwise returns FALSE
	 */
	virtual bool IsCommand() const override
	{
		Assert( false );
		return false;
	}

	/**
	 * @brief Is set a flag
	 * 
	 * @param flag	Flag to check (see ECVarFlags)
	 * @return Return TRUE if flag is set, otherwise returns FALSE
	 */
	virtual bool IsFlagSet( uint32 flag ) const override
	{
		return ( flags & flag ) ? true : false;
	}

	/**
	 * @brief Get flags
	 * @return Return cvar flags (see ECVarFlags)
	 */
	virtual uint32 GetFlags() const override
	{
		return flags;
	}

	/**
	 * @brief Add flags
	 * @param flags		Flags (see ECVarFlags)
	 */
	virtual void AddFlags( uint32 flags ) override
	{
		TConCmdBase::flags |= flags;
	}

	/**
	 * @brief Get name for cvar
	 * @return Return name
	 */
	virtual const achar* GetName() const override
	{
		return pName;
	}

	/**
	 * @brief Set help text for cvar
	 * @param pHelpText		Help text. Should be static data
	 */
	virtual void SetHelpText( const achar* pHelpText ) override
	{
		TConCmdBase::pHelpText = pHelpText ? pHelpText : "";
	}

	/**
	 * @brief Get help text for cvar
	 * @return Return help text
	 */
	virtual const achar* GetHelpText() const override
	{
		return pHelpText;
	}

	/**
	 * @brief Set next cvar in the global list
	 * @warning Internal usage only!
	 * 
	 * @param pNext		Next cvar in the global list
	 */
	virtual void SetNext( IConCmdBase* pNext ) override
	{
		TConCmdBase::pNext = pNext;
	}

	/**
	 * @brief Get next cvar in the global list
	 * @return Return pointer to the next cvar in the global list. If no exists returns NULL
	 */
	virtual IConCmdBase* GetNext() const override
	{
		return pNext;
	}

	/**
	 * @brief Set registered cvar
	 * @warning Internal usage only!
	 * 
	 * @param bRegistered	Is registered cvar
	 */
	virtual void SetRegistered( bool bRegistered ) override
	{
		TConCmdBase::bRegistered = bRegistered;
	}

	/**
	 * @brief Has this cvar been registered
	 * @return Returns TRUE on success, FALSE on failure
	 */
	virtual bool IsRegistered() const override
	{
		return bRegistered;
	}

	/**
	 * @brief Get the DLL identifier
	 * @return Returns the DLL identifier
	 */
	virtual CVarDLLIdentifier_t GetDLLIdentifier() const override
	{
		return CCvarLocalRegister::s_dllIdentifier;
	}

private:
	bool			bRegistered;	/**< Has the cvar been added to the global list? */
	uint32			flags;			/**< Cvar flags */
	const achar*	pName;			/**< Name. Should be static data */
	const achar*	pHelpText;		/**< Help text. Should be static data */
	IConCmdBase*	pNext;			/**< Next IConVar/IConCmds in chain */
};

/**
 * @ingroup stdlib
 * @brief The console invoked command
 */
class CConCmd : public TConCmdBase<IConCmd>
{
public:
	/**
	 * @brief Constructor
	 *
	 * @param pName			Name command. Should be static data
	 * @param pExecFn		Execute function
	 * @param pHelpText		Help text. Should be static data
	 * @param flags			Flags (see EConVarFlags)
	 */
	CConCmd( const achar* pName, ConCmdExecFn_t pExecFn, const achar* pHelpText = "", uint32 flags = FCVAR_NONE );

	/**
	 * @brief Execute command
	 *
	 * @param argc	Argument count
	 * @param argv	Arguments
	 */
	virtual void Exec( uint32 argc, const achar** argv ) override;

	/**
	 * @brief Is command
	 * @return Return TRUE if this interface is a command, otherwise returns FALSE
	 */
	virtual bool IsCommand() const override;

private:
	ConCmdExecFn_t		pExecFn;	/**< Execute function */
};

/**
 * @ingroup engine
 * @brief The console variable
 */
class CConVar : public TConCmdBase<IConVar>
{
public:
	/**
	 * @brief Constructor
	 *
	 * @param pName					Name command. Should be static data
	 * @param pDefaultValue			Default value. Should be static data
	 * @param pHelpText				Help text. Should be static data
	 * @param flags					Flags (see EConVarFlags)
	 * @param pChangeCallbackFn		Change callback
	 */
	CConVar( const achar* pName, const achar* pDefaultValue, const achar* pHelpText = "", uint32 flags = FCVAR_NONE, ConVarChangeCallbackFn_t pChangeCallbackFn = nullptr );

	/**
	 * @brief Constructor
	 *
	 * @param pName					Name command. Should be static data
	 * @param pDefaultValue			Default value. Should be static data
	 * @param bHasMin				Is has minimum
	 * @param min					Minimum value
	 * @param bHasMax				Is has maximum
	 * @param max					Maximum value
	 * @param pHelpText				Help text. Should be static data
	 * @param flags					Flags (see EConVarFlags)
	 * @param pChangeCallbackFn		Change callback
	 */
	CConVar( const achar* pName, const achar* pDefaultValue, bool bHasMin, float min, bool bHasMax, float max, const achar* pHelpText = "", uint32 flags = FCVAR_NONE, ConVarChangeCallbackFn_t pChangeCallbackFn = nullptr );

	/**
	 * @brief Reset value
	 */
	virtual void Reset() override;

	/**
	 * @brief Set integer value
	 * @param value		Value
	 */
	virtual void SetInt( int32 value ) override;

	/**
	 * @brief Set float value
	 * @param value		Value
	 */
	virtual void SetFloat( float value ) override;

	/**
	 * @brief Set bool value
	 * @param value		Value
	 */
	virtual void SetBool( bool value ) override;

	/**
	 * @brief Set string value
	 * @param pValue	Value
	 */
	virtual void SetString( const achar* pValue ) override;

	/**
	 * @brief Set change callback
	 * @param pChangeCallbackFn		Change callback
	 */
	virtual void SetChangeCallback( ConVarChangeCallbackFn_t pChangeCallbackFn ) override;

	/**
	 * @brief Set parent variable
	 * @warning Internal usage only!
	 * 
	 * This either points to self or it points to the original declaration of a IConVar.
	 * This allows IConVars to exist in separate modules, and they all use the first one to be declared
	 * 
	 * @param pParentVar	Parent variable. pParentVar->GetParent() must equal pParentVar (ie: pParentVar must be the root, or original IConVar)
	 */
	virtual void SetParent( IConVar* pParentVar ) override;

	/**
	 * @brief Get integer value
	 * @return Return value as integer type
	 */
	virtual int32 GetInt() const override;

	/**
	 * @brief Get float value
	 * @return Return value as float type
	 */
	virtual float GetFloat() const override;

	/**
	 * @brief Get bool value
	 * @return Return value as bool type
	 */
	virtual bool GetBool() const override;

	/**
	 * @brief Get string value
	 * @return Return value as string type
	 */
	virtual const achar* GetString() const override;

	/**
	 * @brief Get default value in string view
	 * @return Return default value in string view
	 */
	virtual const achar* GetDefault() const override;

	/**
	 * @brief Get parent variable
	 * 
	 * This either points to self or it points to the original declaration of a IConVar.
	 * This allows IConVars to exist in separate modules, and they all use the first one to be declared
	 * 
	 * @return Return parent variable. GetParent()->GetParent() must equal GetParent() (ie: GetParent() must be the root, or original IConVar)
	 */
	virtual IConVar* GetParent() const override;

	/**
	 * @brief Get change callback
	 * @return Return change callback. If isn't returns NULL
	 */
	virtual ConVarChangeCallbackFn_t GetChangeCallback() const override;

	/**
	 * @brief Is command
	 * @return Return TRUE if this interface is a command, otherwise returns FALSE
	 */
	virtual bool IsCommand() const override;

	/**
	 * @brief Is has minimum value
	 * @return Return TRUE if variable has minimum value, otherwise returns FALSE
	 */
	virtual bool HasMin() const override;

	/**
	 * @brief Is has maximum value
	 * @return Return TRUE if variable has maximum value, otherwise returns FALSE
	 */
	virtual bool HasMax() const override;

	/**
	 * @brief Get minimum value
	 * @return Return minimum value
	 */
	virtual float GetMin() const override;

	/**
	 * @brief Get maximum value
	 * @return Return maximum value
	 */
	virtual float GetMax() const override;

private:
	/**
	 * @brief Clamp value
	 * 
	 * @param value		Value to clamp
	 * @return Return TRUE if value changed, otherwise returns FALSE
	 */
	bool ClampValue( float& value );

	/**
	 * @brief Call change callback
	 */
	void OnChangeValue();

	bool						bHasMin;			/**< Is has minimum */
	bool						bHasMax;			/**< Is has maximum */
	float						minValue;			/**< Minimum value */
	float						maxValue;			/**< Maximum value */
	int32						intValue;			/**< Integer value */
	float						floatValue;			/**< Float value */
	std::string					stringValue;		/**< String value */
	const achar*				pDefaultValue;		/**< Default value */
	IConVar*					pParent;			/**< This either points to self or it points to the original declaration of a IConVar */
	ConVarChangeCallbackFn_t	pChangeCallbackFn;	/**< Call this function when IConVar changes */
};

/**
 * @ingroup stdlib
 * @brief Reference to IConVars that already exist (replaces the FindVar method)
 */
class CConVarRef
{
public:
	/**
	 * @brief Constructor
	 * 
	 * @param pName				Cvar name
	 * @param bIgnoreMissing	Is need ignore missing cvar
	 */
	FORCEINLINE CConVarRef( const achar* pName, bool bIgnoreMissing = false )
		: pConVar( nullptr )
	{
		Init( pName, bIgnoreMissing );
	}

	/**
	 * @brief Constructor
	 * @param pConVar	Console variable
	 */
	FORCEINLINE CConVarRef( IConVar* pConVar )
		: pConVar( pConVar )
	{}

	/**
	 * @brief Initialize reference by cvar name
	 * 
	 * @param pName				Cvar name
	 * @param bIgnoreMissing	Is need ignore missing cvar
	 */
	void Init( const achar* pName, bool bIgnoreMissing = false );

	/**
	 * @brief Overloaded operator =
	 * 
	 * @param pConVar	Pointer to cvar
	 * @return Return reference to current object
	 */
	FORCEINLINE CConVarRef& operator=( IConVar* pConVar )
	{
		CConVarRef::pConVar = pConVar;
		return *this;
	}

	/**
	 * @brief Overloaded operator =
	 * 
	 * @param copy	Copy
	 * @return Return reference to current object
	 */
	FORCEINLINE CConVarRef& operator=( const CConVarRef& copy )
	{
		pConVar = copy.pConVar;
		return *this;
	}

	/**
	 * @brief Overloaded operator ==
	 * 
	 * @param right		Right operand
	 * @return Return TRUE if pointers is equal, FALSE otherwise
	 */
	FORCEINLINE bool operator==( const CConVarRef& right ) const
	{
		return pConVar == right.pConVar;
	}

	/**
	 * @brief Overloaded operator ==
	 * 
	 * @param pRight	Right operand
	 * @return Return TRUE if pointers is equal, FALSE otherwise
	 */
	FORCEINLINE bool operator==( IConVar* pRight ) const
	{
		return pConVar == pRight;
	}

	/**
	 * @brief Overloaded operator !=
	 * 
	 * @param right		Right operand
	 * @return Return TRUE if pointers is not equal, FALSE otherwise
	 */
	FORCEINLINE bool operator!=( const CConVarRef& right ) const
	{
		return pConVar != right.pConVar;
	}

	/**
	 * @brief Overloaded operator !=
	 * 
	 * @param pRight	Right operand
	 * @return Return TRUE if pointers is not equal, FALSE otherwise
	 */
	FORCEINLINE bool operator!=( IConVar* pRight ) const
	{
		return pConVar != pRight;
	}

	/**
	 * @brief Is valid reference
	 * @return Return TRUE if reference is valid, otherwise returns FALSE
	 */
	FORCEINLINE bool IsValid() const
	{
		return !!pConVar;
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
	 * @brief Overloaded operator IConVar*
	 * @return Return pointer to cvar
	 */
	FORCEINLINE operator IConVar*() const
	{
		return pConVar;
	}

	/**
	 * @brief Overloaded operator IConVar*&
	 * @return Return reference to pointer cvar
	 */
	FORCEINLINE operator IConVar*&()
	{
		return pConVar;
	}

	/**
	 * @brief Overloaded operator ->
	 * @return Return pointer to cvar
	 */
	FORCEINLINE IConVar* operator->() const
	{
		Assert( pConVar );
		return pConVar;
	}

private:
	IConVar*	pConVar;	/**< Found cvar */
};

/**
* @ingroup stdlib
* @brief Register IConVars and IConCmds with the ICvar
* 
* @param flags		Add these flags to IConVars and IConCmds
* @param pAccessor	Accessor for register IConVars and IConCmds. If NULL use default accessor
*/
void ConVar_Register( uint32 flags = FCVAR_NONE, ICvarAccessor* pAccessor = nullptr );

/**
 * @ingroup stdlib
 * @brief Unregister IConVars and IConCmds with the ICvar
 */
void ConVar_Unregister();

#endif // !CONVAR_H