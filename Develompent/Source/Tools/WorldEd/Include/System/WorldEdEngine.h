/**
 * @file
 * @addtogroup WorldEd WorldEd
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

#ifndef WORLDEDENGINE_H
#define WORLDEDENGINE_H

#include <vector>

#include "Core/Math/Color.h"
#include "Core/Logger/LoggerMacros.h"
#include "Core/Misc/SharedPointer.h"
#include "Engine/Render/Viewport.h"
#include "Editor/System/EditorEngine.h"

// Windows
#include "Windows/EditorWindow.h"
#include "Windows/ActorClassesWindow.h"
#include "Windows/ActorPropertiesWindow.h"
#include "Windows/ContentBrowserWindow.h"
#include "Windows/ExplorerLevelWindow.h"
#include "Windows/LogsWindow.h"
#include "Windows/LevelViewportWindow.h"

/**
 * @ingroup WorldEd
 * @brief WorldEd engine
 */
class CWorldEdEngine : public CEditorEngine
{
	DECLARE_CLASS( CWorldEdEngine, CEditorEngine, 0, 0 )

public:
	/**
	 * Constructor
	 */
	CWorldEdEngine();

	/**
	 * @brief Destructor
	 */
	virtual ~CWorldEdEngine();

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
	virtual void ProcessEvent( struct WindowEvent& InWindowEvent ) override;

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
	 * @brief Get selected asset
	 * @param Return reference of selected asset. If isn't returns empty string
	 */
	virtual std::wstring GetSelectedAsset() const;

	/**
	 * @brief Set selected asset
	 * @param InAssetReference	New asset reference to select
	 */
	virtual void SetSelectedAsset( const std::wstring& InAssetReference );

	/**
	 * Get editor name
	 * 
	 * @return Return editor name
	 */
	virtual std::wstring GetEditorName() const;

	/**
	 * @brief Get actor classes window
	 * @return Return reference to actor classes window
	 */
	FORCEINLINE const TSharedPtr<CActorClassesWindow>& GetActorClassesWindow() const
	{
		return actorClassesWindow;
	}

	/**
	 * @brief Get actor properties window
	 * @return Return reference to actor properties window
	 */
	FORCEINLINE const TSharedPtr<CActorPropertiesWindow>& GetActorPropertiesWindow() const
	{
		return actorPropertiesWindow;
	}

	/**
	 * @brief Get content browser window
	 * @return Return reference to content browser window
	 */
	FORCEINLINE const TSharedPtr<CContentBrowserWindow>& GetContentBrowserWindow() const
	{
		return contentBrowserWindow;
	}

	/**
	 * @brief Get explorer level window
	 * @return Return reference to explorer level window
	 */
	FORCEINLINE const TSharedPtr<CExplorerLevelWindow>& GetExplorerLevelWindow() const
	{
		return explorerLevelWindow;
	}

	/**
	 * @brief Get logs window
	 * @return Return reference to logs window
	 */
	FORCEINLINE const TSharedPtr<CLogsWindow>& GetLogsWindow() const
	{
		return logsWindow;
	}

	/**
	 * @brief Get level viewport window
	 * 
	 * @param InViewportType	Viewport type
	 * @return Return reference to viewport window
	 */
	FORCEINLINE const TSharedPtr<CLevelViewportWindow>& GetLevelViewportWindow( ELevelViewportType InViewportType ) const
	{
		return viewportWindows[InViewportType];
	}

	/**
	 * @brief Get editor window
	 * @return Return reference to editor window
	 */
	FORCEINLINE const TSharedPtr<CEditorWindow>& GetEditorWindow() const
	{
		return editorWindow;
	}

private:
	std::vector<TRefCountPtr<class CViewport>>	viewports;							/**< Array of viewports for render */
	CViewportClient*							editorInterfaceViewportClient;		/**< Viewport client for render editor interface */

	// Windows
	TSharedPtr<CEditorWindow>					editorWindow;						/**< Main editor window */
	TSharedPtr<CActorClassesWindow>				actorClassesWindow;					/**< Actor classes window */
	TSharedPtr<CActorPropertiesWindow>			actorPropertiesWindow;				/**< Actor properties window */
	TSharedPtr<CContentBrowserWindow>			contentBrowserWindow;				/**< Content browser window */
	TSharedPtr<CExplorerLevelWindow>			explorerLevelWindow;				/**< Explorer level window */
	TSharedPtr<CLogsWindow>						logsWindow;							/**< Logs window */
	TSharedPtr<CLevelViewportWindow>			viewportWindows[LVT_Max];			/**< Level viewport windows */
};

#endif // !WORLDEDENGINE_H