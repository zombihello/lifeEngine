/**
 * @file
 * @addtogroup engine engine
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

#ifndef ICONVAR_H
#define ICONVAR_H

#include "engine/iconcmd.h"

/**
 * @ingroup engine
 * @brief Type callback function of change console variable
 */
typedef void ( *ConVarChangeCallbackFn_t )( class IConVar* pConVar );

/**
 * @ingroup engine
 * @brief The console variable interface
 */
class IConVar : public IConCmdBase
{
public:
	/**
	 * @brief Reset value
	 */
	virtual void Reset() = 0;

	/**
	 * @brief Set integer value
	 * @param value		Value
	 */
	virtual void SetInt( int32 value ) = 0;

	/**
	 * @brief Set float value
	 * @param value		Value
	 */
	virtual void SetFloat( float value ) = 0;

	/**
	 * @brief Set bool value
	 * @param value		Value
	 */
	virtual void SetBool( bool value ) = 0;

	/**
	 * @brief Set string value
	 * @param pValue	Value
	 */
	virtual void SetString( const achar* pValue ) = 0;

	/**
	 * @brief Set change callback
	 * @param pChangeCallbackFn		Change callback
	 */
	virtual void SetChangeCallback( ConVarChangeCallbackFn_t pChangeCallbackFn ) = 0;

	/**
	 * @brief Set parent variable
	 * @warning Internal usage only!
	 * 
	 * This either points to self or it points to the original declaration of a IConVar.
	 * This allows IConVars to exist in separate modules, and they all use the first one to be declared
	 * 
	 * @param pParentVar	Parent variable. pParentVar->GetParent() must equal pParentVar (ie: pParentVar must be the root, or original IConVar)
	 */
	virtual void SetParent( IConVar* pParentVar ) = 0;

	/**
	 * @brief Get integer value
	 * @return Return value as integer type
	 */
	virtual int32 GetInt() const = 0;

	/**
	 * @brief Get float value
	 * @return Return value as float type
	 */
	virtual float GetFloat() const = 0;

	/**
	 * @brief Get bool value
	 * @return Return value as bool type
	 */
	virtual bool GetBool() const = 0;

	/**
	 * @brief Get string value
	 * @return Return value as string type
	 */
	virtual const achar* GetString() const = 0;

	/**
	 * @brief Get default value in string view
	 * @return Return default value in string view
	 */
	virtual const achar* GetDefault() const = 0;

	/**
	 * @brief Get parent variable
	 * 
	 * This either points to self or it points to the original declaration of a IConVar.
	 * This allows IConVars to exist in separate modules, and they all use the first one to be declared
	 * 
	 * @return Return parent variable. GetParent()->GetParent() must equal GetParent() (ie: GetParent() must be the root, or original IConVar)
	 */
	virtual IConVar* GetParent() const = 0;

	/**
	 * @brief Get change callback
	 * @return Return change callback. If isn't returns NULL
	 */
	virtual ConVarChangeCallbackFn_t GetChangeCallback() const = 0;

	/**
	 * @brief Is has minimum value
	 * @return Return TRUE if variable has minimum value, otherwise returns FALSE
	 */
	virtual bool HasMin() const = 0;

	/**
	 * @brief Is has maximum value
	 * @return Return TRUE if variable has maximum value, otherwise returns FALSE
	 */
	virtual bool HasMax() const = 0;

	/**
	 * @brief Get minimum value
	 * @return Return minimum value
	 */
	virtual float GetMin() const = 0;

	/**
	 * @brief Get maximum value
	 * @return Return maximum value
	 */
	virtual float GetMax() const = 0;
};

#endif // !ICONVAR_H