/**
 * @file
 * @addtogroup WorldEd World editor
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef VIEWPORTS_H
#define VIEWPORTS_H

#include <wx/wx.h>
#include <string>
#include <vector>

#include "Render/Viewport.h"
#include "Render/EditorLevelViewportClient.h"
#include "Render/Scene.h"
#include "Core.h"

/**
 * @ingroup WorldEd
 * @brief Enumeration of viewport config
 */
enum EViewportConfig
{
	VC_None			= -1,		/**< None */
	VC_2_2_Split,				/**< 2x2 split */
	VC_1_2_Split,				/**< 1x2 split */
	VC_1_1_SplitH,				/**< 1x1 horizontal split */
	VC_1_1_SplitV,				/**< 1x1 vertical split */
	VC_Max						/**< Max configs */
};

/**
 * @ingroup WorldEd
 * @brief The default values for a viewport within an FViewportConfig_Template
 */
struct FViewportConfig_Viewport
{
	/**
	 * @brief Constructor
	 */
	FViewportConfig_Viewport();

	/**
	 * @brief Destructor
	 */
	virtual ~FViewportConfig_Viewport();

	bool					bEnabled;				/**< Is enabled viewport */
	bool					bSetListenerPosition;	/**< Is need sets the listener position */
	EShowFlags				showFlags;				/**< Show flags */
	ELevelViewportType		viewportType;			/**< Viewport type */
};

/**
 * @ingroup WorldEd
 * @brief A template for a viewport configuration. Holds the baseline layouts and flags for a config
 */
struct FViewportConfig_Template
{
	/**
	 * @brief Constructor
	 */
	FViewportConfig_Template();

	/**
	 * @brief Destructor
	 */
	virtual ~FViewportConfig_Template();

	/**
	 * @brief Set template
	 * @param InViewportConfig		Type viewport config
	 */
	void Set( EViewportConfig InViewportConfig );

	std::wstring				description;				/**< The description for this configuration */
	EViewportConfig				viewportConfig;				/**< The viewport config this template represents */
	FViewportConfig_Viewport	viewportTemplates[ 4 ];		/**< The viewport templates within this config template */
};

/**
 * @ingroup WorldEd
 * @brief An instance of a FViewportConfig_Template. There is only one of these
 * in use at any given time and it represents the current editor viewport
 * layout.  This contains more information than the template (i.e. splitter
 * and camera locations).
 */
struct FVCD_Viewport : public FViewportConfig_Viewport
{
	/**
	 * @brief Constructor
	 */
	FVCD_Viewport();

	/**
	 * @brief Destructor
	 */
	virtual ~FVCD_Viewport();

	/**
	 * @brief Override operator =
	 */
	FORCEINLINE FVCD_Viewport operator=( const FViewportConfig_Viewport& InOther )
	{
		viewportType			= InOther.viewportType;
		bEnabled				= InOther.bEnabled;
		bSetListenerPosition	= InOther.bSetListenerPosition;
		showFlags				= InOther.showFlags;
		return *this;
	}

	class WxLevelViewportWindow*		viewportWindow;		/**< The window that holds the viewport itself */
};

/**
 * @ingroup WorldEd
 * @brief Contains a level editing viewport.
 */
class WxLevelViewportWindow : public wxWindow, public FEditorLevelViewportClient
{
public:
	/**
	 * @brief Constructor
	 */
	WxLevelViewportWindow();

	/**
	 * @brief Destructor
	 */
	~WxLevelViewportWindow();

	/**
	 * @brief Init UI
	 * 
	 * @param InViewportType			Viewport type
	 * @param InSetListenerPosition		Is need set listener position
	 * @param InShowFlags				Show flags for viewport
	 */
	void SetUp( ELevelViewportType InViewportType, bool InSetListenerPosition, EShowFlags InShowFlags );

private:
	//
	// Event handlers
	//
	void OnSize( wxSizeEvent& InEvent );

	class WxLevelViewportToolBar*		toolBar;		/**< Level viewport tool bar */
	class FViewport*					viewport;		/**< Viewport of render */

	DECLARE_EVENT_TABLE()
};

/**
 * @ingroup WorldEd
 * @brief Viewport config data
 */
class FViewportConfig_Data
{
public:
	/**
	 * @brief Constructor 
	 */
	FViewportConfig_Data();

	/**
	 * @brief Destructor
	 */
	~FViewportConfig_Data();

	/**
	 * @brief Set viewport config template
	 * @param InTemplate		Viewport config template
	 */
	void SetTemplate( EViewportConfig InTemplate );

	/**
	 * @brief Apply viewport config
	 * @param InParent		Parent window
	 */
	void Apply( class WxViewportsContainer* InParent );

	/**
	 * @brief Load settings from config
	 * @return Return TRUE if seccussed loaded data from config
	 */
	bool LoadFromConfig();

	/**
	 * @brief Save settings to config
	 */
	void SaveToConfig();

	/**
	 * @brief Resize proportionally
	 * 
	 * @param InScaleX			Scale by X
	 * @param InScaleY			Scale by Y
	 * @param InIsNeedRedraw	Is need redraw
	 */
	void ResizeProportionally( float InScaleX, float InScaleY, bool InIsNeedRedraw = true );

	/**
	 * @brief Override operator =
	 */
	FORCEINLINE FViewportConfig_Data operator=( const FViewportConfig_Template& InOther )
	{
		for ( uint32 index = 0; index < 4; ++index )
		{
			viewports[ index ] = InOther.viewportTemplates[ index ];
		}
		return *this;
	}

	/**
	 * @brief Get template viewport config
	 * @return Return template viewport config
	 */
	FORCEINLINE EViewportConfig GetTemplate() const
	{
		return templateViewport;
	}

private:
	EViewportConfig								templateViewport;		/**< The template this instance is based on */	
	FVCD_Viewport								viewports[ 4 ];			/**< Four standard viewports */		
	std::vector< class wxSplitterWindow* >		splitterWindows;		/**< The splitters windows that make this config possible. */
	wxBoxSizer*									sizer;					/**< The top level sizer for the viewports */
};

#endif // !VIEWPORTS_H