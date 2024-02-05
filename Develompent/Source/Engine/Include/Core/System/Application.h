/**
 * @file
 * @addtogroup Core Core
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

#ifndef APPLICATION_H
#define APPLICATION_H

#include <string>
#include "Core/Core.h"

#if WITH_EDITOR
	/**
	 * @ingroup Core
	 * @brief Typedef of pointer to tool function where the one can something do
	 */
	typedef bool ( *LEToolRunFn_t )();
#endif // WITH_EDITOR

/**
 * @ingroup Core
 * @brief Enum of application type
 */
enum EApplicationType
{
	AT_Game,		/**< Game */

#if WITH_EDITOR
	AT_Editor,		/**< Editor */
	AT_Cooker,		/**< Cooker */
	AT_Tool			/**< Tool */
#endif // WITH_EDITOR
};

/**
 * @ingroup Core
 * @brief Application
 */
class CApplication
{
public:
	/**
	 * @brief Constructor
	 */
	CApplication()
		: name( TEXT( "Example Game" ) )
		, shortName( TEXT( "ExampleGame" ) )
		, type( AT_Game )
#if WITH_EDITOR
		, LEToolRun( nullptr )
#endif // WITH_EDITOR
	{}

	/**
	 * @brief Get singleton instance
	 * @return Return singleton instance
	 */
	static FORCEINLINE CApplication& Get()
	{
		static CApplication		s_Application;
		return s_Application;
	}

	/**
	 * @brief Set application type
	 * @param InNewAppType		New application type
	 */
	FORCEINLINE void SetType( EApplicationType InNewAppType )
	{
		type = InNewAppType;
	}

	/**
	 * @brief Get application type
	 * @return Return application type
	 */
	FORCEINLINE EApplicationType GetType() const
	{
		return type;
	}

	/**
	 * @brief Set application name
	 * @note Example of application name: "Eleot: Episodic". It using for window title
	 * 
	 * @param InNewAppName	New application name
	 */
	FORCEINLINE void SetName( const std::wstring& InNewAppName )
	{
		name = InNewAppName;
	}

	/**
	 * @brief Set application short name
	 * @note Example of application short name: "EleotGame", it using for game directory name, log name and other 
	 *
	 * @param InNewAppShortName		New application short name
	 */
	FORCEINLINE void SetShortName( const std::wstring& InNewAppShortName )
	{
		shortName = InNewAppShortName;
	}

	/**
	 * @brief Get application name
	 * @note Example of application name: "Eleot: Episodic". It using for window title
	 * 
	 * @return Return application name
	 */
	FORCEINLINE const std::wstring& GetName() const
	{
		return name;
	}

	/**
	 * @brief Get application short name
	 * @note Example of application short name: "EleotGame", it using for game directory name, log name and other 
	 * 
	 * @return Return application short name
	 */
	FORCEINLINE const std::wstring& GetShortName() const
	{
		return shortName;
	}

#if WITH_EDITOR
	/**
	 * @brief Set tool run function where the one can something do
	 * @param InToolRunFn	Pointer to tool function where the one can something do
	 */
	FORCEINLINE void SetToolRunFn( LEToolRunFn_t InToolRunFn )
	{
		LEToolRun = InToolRunFn;
	}

	/**
	 * @brief Get tool run function where the one can something do
	 * @return Return pointer to tool function where the one can something do. If not set returns NULL
	 */
	FORCEINLINE LEToolRunFn_t GetToolRunFn() const
	{
		return LEToolRun;
	}
#endif // WITH_EDITOR

private:
	std::wstring			name;		/**< Application name. For example: "Eleot: Episodic", it using for window title */
	std::wstring			shortName;	/**< Application short name. For example: "EleotGame", it using for game directory name, log name and other */
	EApplicationType		type;		/**< Application type */

#if WITH_EDITOR
	LEToolRunFn_t			LEToolRun;	/**< Pointer to tool function where the one can something do */
#endif // WITH_EDITOR
};

#endif // !APPLICATION_H