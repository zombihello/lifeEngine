/**
 * @file
 * @addtogroup UI User interface
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef IMGUILOCKEDTEXTURE_H
#define IMGUILOCKEDTEXTURE_H

#include "LEBuild.h"
#if WITH_IMGUI

#include "RHI/TypesRHI.h"
#include "CoreDefines.h"

/**
 * @ingroup UI
 * @brief ImGUI locked texture 2D
 * 
 * You must use it struct in ImGui::Image*(). 
 * She contain handle of locked texture in CImGUIEngine and for locking texture use CImGUIEngine::LockTexture
 */
struct SImGUILockedTexture2D
{
	/**
	 * @brief Constructor
	 */
	FORCEINLINE SImGUILockedTexture2D()
		: handle( nullptr )
	{}

	/**
	 * @brief Constructor
	 * @param InTexture2D		Texture 2D
	 */
	FORCEINLINE SImGUILockedTexture2D( Texture2DRHIParamRef_t InTexture2D )
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
	FORCEINLINE bool operator==( const SImGUILockedTexture2D& InRight ) const
	{
		return handle == InRight.handle;
	}

	/**
	 * @brief Overloaded operator !=
	 * @param InRight	Right operand
	 *
	 * @return Return TRUE if isn't equal to InRight, otherwise will return FALSE
	 */
	FORCEINLINE bool operator!=( const SImGUILockedTexture2D& InRight ) const
	{
		return handle != InRight.handle;
	}

	Texture2DRHIParamRef_t		handle;		/**< Texture 2D */
};

#endif // WITH_IMGUI
#endif // !IMGUILOCKEDTEXTURE_H