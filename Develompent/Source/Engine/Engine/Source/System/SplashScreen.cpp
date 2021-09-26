#include "Containers/String.h"
#include "Misc/Misc.h"
#include "System/SplashScreen.h"

#if PLATFORM_WINDOWS

static std::wstring			GSplashScreenFileName;

void appShowSplash( const tchar* InSplashName )
{
	GSplashScreenFileName = appBaseDir() + FString::Format( TEXT( "/Content/Splash/%s" ), InSplashName );

}

void appHideSplash()
{}

void appSetSplashText( const ESplashTextType InType, const tchar* InText )
{}
#endif // PLATFORM_WINDOWS