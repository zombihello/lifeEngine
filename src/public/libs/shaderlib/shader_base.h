/**
 * @file
 * @addtogroup shaderlib shaderlib
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

#ifndef SHADER_BASE_H
#define SHADER_BASE_H

#include "materialsystem/ishader.h"

/**
 * @ingroup shaderslibrary
 * @brief Macro of begin implementation shader with custom flags
 * 
 * @param Name			Shader name
 * @param HelpString	Help message
 * @param Flags			Shader flags (see EShaderFlags)
 */
#define BEGIN_SHADER_FLAGS( Name, HelpString, Flags ) \
	namespace Name \
	{ \
		/** Shader help message */ \
		static const achar*					s_pHelpString = HelpString; \
		/** Shader name */ \
		static const achar*					s_pName = #Name; \
		/** Shader flags (see EShaderFlags) */ \
		static uint32						s_Flags = Flags; \
		/** Shader parameters */ \
		static std::vector<ShaderParam*>	s_ShaderParams; \

/**
 * @ingroup shaderslibrary
 * @brief Macro of begin implementation shader
 *
 * @param Name			Shader name
 * @param HelpString	Help message
 */
#define BEGIN_SHADER( Name, HelpString ) \
	BEGIN_SHADER_FLAGS( Name, HelpString, 0 )

/**
 * @ingroup shaderslibrary
 * @brief Macro of begin shader parameters section
 */
#define BEGIN_SHADER_PARAMS

/**
 * @ingroup shaderslibrary
 * @brief Macro of create shader parameter with custom flags
 * 
 * @param Name			Parameter name
 * @param Type			Parameter type
 * @param DefaultValue	Default value of parameter
 * @param HelpString	Help message for this parameter
 * @param Flags			Parameter flags
 */
#define SHADER_PARAM_FLAGS( Name, Type, DefaultValue, HelpString, Flags ) \
	static ShaderParam Name{ #Name, Type, DefaultValue, HelpString, Flags };

/**
 * @ingroup shaderslibrary
 * @brief Macro of create shader parameter
 * 
 * @param Name			Parameter name
 * @param Type			Parameter type
 * @param DefaultValue	Default value of parameter
 * @param Help			Help message for this parameter
 */
#define SHADER_PARAM( Name, Type, DefaultValue, HelpString ) \
	static ShaderParam Name{ #Name, Type, DefaultValue, HelpString, 0 };

/**
 * @ingroup shaderslibrary
 * @brief Macro of end shader parameters section
 */
#define END_SHADER_PARAMS \
	class CShader : public CBaseShader \
	{ \
	public: \
		/** Get shader name */ \
		virtual const achar* GetName() const override \
		{ \
			return s_pName; \
		} \
		/** Get shader help message */ \
		virtual const achar* GetHelp() const override \
		{ \
			return s_pHelpString; \
		} \
		/** Get shader flags */ \
		virtual uint32 GetFlags() const override \
		{ \
			return s_Flags; \
		} \
		/** Get number of shader parameters */ \
		virtual uint32 GetNumParams() const override \
		{ \
			return ( uint32 )s_ShaderParams.size(); \
		} \
		/** Get shader parameter by index */ \
		virtual ShaderParam GetParam( uint32 index ) const override \
		{ \
			Assert( index < ( uint32 )s_ShaderParams.size() ); \
			return *s_ShaderParams[index]; \
		}

/**
 * @ingroup shaderslibrary
 * @brief Macro of set fallback shader
 */
#define SHADER_FALLBACK \
	/** Get fallback shader */ \
	virtual const achar* GetFallbackShader() const override

/**
 * @ingroup shaderslibrary
 * @brief Macro of end implementation shader
 */
#define END_SHADER \
		}; \
		static CShader s_ShaderInstance; \
	} // Namespace

/**
 * @ingroup shaderslibrary
 * @brief Base shader
 */
class CBaseShader : public IShader
{
public:
	/**
	 * @brief Constructor
	 */
	CBaseShader();

	/**
	 * @brief Get fallback shader 
	 * @return If StudioAPI isn't support this shader return fallback shader name, otherwise returns NULL
	 */
	virtual const achar* GetFallbackShader() const override;
};

#endif // !SHADER_BASE_H