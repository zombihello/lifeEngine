#include "Misc/EngineGlobals.h"
#include "Misc/Misc.h"
#include "Misc/FileTools.h"
#include "System/SplashScreen.h"
#include "System/Config.h"
#include "RHI/BaseRHI.h"
#include "WindowsThreading.h"

#if WITH_EDITOR
#include "WorldEd.h"
#endif // WITH_EDITOR

static std::wstring		s_SplashScreenFileName;
static HANDLE			s_SplashScreenThread = nullptr;
static HBITMAP			s_SplashScreenBitmap = nullptr;
static HWND				s_SplashScreenWnd = nullptr;
static std::wstring		s_SplashScreenText[STT_NumTextTypes];
static RECT				s_SplashScreenTextRects[STT_NumTextTypes];
static HFONT			s_SplashScreenSmallTextFontHandle = nullptr;
static HFONT			s_SplashScreenNormalTextFontHandle = nullptr;
static CMutex			s_SplashScreenSynchronizationObject;
static CEvent*			s_ThreadInitSyncEvent = nullptr;

/*
==================
SplashScreenWindowProc
==================
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
		DrawState( hdc, DSS_NORMAL, NULL, ( LPARAM ) s_SplashScreenBitmap, 0, 0, 0, 0, 0, DST_BITMAP );

		{
			// Take a critical section since another thread may be trying to set the splash text
			CScopeLock		scopeLock( s_SplashScreenSynchronizationObject );

			// Draw splash text
			for ( int32 curTypeIndex = 0; curTypeIndex < STT_NumTextTypes; ++curTypeIndex )
			{
				const std::wstring&		splashText = s_SplashScreenText[ curTypeIndex ];
				const RECT&				textRect = s_SplashScreenTextRects[ curTypeIndex ];

				if ( splashText.size() > 0 )
				{
					if ( curTypeIndex == STT_StartupProgress ||
						 curTypeIndex == STT_GameName )
					{
						SelectObject( hdc, s_SplashScreenNormalTextFontHandle );
					}
					else
					{
						SelectObject( hdc, s_SplashScreenSmallTextFontHandle );
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

/*
==================
SplashScreenThread
==================
*/
DWORD WINAPI SplashScreenThread( LPVOID InUnused )
{
	bool					bNeedDestroyIcon = true;
	HINSTANCE				hInstance = GetModuleHandle( nullptr );
	WNDCLASS				wndClass;
	wndClass.style			= CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc	= ( WNDPROC )SplashScreenWindowProc;
	wndClass.cbClsExtra		= 0;
	wndClass.cbWndExtra		= 0;
	wndClass.hInstance		= hInstance;

	const uint32			defaultExeIcon = 0;		// In EXE default icon its 0
	wndClass.hIcon			= ExtractIcon( hInstance, L_GetExecutablePath(), defaultExeIcon );
	if (! wndClass.hIcon )
	{
		wndClass.hIcon		= LoadIcon( ( HINSTANCE )NULL, IDI_APPLICATION );
		bNeedDestroyIcon	= false;
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
	s_SplashScreenBitmap = ( HBITMAP )LoadImage( hInstance, ( LPCTSTR )s_SplashScreenFileName.c_str(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE );
	
	// If splash screen not loaded - try load engine splash
	if ( !s_SplashScreenBitmap )
	{
		if ( g_IsEditor )
		{
			s_SplashScreenFileName = Sys_BaseDir() + TEXT( "Engine/Splash/EdSplash.bmp" );
		}
		else
		{
			s_SplashScreenFileName = Sys_BaseDir() + TEXT( "Engine/Splash/Splash.bmp" );
		}

		s_SplashScreenBitmap = ( HBITMAP )LoadImage( hInstance, ( LPCTSTR )s_SplashScreenFileName.c_str(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE );
	}

	if ( s_SplashScreenBitmap )
	{
		BITMAP			bitmap;
		GetObjectW( s_SplashScreenBitmap, sizeof( bitmap ), &bitmap );

		int32			screenPosX = ( GetSystemMetrics( SM_CXSCREEN ) - bitmap.bmWidth ) / 2;
		int32			screenPosY = ( GetSystemMetrics( SM_CYSCREEN ) - bitmap.bmHeight ) / 2;

		// Force the editor splash screen to show up in the taskbar and alt-tab lists
		uint32			windowStyle = WS_EX_APPWINDOW;

		s_SplashScreenWnd = CreateWindowEx( windowStyle, wndClass.lpszClassName, TEXT( "SplashScreen" ), WS_BORDER | WS_POPUP,
										   screenPosX, screenPosY, bitmap.bmWidth, bitmap.bmHeight, nullptr, nullptr, hInstance, nullptr );

		// Setup font
		{
			HFONT			systemFontHandle = ( HFONT )GetStockObject( DEFAULT_GUI_FONT );

			// Create small font
			{
				LOGFONT		myFont;
				Memory::Memzero( &myFont, sizeof( myFont ) );
				GetObjectW( systemFontHandle, sizeof( myFont ), &myFont );
				myFont.lfHeight = 10;

				s_SplashScreenSmallTextFontHandle = CreateFontIndirect( &myFont );
				if ( s_SplashScreenSmallTextFontHandle == NULL )
				{
					// Couldn't create font, so just use a system font
					s_SplashScreenSmallTextFontHandle = systemFontHandle;
				}
			}

			// Create normal font
			{
				LOGFONT			myFont;
				Memory::Memzero( &myFont, sizeof( myFont ) );
				GetObjectW( systemFontHandle, sizeof( myFont ), &myFont );
				myFont.lfHeight = 12;

				s_SplashScreenNormalTextFontHandle = CreateFontIndirect( &myFont );
				if ( s_SplashScreenNormalTextFontHandle == NULL )
				{
					// Couldn't create font, so just use a system font
					s_SplashScreenNormalTextFontHandle = systemFontHandle;
				}
			}
		}

		// Setup bounds for game name
		s_SplashScreenTextRects[ STT_GameName ].top						= bitmap.bmHeight - 60;
		s_SplashScreenTextRects[ STT_GameName ].bottom					= bitmap.bmHeight - 30;
		s_SplashScreenTextRects[ STT_GameName ].left						= 10;
		s_SplashScreenTextRects[ STT_GameName ].right					= bitmap.bmWidth - 20;

		// Setup bounds for copyright info text
		s_SplashScreenTextRects[ STT_CopyrightInfo ].top					= bitmap.bmHeight - 15;
		s_SplashScreenTextRects[ STT_CopyrightInfo ].bottom				= bitmap.bmHeight - 10;
		s_SplashScreenTextRects[ STT_CopyrightInfo ].left				= bitmap.bmWidth - 150;
		s_SplashScreenTextRects[ STT_CopyrightInfo ].right				= bitmap.bmWidth;

		// Setup bounds for version info text
		s_SplashScreenTextRects[ STT_VersionInfo1 ].top					= bitmap.bmHeight - 40;
		s_SplashScreenTextRects[ STT_VersionInfo1 ].bottom				= bitmap.bmHeight - 20;
		s_SplashScreenTextRects[ STT_VersionInfo1 ].left					= 10;
		s_SplashScreenTextRects[ STT_VersionInfo1 ].right				= bitmap.bmWidth - 20;

		// Setup bounds for startup progress text
		s_SplashScreenTextRects[ STT_StartupProgress ].top				= bitmap.bmHeight - 20;
		s_SplashScreenTextRects[ STT_StartupProgress ].bottom			= bitmap.bmHeight;
		s_SplashScreenTextRects[ STT_StartupProgress ].left				= 10;
		s_SplashScreenTextRects[ STT_StartupProgress ].right				= bitmap.bmWidth - 20;

		// In the editor, we'll display loading info
#if WITH_EDITOR
		if ( g_IsEditor )
		{
			// Set initial startup progress info
			Sys_SetSplashText( STT_StartupProgress, TEXT( "Loading..." ) );

			// Set game name
			{
				std::wstring		finalGameName = Sys_GetWorldEdName();				
				Sys_SetSplashText( STT_GameName, finalGameName.c_str() );

				// Change the window text (which will be displayed in the taskbar)
				SetWindowText( s_SplashScreenWnd, finalGameName.c_str() );
			}

			// Set version info and copyright
			Sys_SetSplashText( STT_VersionInfo1, ENGINE_NAME TEXT( " (version " ) ENGINE_VERSION_STRING TEXT( ")" ) );
			Sys_SetSplashText( STT_CopyrightInfo, TEXT( "(C) Broken Singularity. All rights reserved" ) );			
		}
#endif // WITH_EDITOR

		if ( s_SplashScreenWnd )
		{
			ShowWindow( s_SplashScreenWnd, SW_SHOW );
			UpdateWindow( s_SplashScreenWnd );

			if ( s_ThreadInitSyncEvent )
			{
				s_ThreadInitSyncEvent->Trigger();
			}

			MSG			message;
			while ( GetMessage( &message, nullptr, 0, 0 ) )
			{
				TranslateMessage( &message );
				DispatchMessage( &message );
			}
		}

		DeleteObject( s_SplashScreenBitmap );
		s_SplashScreenBitmap = nullptr;
		if ( bNeedDestroyIcon )
		{
			DestroyIcon( wndClass.hIcon );
		}
	}

	UnregisterClass( wndClass.lpszClassName, hInstance );
	return 0;
}

/*
==================
Sys_ShowSplash
==================
*/
void Sys_ShowSplash( const tchar* InSplashName )
{
	if ( !g_IsCommandlet )
	{
		s_SplashScreenFileName	= Sys_GameDir() + L_Sprintf( PATH_SEPARATOR TEXT( "Splash" ) PATH_SEPARATOR TEXT( "%s" ), InSplashName );
		s_ThreadInitSyncEvent	= new CEvent( true );
		s_SplashScreenThread	= CreateThread( nullptr, 0, ( LPTHREAD_START_ROUTINE ) SplashScreenThread, nullptr, 0, nullptr );

		// Wait of open splash screen
		s_ThreadInitSyncEvent->Wait( INFINITE );
		delete s_ThreadInitSyncEvent;
		s_ThreadInitSyncEvent = nullptr;
	}
}

/*
==================
Sys_HideSplash
==================
*/
void Sys_HideSplash()
{
	if ( s_SplashScreenThread )
	{
		if ( s_SplashScreenWnd )
		{
			// Send message to splash screen window to destroy itself
			PostMessage( s_SplashScreenWnd, WM_DESTROY, 0, 0 );
		}

		// Wait for splash screen thread to finish
		WaitForSingleObject( s_SplashScreenThread, INFINITE );

		// Clean up
		CloseHandle( s_SplashScreenThread );
		s_SplashScreenThread = nullptr;
		s_SplashScreenThread = nullptr;
		s_SplashScreenWnd = nullptr;
	}
}

/*
==================
Sys_SetSplashText
==================
*/
void Sys_SetSplashText( const ESplashTextType InType, const tchar* InText )
{
	// We only want to bother drawing startup progress in the editor, since this information is
	// not interesting to an end-user (also, it's not usually localized properly.)
	if ( s_SplashScreenThread )
	{
		// Only allow copyright text displayed while loading the game.  Editor displays all.
		if ( InType == STT_CopyrightInfo || InType == STT_GameName || g_IsEditor )
		{
			{
				// Take a critical section since the splash thread may already be repainting using this text
				CScopeLock			scopeLock( s_SplashScreenSynchronizationObject );
				
				// Update splash text
				s_SplashScreenText[ InType ] = InText;
			}

			// Repaint the window
			InvalidateRect( s_SplashScreenWnd, &s_SplashScreenTextRects[ InType ], false );
		}
	}
}