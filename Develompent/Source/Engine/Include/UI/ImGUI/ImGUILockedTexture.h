/**
 * @file
 * @addtogroup UI User interface
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

#ifndef IMGUILOCKEDTEXTURE_H
#define IMGUILOCKEDTEXTURE_H

#include "Core/LEBuild.h"
#if WITH_IMGUI

#include "RHI/TypesRHI.h"
#include "Core/CoreDefines.h"

/**
 * @ingroup UI
 * @brief ImGUI locked texture 2D
 * 
 * You must use it struct in ImGui::Image*(). 
 * She contain handle of locked texture in CImGUIEngine and for locking texture use CImGUIEngine::LockTexture
 */
struct ImGUILockedTexture2D
{
	/**
	 * @brief Constructor
	 */
	FORCEINLINE ImGUILockedTexture2D()
		: handle( nullptr )
	{}

	/**
	 * @brief Constructor
	 * @param InTexture2D		Texture 2D
	 */
	FORCEINLINE ImGUILockedTexture2D( Texture2DRHIParamRef_t InTexture2D )
		: handle( InTexture2D )
	{}

	/**
	 * @brief Overloaded operator ptrint
	 * @return Return address
	 */
	FORCEINLINE operator ptrint() const
	{
		return ( ptrint )handle;
	}

	/**
	 * @brief Overloaded operator uptrint
	 * @return Return address
	 */
	FORCEINLINE operator uptrint() const
	{
		return ( uptrint )handle;
	}

	/**
	 * @brief Overloaded operator ==
	 * @param InRight	Right operand
	 *
	 * @return Return TRUE if is equal to InRight, otherwise will return FALSE
	 */
	FORCEINLINE bool operator==( const ImGUILockedTexture2D& InRight ) const
	{
		return handle == InRight.handle;
	}

	/**
	 * @brief Overloaded operator !=
	 * @param InRight	Right operand
	 *
	 * @return Return TRUE if isn't equal to InRight, otherwise will return FALSE
	 */
	FORCEINLINE bool operator!=( const ImGUILockedTexture2D& InRight ) const
	{
		return handle != InRight.handle;
	}

	Texture2DRHIParamRef_t		handle;		/**< Texture 2D */
};

#endif // WITH_IMGUI
#endif // !IMGUILOCKEDTEXTURE_H