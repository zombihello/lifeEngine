#include "Containers/String.h"
#include "Misc/Misc.h"
#include "System/SplashScreen.h"
#include "Misc/EngineGlobals.h"
#include "RHI/BaseRHI.h"
#include "WindowsThreading.h"
#include "System/Config.h"

static std::wstring			GSplashScreenFileName;
static HANDLE				GSplashScreenThread = nullptr;
static HBITMAP				GSplashScreenBitmap = nullptr;
static HWND					GSplashScreenWnd = nullptr;
static std::wstring			GSplashScreenText[ STT_NumTextTypes ];
static RECT					GSplashScreenTextRects[ STT_NumTextTypes ];
static HFONT				GSplashScreenSmallTextFontHandle = nullptr;
static HFONT				GSplashScreenNormalTextFontHandle = nullptr;
static FCriticalSection		GSplashScreenSynchronizationObject;
static FEvent*				GThreadInitSyncEvent = nullptr;

/**
 * Window's proc for splash screen
 */
LRESULT CALLBACK SplashScreenWindowProc( HWND InHWND, UINT InMessage, WPARAM InWParam, LPARAM InLParam )
{
	HDC				hdc;
	PAINTSTRUCT		ps;

	switch ( InMessage )
	{
	case WM_PAINT:
	{
		hdc = BeginPaint( InHWND, &ps );

		// Draw splash bitmap
		DrawState( hdc, DSS_NORMAL, NULL, ( LPARAM ) GSplashScreenBitmap, 0, 0, 0, 0, 0, DST_BITMAP );

		{
			// Take a critical section since another thread may be trying to set the splash text
			GSplashScreenSynchronizationObject.Lock();

			// Draw splash text
			for ( int32 curTypeIndex = 0; curTypeIndex < STT_NumTextTypes; ++curTypeIndex )
			{
				const std::wstring&		splashText = GSplashScreenText[ curTypeIndex ];
				const RECT&				textRect = GSplashScreenTextRects[ curTypeIndex ];

				if ( splashText.size() > 0 )
				{
					if ( curTypeIndex == STT_StartupProgress ||
						 curTypeIndex == STT_GameName )
					{
						SelectObject( hdc, GSplashScreenNormalTextFontHandle );
					}
					else
					{
						SelectObject( hdc, GSplashScreenSmallTextFontHandle );
					}

					SetBkColor( hdc, 0x00000000 );
					SetBkMode( hdc, TRANSPARENT );

					RECT		clientRect;
					GetClientRect( InHWND, &clientRect );

					// Draw background text passes
					const int32		numBGPasses = 8;
					for ( int32 curBGPass = 0; curBGPass < numBGPasses; ++curBGPass )
					{
						int32	bgXOffset, bgYOffset;
						switch ( curBGPass )
						{
						default:
						case 0:	bgXOffset = -1;		bgYOffset = 0;		break;
						case 2:	bgXOffset = -1;		bgYOffset = -1;		break;
						case 3:	bgXOffset = 0;		bgYOffset = -1;		break;
						case 4:	bgXOffset = 1;		bgYOffset = -1;		break;
						case 5:	bgXOffset = 1;		bgYOffset = 0;		break;
						case 6:	bgXOffset = 1;		bgYOffset = 1;		break;
						case 7:	bgXOffset = 0;		bgYOffset = 1;		break;
						case 8:	bgXOffset = -1;		bgYOffset = 1;		break;
						}

						SetTextColor( hdc, 0x00000000 );
						TextOut( hdc, textRect.left + bgXOffset, textRect.top + bgYOffset, splashText.c_str(), ( int32 )splashText.size() );
					}

					// Draw foreground text pass
					if ( curTypeIndex == STT_StartupProgress )
					{
						SetTextColor( hdc, RGB( 180, 180, 180 ) );
					}
					else if ( curTypeIndex == STT_GameName )
					{
						SetTextColor( hdc, RGB( 240, 240, 240 ) );
					}
					else
					{
						SetTextColor( hdc, RGB( 160, 160, 160 ) );
					}

					TextOut( hdc, textRect.left, textRect.top, splashText.c_str(), ( int32 )splashText.size() );
				}
			}

			GSplashScreenSynchronizationObject.Unlock();
		}

		EndPaint( InHWND, &ps );
	}
	break;

	case WM_DESTROY:
		PostQuitMessage( 0 );
		break;

	default:
		return DefWindowProc( InHWND, InMessage, InWParam, InLParam );
	}

	return 0;
}

/**
 * Splash screen thread entry function
 */
DWORD WINAPI SplashScreenThread( LPVOID InUnused )
{
	HINSTANCE		hInstance = GetModuleHandle( nullptr );
	WNDCLASS		wndClass;
	wndClass.style			= CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc	= ( WNDPROC )SplashScreenWindowProc;
	wndClass.cbClsExtra		= 0;
	wndClass.cbWndExtra		= 0;
	wndClass.hInstance		= hInstance;

	//wndClass.hIcon = LoadIcon( hInstance, MAKEINTRESOURCE( GIsEditor ? GEditorIcon : GGameIcon ) );
	//if ( wndClass.hIcon == NULL )
	{
		wndClass.hIcon		= LoadIcon( ( HINSTANCE )NULL, IDI_APPLICATION );
	}

	wndClass.hCursor		= LoadCursor( ( HINSTANCE )NULL, IDC_ARROW );
	wndClass.hbrBackground	= ( HBRUSH )GetStockObject( WHITE_BRUSH );
	wndClass.lpszMenuName	= nullptr;
	wndClass.lpszClassName	= TEXT( "SplashScreenClass" );

	if ( !RegisterClass( &wndClass ) )
	{
		return 0;
	}

	// Load splash screen image, display it and handle all window's messages
	GSplashScreenBitmap = ( HBITMAP )LoadImage( hInstance, ( LPCTSTR )GSplashScreenFileName.c_str(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE );
	if ( GSplashScreenBitmap )
	{
		BITMAP			bitmap;
		GetObjectW( GSplashScreenBitmap, sizeof( bitmap ), &bitmap );

		int32			screenPosX = ( GetSystemMetrics( SM_CXSCREEN ) - bitmap.bmWidth ) / 2;
		int32			screenPosY = ( GetSystemMetrics( SM_CYSCREEN ) - bitmap.bmHeight ) / 2;

		// Force the editor splash screen to show up in the taskbar and alt-tab lists
		uint32			windowStyle = WS_EX_APPWINDOW;

		GSplashScreenWnd = CreateWindowEx( windowStyle, wndClass.lpszClassName, TEXT( "SplashScreen" ), WS_BORDER | WS_POPUP,
										   screenPosX, screenPosY, bitmap.bmWidth, bitmap.bmHeight, nullptr, nullptr, hInstance, nullptr );

		// Setup font
		{
			HFONT			systemFontHandle = ( HFONT )GetStockObject( DEFAULT_GUI_FONT );

			// Create small font
			{
				LOGFONT		myFont;
				appMemzero( &myFont, sizeof( myFont ) );
				GetObjectW( systemFontHandle, sizeof( myFont ), &myFont );
				myFont.lfHeight = 10;

				GSplashScreenSmallTextFontHandle = CreateFontIndirect( &myFont );
				if ( GSplashScreenSmallTextFontHandle == NULL )
				{
					// Couldn't create font, so just use a system font
					GSplashScreenSmallTextFontHandle = systemFontHandle;
				}
			}

			// Create normal font
			{
				LOGFONT			myFont;
				appMemzero( &myFont, sizeof( myFont ) );
				GetObjectW( systemFontHandle, sizeof( myFont ), &myFont );
				myFont.lfHeight = 12;

				GSplashScreenNormalTextFontHandle = CreateFontIndirect( &myFont );
				if ( GSplashScreenNormalTextFontHandle == NULL )
				{
					// Couldn't create font, so just use a system font
					GSplashScreenNormalTextFontHandle = systemFontHandle;
				}
			}
		}

		// Setup bounds for game name
		GSplashScreenTextRects[ STT_GameName ].top						= bitmap.bmHeight - 60;
		GSplashScreenTextRects[ STT_GameName ].bottom					= bitmap.bmHeight - 30;
		GSplashScreenTextRects[ STT_GameName ].left						= 10;
		GSplashScreenTextRects[ STT_GameName ].right					= bitmap.bmWidth - 20;

		// Setup bounds for copyright info text
		GSplashScreenTextRects[ STT_CopyrightInfo ].top					= bitmap.bmHeight - 15;
		GSplashScreenTextRects[ STT_CopyrightInfo ].bottom				= bitmap.bmHeight - 10;
		GSplashScreenTextRects[ STT_CopyrightInfo ].left				= bitmap.bmWidth - 150;
		GSplashScreenTextRects[ STT_CopyrightInfo ].right				= bitmap.bmWidth;

		// Setup bounds for version info text
		GSplashScreenTextRects[ STT_VersionInfo1 ].top					= bitmap.bmHeight - 40;
		GSplashScreenTextRects[ STT_VersionInfo1 ].bottom				= bitmap.bmHeight - 20;
		GSplashScreenTextRects[ STT_VersionInfo1 ].left					= 10;
		GSplashScreenTextRects[ STT_VersionInfo1 ].right				= bitmap.bmWidth - 20;

		// Setup bounds for startup progress text
		GSplashScreenTextRects[ STT_StartupProgress ].top				= bitmap.bmHeight - 20;
		GSplashScreenTextRects[ STT_StartupProgress ].bottom			= bitmap.bmHeight;
		GSplashScreenTextRects[ STT_StartupProgress ].left				= 10;
		GSplashScreenTextRects[ STT_StartupProgress ].right				= bitmap.bmWidth - 20;

		// In the editor, we'll display loading info
		if ( GIsEditor )
		{
			// Set initial startup progress info
			appSetSplashText( STT_StartupProgress, TEXT( "Loading..." ) );

			// Set game name
			{
#if _WIN64
				const std::wstring				platformBitsString( TEXT( "64" ) );
#else
				const std::wstring				platformBitsString( TEXT( "32" ) );
#endif // _WIN64

				const std::wstring				finalGameName = FString::Format( TEXT( "WorldEd for %s (%s-bit, %s)" ), GGameName.c_str(), platformBitsString.c_str(), GRHI->GetRHIName() );			
				appSetSplashText( STT_GameName, finalGameName.c_str() );

				// Change the window text (which will be displayed in the taskbar)
				SetWindowText( GSplashScreenWnd, finalGameName.c_str() );
			}

			// Set version info and copyright
			appSetSplashText( STT_VersionInfo1, ENGINE_NAME TEXT( " (version " ) ENGINE_VERSION_STRING TEXT( ")" ) );
			appSetSplashText( STT_CopyrightInfo, TEXT( "(Ñ) Broken Singularity. All rights reserved" ) );			
		}

		if ( GThreadInitSyncEvent )
		{
			GThreadInitSyncEvent->Trigger();
		}

		if ( GSplashScreenWnd )
		{
			ShowWindow( GSplashScreenWnd, SW_SHOW );
			UpdateWindow( GSplashScreenWnd );

			MSG			message;
			while ( GetMessage( &message, nullptr, 0, 0 ) )
			{
				TranslateMessage( &message );
				DispatchMessage( &message );
			}
		}

		DeleteObject( GSplashScreenBitmap );
		GSplashScreenBitmap = nullptr;
	}

	UnregisterClass( wndClass.lpszClassName, hInstance );
	return 0;
}

void appShowSplash( const tchar* InSplashName )
{
	GSplashScreenFileName	= appBaseDir() + FString::Format( TEXT( "/Content/Splash/%s" ), InSplashName );
	GThreadInitSyncEvent	= GSynchronizeFactory->CreateSynchEvent( true );
	GSplashScreenThread		= CreateThread( nullptr, 0, ( LPTHREAD_START_ROUTINE )SplashScreenThread, nullptr, 0, nullptr );

	// Wait of open splash screen
	GThreadInitSyncEvent->Wait( INFINITE );
	GSynchronizeFactory->Destroy( GThreadInitSyncEvent );
	GThreadInitSyncEvent = nullptr;
}

void appHideSplash()
{
	if ( GSplashScreenThread )
	{
		if ( GSplashScreenWnd )
		{
			// Send message to splash screen window to destroy itself
			PostMessage( GSplashScreenWnd, WM_DESTROY, 0, 0 );
		}

		// Wait for splash screen thread to finish
		WaitForSingleObject( GSplashScreenThread, INFINITE );

		// Clean up
		CloseHandle( GSplashScreenThread );
		GSplashScreenThread = nullptr;
		GSplashScreenThread = nullptr;
		GSplashScreenWnd = nullptr;
	}
}

void appSetSplashText( const ESplashTextType InType, const tchar* InText )
{
	// We only want to bother drawing startup progress in the editor, since this information is
	// not interesting to an end-user (also, it's not usually localized properly.)
	if ( GSplashScreenThread )
	{
		// Only allow copyright text displayed while loading the game.  Editor displays all.
		if ( InType == STT_CopyrightInfo || InType == STT_GameName || GIsEditor )
		{
			{
				// Take a critical section since the splash thread may already be repainting using this text
				GSplashScreenSynchronizationObject.Lock();
				
				// Update splash text
				GSplashScreenText[ InType ] = InText;

				GSplashScreenSynchronizationObject.Unlock();
			}

			// Repaint the window
			InvalidateRect( GSplashScreenWnd, &GSplashScreenTextRects[ InType ], false );
		}
	}
}