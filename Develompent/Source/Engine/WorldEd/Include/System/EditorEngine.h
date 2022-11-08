/**
 * @file
 * @addtogroup WorldEd WorldEd
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef EDITORENGINE_H
#define EDITORENGINE_H

#include <vector>

#include "Math/Color.h"
#include "Logger/LoggerMacros.h"
#include "Misc/SharedPointer.h"
#include "System/BaseEngine.h"
#include "System/EditorConstraints.h"
#include "Render/Viewport.h"
#include "WorldEd.h"

// Windows
#include "Windows/ActorClassesWindow.h"
#include "Windows/ActorPropertiesWindow.h"
#include "Windows/ContentBrowserWindow.h"
#include "Windows/ExplorerLevelWindow.h"
#include "Windows/LogsWindow.h"
#include "Windows/ViewportWindow.h"

/**
 * @ingroup WorldEd
 * Class of editor engine
 */
class CEditorEngine : public CBaseEngine
{
	DECLARE_CLASS( CEditorEngine, CBaseEngine )

public:
	/**
	 * Constructor
	 */
	CEditorEngine();

	/**
	 * Destructor
	 */
	virtual ~CEditorEngine();

	/**
	 * Initialize engine
	 */
	virtual void Init() override;

	/**
	 * Update logic of engine
	 *
	 * @param[in] InDeltaTime Delta time
	 */
	virtual void Tick( float InDeltaSeconds ) override;

	/**
	 * Shutdown engine
	 */
	virtual void Shutdown() override;

	/**
	 * Print log to log widget
	 * 
	 * @param InLogType Log type
	 * @param InMessage Message of log
	 */
	virtual void PrintLogToWidget( ELogType InLogType, const tchar* InMessage );

	/**
	 * @brief Process event
	 *
	 * @param InWindowEvent Window event
	 */
	virtual void ProcessEvent( struct SWindowEvent& InWindowEvent ) override;

	/**
	 * Add viewport to render list
	 *
	 * @param[in] InViewport Viewport
	 */
	FORCEINLINE void AddViewport( const TRefCountPtr<class CViewport>& InViewport )
	{
		viewports.push_back( InViewport );
	}

	/**
	 * Remove viewport from render list
	 *
	 * @param[in] InViewport Viewport
	 */
	FORCEINLINE void RemoveViewport( const TRefCountPtr<class CViewport>& InViewport )
	{
		for ( uint32 index = 0, count = ( uint32 )viewports.size(); index < count; ++index )
		{
			const TRefCountPtr<class CViewport>&	viewport = viewports[index];
			if ( viewport == InViewport )
			{
				viewports.erase( viewports.begin() + index );
				return;
			}
		}
	}

	/**
	 * Load map
	 *
	 * @param[in] InMap Path to map
	 * @param[out] OutError Error of loading map
	 * @return Return true if map loaded successed, else return false
	 */
	virtual bool LoadMap( const std::wstring& InMap, std::wstring& OutError ) override;

	/**
	 * Save map
	 * 
	 * @param[in] InMap Path to map
	 * @param[out] OutError Error of saving map
	 * @return Return true if map saved successed, else return false
	 */
	virtual bool SaveMap( const std::wstring& InMap, std::wstring& OutError );

	/**
	 * Serialize TOC
	 *
	 * @param InIsSave	Is save TOC file
	 * @return Return TRUE if TOC file serialized seccussed, else return FALSE
	 */
	bool SerializeTOC( bool InIsSave = false );

	/**
	 * Get editor name
	 * 
	 * @return Return editor name
	 */
	virtual std::wstring GetEditorName() const;

	/**
	 * Get editor constraints
	 * @return Return editor constraints
	 */
	FORCEINLINE CEditorConstraints& GetConstraints()
	{
		return constraints;
	}

	/**
	 * Get editor constraints
	 * @return Return editor constraints
	 */
	FORCEINLINE const CEditorConstraints& GetConstraints() const
	{
		return constraints;
	}

	/**
	 * Get game content directory
	 * @return Return game content directory
	 */
	FORCEINLINE std::wstring GetGameContentDir() const
	{
		return CString::Format( TEXT( "%s/Content/" ), appGameDir().c_str() );
	}

	/**
	 * Get engine content directory
	 * @return Return engine content directory
	 */
	FORCEINLINE std::wstring GetEngineContentDir() const
	{
		return CString::Format( TEXT( "%s/Engine/Content/" ), appBaseDir().c_str() );
	}

	/**
	 * Get selection color
	 * @return Return selection color
	 */
	FORCEINLINE CColor GetSelectionColor() const
	{
		return CColor( 32, 0, 128, 255 );
	}

	/**
	 * Set current editor mode
	 * @param InNewEditorMode	New editor mode
	 */
	FORCEINLINE void SetEditorMode( EEditorMode InNewEditorMode )
	{
		currentEditorMode = InNewEditorMode;
		SEditorDelegates::onEditorModeChanged.Broadcast( InNewEditorMode );
	}

	/**
	 * Get current editor mode
	 * @return Return current editor mode
	 */
	FORCEINLINE EEditorMode GetEditorMode() const
	{
		return currentEditorMode;
	}

	/**
	 * @brief Get actor classes window
	 * @return Return reference to actor classes window
	 */
	FORCEINLINE CActorClassesWindow& GetActorClassesWindow()
	{
		return actorClassesWindow;
	}

	/**
	 * @brief Get actor properties window
	 * @return Return reference to actor properties window
	 */
	FORCEINLINE CActorPropertiesWindow& GetActorPropertiesWindow()
	{
		return actorPropertiesWindow;
	}

	/**
	 * @brief Get content browser window
	 * @return Return reference to content browser window
	 */
	FORCEINLINE CContentBrowserWindow& GetContentBrowserWindow()
	{
		return contentBrowserWindow;
	}

	/**
	 * @brief Get explorer level window
	 * @return Return reference to explorer level window
	 */
	FORCEINLINE CExplorerLevelWindow& GetExplorerLevelWindow()
	{
		return explorerLevelWindow;
	}

	/**
	 * @brief Get logs window
	 * @return Return reference to logs window
	 */
	FORCEINLINE CLogsWindow& GetLogsWindow()
	{
		return logsWindow;
	}

	/**
	 * @brief Get viewport window
	 * 
	 * @param InViewportType	Viewport type
	 * @return Return reference to viewport window
	 */
	FORCEINLINE CViewportWindow& GetViewportWindow( ELevelViewportType InViewportType )
	{
		return viewportWindows[InViewportType];
	}

private:
	/**
	 * Add TOC entries
	 * @param InRootDir		Root directory
	 */
	void AddTOCEntries( const std::wstring& InRootDir );

	EEditorMode									currentEditorMode;					/**< Current editor mode */
	std::vector<TRefCountPtr<class CViewport>>	viewports;							/**< Array of viewports for render */
	CEditorConstraints							constraints;						/**< Editor constraints */
	class CEditorInterfaceViewportClient*		editorInterfaceViewportClient;		/**< Viewport client for render editor interface */

	// Windows
	CActorClassesWindow							actorClassesWindow;					/**< Actor classes window */
	CActorPropertiesWindow						actorPropertiesWindow;				/**< Actor properties window */
	CContentBrowserWindow						contentBrowserWindow;				/**< Content browser window */
	CExplorerLevelWindow						explorerLevelWindow;				/**< Explorer level window */
	CLogsWindow									logsWindow;							/**< Logs window */
	CViewportWindow								viewportWindows[LVT_Max];			/**< Viewport windows */
};

#endif // !EDITORENGINE_H