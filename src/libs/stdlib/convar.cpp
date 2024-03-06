/**
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

#include "pch_stdlib.h"
#include "stdlib/convar.h"

CVarDLLIdentifier_t		CCvarLocalRegister::s_dllIdentifier = -1;
IConCmdBase*			CCvarLocalRegister::s_pConCmdList = nullptr;
ICvarAccessor*			CCvarLocalRegister::s_pAccessor = nullptr;
bool					CCvarLocalRegister::s_bCVarsRegistered = false;

/**
 * @ingroup stdlib
 * @brief Default Cvar accessor
 */
class CCvarDefaultAccessor : public ICvarAccessor
{
public:
	/**
	 * @brief Register a command in the console system
	 * @param pCommand	IConVar/IConCmd to register
	 */
	virtual void RegisterCommand( IConCmdBase* pCommand ) override
	{
		// Link to engine's list instead
		g_pCvar->RegisterCommand( pCommand );
	}
};
static CCvarDefaultAccessor		s_CvarDefaultAccessor;


/*
==================
ConVar_Register
==================
*/
void ConVar_Register( uint32 flags /*= FCVAR_NONE*/, ICvarAccessor* pAccessor /*= nullptr*/ )
{
	if ( !g_pCvar || CCvarLocalRegister::s_bCVarsRegistered )
	{
		return;
	}

	Assert( CCvarLocalRegister::s_dllIdentifier == -1 );
	CCvarLocalRegister::s_bCVarsRegistered	= true;
	CCvarLocalRegister::s_dllIdentifier		= g_pCvar->AllocateDLLIdentifier();
	CCvarLocalRegister::s_pAccessor			= pAccessor ? pAccessor : &s_CvarDefaultAccessor;

	IConCmdBase*	pCurCommand = CCvarLocalRegister::s_pConCmdList;
	while ( pCurCommand )
	{
		IConCmdBase*	pNextCommand = pCurCommand->GetNext();
		pCurCommand->AddFlags( flags );
		CCvarLocalRegister::s_pAccessor->RegisterCommand( pCurCommand );
		pCurCommand = pNextCommand;
	}
}

/*
==================
ConVar_Unregister
==================
*/
void ConVar_Unregister()
{
	if ( !g_pCvar || !CCvarLocalRegister::s_bCVarsRegistered )
	{
		return;
	}

	Assert( CCvarLocalRegister::s_dllIdentifier >= 0 );
	g_pCvar->UnregisterCommands( CCvarLocalRegister::s_dllIdentifier );
	CCvarLocalRegister::s_dllIdentifier		= -1;
	CCvarLocalRegister::s_bCVarsRegistered	= false;
	CCvarLocalRegister::s_pAccessor			= nullptr;
}


/*
==================
CConCmd::CConCmd
==================
*/
CConCmd::CConCmd( const achar* pName, ConCmdExecFn_t pExecFn, const achar* pHelpText /* = "" */, uint32 flags /* = FCVAR_NONE */ )
	: TConCmdBase<IConCmd>( pName, pHelpText, flags )
	, pExecFn( pExecFn )
{}

/*
==================
CConCmd::Exec
==================
*/
void CConCmd::Exec( uint32 argc, const achar** argv )
{
	if ( pExecFn )
	{
		pExecFn( argc, argv );
	}
}

/*
==================
CConCmd::IsCommand
==================
*/
bool CConCmd::IsCommand() const
{
	return true;
}


/*
==================
CConVar::CConVar
==================
*/
CConVar::CConVar( const achar* pName, const achar* pDefaultValue, const achar* pHelpText /* = "" */, uint32 flags /* = FCVAR_NONE */, ConVarChangeCallbackFn_t pChangeCallbackFn /* = nullptr */ )
	: TConCmdBase<IConVar>( pName, pHelpText, flags )
	, bHasMin( false )
	, bHasMax( false )
	, minValue( 0.f )
	, maxValue( 0.f )
	, intValue( 0 )
	, floatValue( 0.f )
	, pDefaultValue( pDefaultValue )
	, pParent( this )
	, pChangeCallbackFn( pChangeCallbackFn )
{
	SetString( pDefaultValue );
}

/*
==================
CConVar::CConVar
==================
*/
CConVar::CConVar( const achar* pName, const achar* pDefaultValue, bool bHasMin, float min, bool bHasMax, float max, const achar* pHelpText /* = "" */, uint32 flags /* = FCVAR_NONE */, ConVarChangeCallbackFn_t pChangeCallbackFn /* = nullptr */ )
	: TConCmdBase<IConVar>( pName, pHelpText, flags )
	, bHasMin( bHasMin )
	, bHasMax( bHasMax )
	, minValue( min )
	, maxValue( max )
	, intValue( 0 )
	, floatValue( 0.f )
	, pDefaultValue( pDefaultValue )
	, pParent( this )
	, pChangeCallbackFn( pChangeCallbackFn )
{
	SetString( pDefaultValue );
}

/*
==================
CConVar::Reset
==================
*/
void CConVar::Reset()
{
	IConVar*	pVar = ( IConVar* )pParent;
	pVar->SetString( pVar->GetDefault() );
}

/*
==================
CConVar::SetInt
==================
*/
void CConVar::SetInt( int32 value )
{
	if ( pParent == this )
	{
		if ( value != intValue )
		{
			floatValue = ( float )value;
			if ( ClampValue( floatValue ) )
			{
				intValue = ( int32 )floatValue;
			}
			else
			{
				intValue = value;
			}
			stringValue = L_Sprintf( "%i", intValue );
			OnChangeValue();
		}
	}
	else
	{
		pParent->SetInt( value );
	}
}

/*
==================
CConVar::SetFloat
==================
*/
void CConVar::SetFloat( float value )
{
	if ( pParent == this )
	{
		if ( value != floatValue )
		{
			floatValue	= value;
			ClampValue( floatValue );
			intValue	= ( int32 )floatValue;
			stringValue = L_Sprintf( "%f", floatValue );
			OnChangeValue();
		}
	}
	else
	{
		pParent->SetFloat( value );
	}
}

/*
==================
CConVar::SetBool
==================
*/
void CConVar::SetBool( bool value )
{
	SetInt( value ? 1 : 0 );
}

/*
==================
CConVar::SetString
==================
*/
void CConVar::SetString( const achar* pValue )
{
	if ( pParent == this )
	{
		if ( stringValue != pValue )
		{
			stringValue = pValue;
			if ( !pValue )
			{
				floatValue = 0.f;
			}
			else
			{
				floatValue = L_Atof( pValue );
			}

			if ( ClampValue( floatValue ) )
			{
				stringValue = L_Sprintf( "%f", floatValue );
			}
			intValue = ( int32 )floatValue;
			OnChangeValue();
		}
	}
	else
	{
		pParent->SetString( pValue );
	}
}

/*
==================
CConVar::SetChangeCallback
==================
*/
void CConVar::SetChangeCallback( ConVarChangeCallbackFn_t pChangeCallbackFn )
{
	if ( pParent == this )
	{
		Assert( !CConVar::pChangeCallbackFn || !pChangeCallbackFn );
		CConVar::pChangeCallbackFn = pChangeCallbackFn;
	}
	else
	{
		pParent->SetChangeCallback( pChangeCallbackFn );
	}
}

/*
==================
CConVar::SetParent
==================
*/
void CConVar::SetParent( IConVar* pParentVar )
{
	if ( pParentVar && pParentVar != this )
	{
		Assert( pParentVar->GetParent() == pParentVar );
		pParent = pParentVar;
	}
	else
	{
		pParent = this;
	}
}

/*
==================
CConVar::GetInt
==================
*/
int32 CConVar::GetInt() const
{
	if ( pParent == this )
	{
		return intValue;
	}
	else
	{
		return pParent->GetInt();
	}
}

/*
==================
CConVar::GetFloat
==================
*/
float CConVar::GetFloat() const
{
	if ( pParent == this )
	{
		return floatValue;
	}
	else
	{
		return pParent->GetFloat();
	}
}

/*
==================
CConVar::GetBool
==================
*/
bool CConVar::GetBool() const
{
	return !!GetInt();
}

/*
==================
CConVar::GetString
==================
*/
const achar* CConVar::GetString() const
{
	if ( pParent == this )
	{
		return stringValue.c_str();
	}
	else
	{
		return pParent->GetString();
	}
}

/*
==================
CConVar::GetDefault
==================
*/
const achar* CConVar::GetDefault() const
{
	if ( pParent == this )
	{
		return pDefaultValue;
	}
	else
	{
		return pParent->GetDefault();
	}
}

/*
==================
CConVar::GetParent
==================
*/
IConVar* CConVar::GetParent() const
{
	return pParent;
}

/*
==================
CConVar::GetChangeCallback
==================
*/
ConVarChangeCallbackFn_t CConVar::GetChangeCallback() const
{
	if ( pParent == this )
	{
		return pChangeCallbackFn;
	}
	else
	{
		return pParent->GetChangeCallback();
	}
}

/*
==================
CConVar::IsCommand
==================
*/
bool CConVar::IsCommand() const
{
	return false;
}

/*
==================
CConVar::HasMin
==================
*/
bool CConVar::HasMin() const
{
	return bHasMin;
}

/*
==================
CConVar::HasMax
==================
*/
bool CConVar::HasMax() const
{
	return bHasMax;
}

/*
==================
CConVar::GetMin
==================
*/
float CConVar::GetMin() const
{
	return minValue;
}

/*
==================
CConVar::GetMax
==================
*/
float CConVar::GetMax() const
{
	return maxValue;
}

/*
==================
CConVar::ClampValue
==================
*/
bool CConVar::ClampValue( float& value )
{
	if ( bHasMin && value < minValue )
	{
		value = minValue;
		return true;
	}

	if ( bHasMax && value > maxValue )
	{
		value = maxValue;
		return true;
	}

	return false;
}

/*
==================
CConVar::OnChangeValue
==================
*/
void CConVar::OnChangeValue()
{
	// Invoke callback function
	if ( pChangeCallbackFn )
	{
		pChangeCallbackFn( this );
	}

	if ( g_pCvar )
	{
		g_pCvar->CallGlobalChangeCallback( this );
	}
}


/*
==================
CConVarRef::Init
==================
*/
void CConVarRef::Init( const achar* pName, bool bIgnoreMissing /* = false */ )
{
	// Try find cvar in the system
	pConVar = g_pCvar ? g_pCvar->FindVar( pName ) : nullptr;

	// If pConVar isn't found print warning
	if ( !IsValid() && !bIgnoreMissing )
	{
		static bool		s_bFirst = true;
		if ( s_bFirst )
		{
			Warning( "CConVarRef %s doesn't point to an existing IConVar", pName );
			s_bFirst = false;
		}
	}
}