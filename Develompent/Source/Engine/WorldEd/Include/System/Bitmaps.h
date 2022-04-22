/**
 * @file
 * @addtogroup WorldEd World editor
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef BITMAPS_H
#define BITMAPS_H

#include <wx/wx.h>
#include <string>

#include "Math/Color.h"

/**
 * @ingroup WorldEd
 * @brief A bitmap that has OS-specific loading hooks
 */
class WxBitmap : public wxBitmap
{
public:
	/**
	 * @brief Constructor
	 */
	WxBitmap();

	/**
	 * @brief Constructor
	 * 
	 * @param InImage WxWidget image
	 * @param InDepth Depth
	 */
	WxBitmap( const wxImage& InImage, int InDepth = -1 );

	/**
	 * @brief Constructor
	 * 
	 * @param InWidth Width image
	 * @param InHeight Height image
	 * @param InDepth Depth
	 */
	WxBitmap( int InWidth, int InHeight, int InDepth = -1 );

	/**
	 * @brief Constructor
	 * 
	 * @param InFilename The base filename, eg "<YourPath>/New" represents "<appBaseDir>/../../Engine/Editor/<YourPath>/New.png"
	 * @param InIsUseMask Is need use mask
	 * @param InColorMask Color mash
	 */
	WxBitmap( const std::wstring& InFilename, bool InIsUseMask = false, const FColor& InColorMask = FColor::black );

	/**
	 * @brief Tries to load the specified bitmap from the <appBaseDir>/../../Engine/Editor directory
	 *
	 * @param InFilename The base filename, eg "<YourPath>/New" represents "<appBaseDir>/../../Engine/Editor/<YourPath>/New.png"
	 * @param InIsUseMask Is need use mask
	 * @param InColorMask Color mash
	 * @return Return TRUE on success, else return FALSE
	 */
	virtual bool Load( const std::wstring& InFilename, bool InIsUseMask = false, const FColor& InColorMask = FColor::black );
};
#endif // !BITMAPS_H