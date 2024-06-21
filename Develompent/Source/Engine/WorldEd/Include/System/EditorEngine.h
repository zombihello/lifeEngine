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
#include "Windows/EditorWindow.h"
#include "Windows/ActorClassesWindow.h"
#include "Windows/ActorPropertiesWindow.h"
#include "Windows/ContentBrowserWindow.h"
#include "Windows/ExplorerLevelWindow.h"
#include "Windows/LogsWindow.h"
#include "Windows/LevelViewportWindow.h"

/**
 * @ingroup WorldEd
 * Enumeration icon types
 */
enum EIconType
{
	IT_Color_R,										/**< Color R channel */
	IT_Color_G,										/**< Color G channel */
	IT_Color_B,										/**< Color B channel */
	IT_Color_A,										/**< Color A channel */
	IT_Import,										/**< Import */
	IT_Insert,										/**< Insert */
	IT_Browse,										/**< Browse */
	IT_Remove,										/**< Remove */
	IT_Add,											/**< Add */
	IT_ToolSelect,									/**< Tool select */
	IT_ToolTranslate,								/**< Tool translate */
	IT_ToolRotate,									/**< Tool rotate */
	IT_ToolScale,									/**< Tool scale */
	IT_PlayStandaloneGame,							/**< Play standalone game */
	IT_Thumbnail_Asset_Unknown,						/**< Unknown asset */
	IT_Thumbnail_Asset_Texture2D,					/**< Texture2D asset */
	IT_Thumbnail_Asset_Material,					/**< Material asset */
	IT_Thumbnail_Asset_StaticMesh,					/**< StaticMesh asset */
	IT_Thumbnail_Asset_AudioBank,					/**< AudioBank asset */
	IT_Thumbnail_Asset_PhysicsMaterial,				/**< PhysicsMaterial asset */
	IT_Num											/**< Number of icon types */
};

/**
 * @ingroup WorldEd
 * Class of editor engine
 */
class CEditorEngine : public CBaseEngine
{
	DECLARE_CLASS_INTRINSIC( CEditorEngine, CBaseEngine, 0, 0, TEXT( "WorldEd" ) )

public:
	/**
	 * Constructor
	 */
	CEditorEngine();

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
	 * Create new map
	 */
	virtual void NewMap();

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
		return L_Sprintf( TEXT( "%s" ) PATH_SEPARATOR TEXT( "Content" ) PATH_SEPARATOR, Sys_GameDir().c_str() );
	}

	/**
	 * Get engine content directory
	 * @return Return engine content directory
	 */
	FORCEINLINE std::wstring GetEngineContentDir() const
	{
		return L_Sprintf( TEXT( "%s" ) PATH_SEPARATOR TEXT( "Engine" ) PATH_SEPARATOR TEXT( "Content" ) PATH_SEPARATOR, Sys_BaseDir().c_str() );
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
		EditorDelegates::onEditorModeChanged.Broadcast( InNewEditorMode );
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

	/**
	 * @brief Get icon
	 * 
	 * @param InIcon	Icon
	 * @return Return asset handle of icon
	 */
	FORCEINLINE TAssetHandle<CTexture2D> GetIcon( EIconType InIcon ) const
	{
		Assert( InIcon != IT_Num );
		return icons[InIcon];
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
	CViewportClient*							editorInterfaceViewportClient;		/**< Viewport client for render editor interface */
	TAssetHandle<CTexture2D>					icons[IT_Num];						/**< Icons */

	// Windows
	TSharedPtr<CEditorWindow>					editorWindow;						/**< Main editor window */
	TSharedPtr<CActorClassesWindow>				actorClassesWindow;					/**< Actor classes window */
	TSharedPtr<CActorPropertiesWindow>			actorPropertiesWindow;				/**< Actor properties window */
	TSharedPtr<CContentBrowserWindow>			contentBrowserWindow;				/**< Content browser window */
	TSharedPtr<CExplorerLevelWindow>			explorerLevelWindow;				/**< Explorer level window */
	TSharedPtr<CLogsWindow>						logsWindow;							/**< Logs window */
	TSharedPtr<CLevelViewportWindow>			viewportWindows[LVT_Max];			/**< Level viewport windows */
};

#endif // !EDITORENGINE_H