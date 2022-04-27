#include "System/Bitmaps.h"
#include "Containers/String.h"
#include "Misc/Misc.h"

WxBitmap::WxBitmap()
{}

WxBitmap::WxBitmap( const wxImage& InImage, int InDepth /* = -1 */ )
	: wxBitmap( InImage, InDepth )
{}

WxBitmap::WxBitmap( int InWidth, int InHeight, int InDepth /* = -1 */ )
	: wxBitmap( InWidth, InHeight, InDepth )
{}

WxBitmap::WxBitmap( const std::wstring& InFilename, bool InIsUseMask /* = false */, const FColor& InColorMask /* = FColor::black */ )
{
	Load( InFilename, InIsUseMask, InColorMask );
}

bool WxBitmap::Load( const std::wstring& InFilename, bool InIsUseMask /* = false */, const FColor& InColorMask /* = FColor::black */ )
{
	static bool		bIsWxPNGHandleInited = false;
	if ( !bIsWxPNGHandleInited )
	{
		wxImage::AddHandler( new wxPNGHandler );
		bIsWxPNGHandleInited = true;
	}

	const bool		bResult = LoadFile( FString::Format( TEXT( "%s/Engine/Editor/%s.png" ), appBaseDir().c_str(), InFilename.c_str() ), wxBITMAP_TYPE_PNG );
	if ( InIsUseMask )
	{
		SetMask( new wxMask( *this, wxColor( InColorMask.r, InColorMask.g, InColorMask.b ) ) );
	}

	return bResult;
}