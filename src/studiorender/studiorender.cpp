/**
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

#include "pch_studiorender.h"
#include "filesystem/ifilesystem.h"
#include "studiorender/istudiorender.h"
#include "studiorender/renderthread.h"

/**
 * @ingroup studiorender
 * @brief Studio render
 */
class CStudioRender : public CBaseAppSystem<IStudioRender>
{
public:
	/**
	 * @brief Constructor
	 */
	CStudioRender();

	/**
	 * @brief Connect application system
	 *
	 * @param pFactory		Pointer to interface factory
	 * @return Return TRUE if successes application system is connected, otherwise return FALSE
	 */
	virtual bool Connect( CreateInterfaceFn_t pFactory ) override;

	/**
	 * @brief Disconnect application system
	 */
	virtual void Disconnect() override;

	/**
	 * @brief Query interface
	 *
	 * Here's where systems can access other interfaces implemented by this object
	 *
	 * @param pInterfaceName	Interface name
	 * @return Return pointer to interface, if doesn't implement the requested interface return NULL
	 */
	virtual void* QueryInterface( const achar* pInterfaceName ) override;

	/**
	 * @brief Init application system
	 * @return Return TRUE if application system is inited
	 */
	virtual bool Init() override;

	/**
	 * @brief Shutdown application system
	 */
	virtual void Shutdown() override;

	/**
	 * @brief Set Studio API
	 * Sets which API we should be using. Has to be done before connect
	 *
	 * @param pStudioAPIDLL		Studio API module name
	 * @return Return TRUE if Studio API successfully set, otherwise returns FALSE
	 */
	virtual void SetStudioAPI( const achar* pStudioAPIDLL ) override;

	/**
	 * @brief Begin render frame
	 * @param pStudioViewport	Studio viewport
	 */
	virtual void BeginFrame( IStudioViewport* pStudioViewport ) override;

	/**
	 * @brief End render frame
	 * @param pStudioViewport	Studio viewport
	 */
	virtual void EndFrame( IStudioViewport* pStudioViewport ) override;

	/**
	 * @brief Get command buffer of the render thread
	 * @return Return pointer to command buffer of render thread. If return NULL it's mean what StudioRender don't use render thread
	 */
	virtual IStudioCmdBuffer* GetCommandBuffer() const override;

	/**
	 * @brief Is current thread is the render
	 * @return Return TRUE if it called from the render thread or render thread isn't use, otherwise returns FALSE
	 */
	virtual bool IsInRenderThread() const override;

private:
	/**
	 * @brief Creates the studio API implementation
	 * 
	 * @param pStudioAPIDLL		Studio API module name
	 * @return Return Studio API interface factory if module loaded, otherwise returns NULL
	 */
	CreateInterfaceFn_t CreateStudioAPI( const achar* pStudioAPIDLL );

	/**
	 * @brief Destroys the studio API implementation
	 */
	void DestroyStudioAPI();

	std::string			studioAPIDLLName;		/**< Studio API dll name */
	dllHandle_t			studioAPIHandle;		/**< Studio API handle */
	CreateInterfaceFn_t	pStudioAPIFactory;		/**< StudioAPI factory */
	CreateInterfaceFn_t	pAppSystemFactory;		/**< Application systems factory. It used for connect sutdiorender's submodules (e.g: studioapi) */
};

EXPOSE_SINGLE_INTERFACE( CStudioRender, IStudioRender, STUDIORENDER_INTERFACE_VERSION );


/*
==================
CStudioRender::CStudioRender
==================
*/
CStudioRender::CStudioRender()
	: studioAPIHandle( nullptr )
	, pStudioAPIFactory( nullptr )
	, pAppSystemFactory( nullptr )
{}

/*
==================
CStudioRender::Connect
==================
*/
bool CStudioRender::Connect( CreateInterfaceFn_t pFactory )
{
	// Connect StdLib
	if ( !ConnectStdLib( pFactory ) )
	{
		return false;
	}

	// Before connect studiorender must be Studio API loaded by CStudioRender::SetStudioAPI
	if ( !pStudioAPIFactory )
	{
		Warning( "The studiorender requires Studio API to run!" );
		return false;
	}

	// Get Studio API
	g_pStudioAPI = ( IStudioAPI* )pStudioAPIFactory( STUDIOAPI_INTERFACE_VERSION );
	if ( !g_pStudioAPI )
	{
		return false;
	}

	pAppSystemFactory = pFactory;
	return true;
}

/*
==================
CStudioRender::Disconnect
==================
*/
void CStudioRender::Disconnect()
{
	// Disconnect StdLib
	DisconnectStdLib();
	g_pStudioAPI		= nullptr;
	pAppSystemFactory	= nullptr;
}

/*
==================
CStudioRender::QueryInterface
==================
*/
void* CStudioRender::QueryInterface( const achar* pInterfaceName )
{
	// Returns various interfaces supported by the Studio API dll
	void*	pInterface = nullptr;
	if ( pStudioAPIFactory )
	{
		pInterface = pStudioAPIFactory( pInterfaceName );
	}

	// Otherwise if not found look in our factory
	if ( !pInterface )
	{
		CreateInterfaceFn_t		pFactory = Sys_GetFactoryThis();
		pInterface = pFactory( pInterfaceName );
	}

	return pInterface;
}

/*
==================
CStudioRender::Init
==================
*/
bool CStudioRender::Init()
{
	// Register cvars
	ConVar_Register();
	
	// Initialize Studio API
	if ( !g_pStudioAPI->Init( pAppSystemFactory ) )
	{
		Warning( "CStudioRender: Failed to init Studio API '%s'", studioAPIDLLName.c_str() );
		DestroyStudioAPI();
		return false;
	}

	// Start the render thread
	R_StartRenderThread();

	// We are done!
	return true;
}

/*
==================
CStudioRender::Shutdown
==================
*/
void CStudioRender::Shutdown()
{
	// Stop the render thread
	R_StopRenderThread();

	// Destroy Studio API and unregister cvars
	DestroyStudioAPI();
	ConVar_Unregister();
}

/*
==================
CStudioRender::SetStudioAPI
==================
*/
void CStudioRender::SetStudioAPI( const achar* pStudioAPIDLL )
{
	// We cannot set the studio API twice
	if ( pStudioAPIFactory )
	{
		Error( "CStudioRender: Cannot set the Studio API twice!" );
	}

	// Set default StudioAPI module if pStudioAPIDLL isn't valid
	if ( !pStudioAPIDLL )
	{
#if PLATFORM_WINDOWS
		pStudioAPIDLL = "studioapi_dx11" DLL_EXT_STRING;
#else
	#error Unknown platform
#endif // PLATFORM_WINDOWS
	}

	// Load Studio API module
	studioAPIDLLName	= pStudioAPIDLL;
	pStudioAPIFactory	= CreateStudioAPI( pStudioAPIDLL );
	if ( !pStudioAPIFactory )
	{
		DestroyStudioAPI();
	}
}

/*
==================
CStudioRender::BeginFrame
==================
*/
void CStudioRender::BeginFrame( IStudioViewport* pStudioViewport )
{
	g_pStudioAPI->BeginDrawingViewport( pStudioViewport );
	pStudioViewport->GetSurface()->ClearSurface( CColor( 255, 0, 0 ) );
}

/*
==================
CStudioRender::EndFrame
==================
*/
void CStudioRender::EndFrame( IStudioViewport* pStudioViewport )
{
	g_pStudioAPI->EndDrawingViewport( pStudioViewport, true, false );
}

/*
==================
CStudioRender::GetCommandBuffer
==================
*/
IStudioCmdBuffer* CStudioRender::GetCommandBuffer() const
{
	return &g_StudioCmdBuffer;
}

/*
==================
CStudioRender::IsInRenderThreads
==================
*/
bool CStudioRender::IsInRenderThread() const
{
	return ( g_RenderThreadHandle == INVALID_THREAD_HANDLE ) || ( Sys_GetCurrentThreadHandle() == g_RenderThreadHandle );
}

/*
==================
CStudioRender::StudioAPI_Load
==================
*/
CreateInterfaceFn_t CStudioRender::CreateStudioAPI( const achar* pStudioAPIDLL )
{
	// Do nothing if pStudioAPIDLL isn't valid
	if ( !pStudioAPIDLL )
	{
		return nullptr;
	}

	// Clean up the old Studio API
	DestroyStudioAPI();

	// Load the new Studio API
	studioAPIHandle = Sys_DLL_LoadModule( pStudioAPIDLL );
	if ( !studioAPIHandle )
	{
		Warning( "CStudioRender: Failed to load Studio API '%s'", pStudioAPIDLL );
		return nullptr;
	}

	// Get our class factory methods
	CreateInterfaceFn_t		pFactory = Sys_GetFactory( studioAPIHandle );
	if ( !pFactory )
	{
		Warning( "CStudioRender: Failed to get interface factory from '%s'", pStudioAPIDLL );
		DestroyStudioAPI();
		return nullptr;
	}

	// We are done!
	Msg( "CStudioRender: Studio API '%s' loaded", pStudioAPIDLL );
	return pFactory;
}

/*
==================
CStudioRender::DestroyStudioAPI
==================
*/
void CStudioRender::DestroyStudioAPI()
{
	if ( studioAPIHandle )
	{
		// Shutdown Studio API if pointer is valid
		if ( g_pStudioAPI )
		{
			g_pStudioAPI->Shutdown();
		}

		Msg( "CStudioRender: Studio API '%s' unloaded", studioAPIDLLName.c_str() );
		Sys_DLL_UnloadModule( studioAPIHandle );

		studioAPIDLLName.clear();
		studioAPIHandle = nullptr;
		pStudioAPIFactory = nullptr;
		g_pStudioAPI = nullptr;
	}
}