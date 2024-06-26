/**
 * @file
 * @addtogroup UI User interface
 *
 * Copyright BSOD-Games, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef IMGUIENGINE_H
#define IMGUIENGINE_H

#include "LEBuild.h"
#if WITH_IMGUI

#include <stack>
#include <vector>

#include "Misc/StringConv.h"
#include "Core.h"
#include "Misc/RefCounted.h"
#include "Misc/RefCountPtr.h"
#include "Misc/SharedPointer.h"
#include "System/Threading.h"
#include "System/WindowEvent.h"
#include "ImGUI/imgui.h"
#include "RHI/TypesRHI.h"
#include "ImGUI/ImGUIIcons.h"
#include "ImGUI/ImGUIExtension.h"

/**
 * @ingroup UI
 * Count draw data buffers for render ImGUI
 */
#define IMGUI_DRAWBUFFERS_COUNT		3

/**
 * @ingroup UI
 * @brief Initialize ImGUI on platform
 */
bool Sys_ImGUIInit();

/**
 * @ingroup UI
 * @brief Shutdown ImGUI on platform
 */
void Sys_ImGUIShutdown();

/**
 * @ingroup UI
 * @brief Begin drawing ImGUI
 */
void Sys_ImGUIBeginDrawing();

/**
 * @ingroup UI
 * @brief End drawing ImGUI
 */
void Sys_ImGUIEndDrawing();

/**
 * @ingroup UI
 * @brief Process event for ImGUI
 * 
 * @param[in] InWindowEvent Window event
 */
void Sys_ImGUIProcessEvent( struct WindowEvent& InWindowEvent );

/**
 * @ingroup UI
 * @brief Enumeration of ImGUI style colors
 */
enum EImGuiColors
{
	IGC_Selection,					/**< Color of selection */
	IGC_Asset_Unknown,				/**< Unknown asset */
	IGC_Asset_Texture2D,			/**< Texture2D asset */
	IGC_Asset_Material,				/**< Material asset */
	IGC_Asset_Unused0,				/**< Unused 0 asset */
	IGC_Asset_StaticMesh,			/**< StaticMesh asset */
	IGC_Asset_AudioBank,			/**< AudioBank asset */
	IGC_Asset_PhysicsMaterial,		/**< PhysicsMaterial asset */
	IGC_TableBgColor0,				/**< Table background color 0 */
	IGC_TableBgColor1,				/**< Table background color 1 */
	IGC_WarningColor,				/**< Warning color */
	IGC_ErrorColor,					/**< Error color */
	IGC_AxisXColor,					/**< Color of X axis */
	IGC_AxisYColor,					/**< Color of Y axis */
	IGC_AxisZColor,					/**< Color of Z axis */
	IGC_AxisXColor_Hovered,			/**< Color of X axis when hovered */
	IGC_AxisYColor_Hovered,			/**< Color of Y axis when hovered */
	IGC_AxisZColor_Hovered,			/**< Color of Z axis when hovered */
	IGC_AxisXColor_Active,			/**< Color of X axis when is active */
	IGC_AxisYColor_Active,			/**< Color of Y axis when is active */
	IGC_AxisZColor_Active,			/**< Color of Z axis when is active */
	IGC_Num							/**< Number of ImGUI colors */
};

/**
 * @ingroup UI
 * @brief Class for draw data of ImGUI
 */
class CImGUIDrawData : public CRefCounted
{
public:
	/**
	 * Constructor
	 */
	CImGUIDrawData();

	/**
	 * Destructor
	 */
	~CImGUIDrawData();

	/**
	 * Mark buffer is free
	 */
	FORCEINLINE void MarkFree()
	{
		isFree = true;
	}

	/**
	 * Clear buffer
	 */
	void Clear();

	/**
	 * Set draw data
	 * @param[in] InDrawData Pointer to draw data of ImGUI
	 */
	void SetDrawData( ImDrawData* InDrawData );

	 /**
	  * Get ImGUI draw data. Const version
	  *
	  * @return Return pointer to ImGUI draw data
	  */
	FORCEINLINE const ImDrawData* GetDrawData() const
	{
		return &drawData;
	}

	/**
	 * Get ImGUI draw data
	 *
	 * @return Return pointer to ImGUI draw data
	 */
	FORCEINLINE ImDrawData* GetDrawData()
	{
		return &drawData;
	}

	/**
	 * Is free buffer
	 * @return Return true if buffer is free, else return false
	 */
	FORCEINLINE bool IsFree() const
	{
		return isFree;
	}

private:
	bool					isFree;				/**< Is free buffer */
	ImDrawData				drawData;			/**< ImGUI draw data */
};

/**
 * @ingroup UI
 * Class of update window ImGUI
 */
class CImGUIWindow
{
public:
	/**
	 * Constructor
	 * @param[in] InViewport Pointer to viewport of ImGUI
	 */
	CImGUIWindow( ImGuiViewport* InViewport );

	/**
	 * Update ImGUI windows
	 */
	void Tick();

	/**
	 * Get ImGUI viewport
	 * @return Return Pointer to ImGUI viewport
	 */
	FORCEINLINE ImGuiViewport* GetViewport() const
	{
		return imguiViewport;
	}

private:
	ImGuiViewport*						imguiViewport;									/**< Pointer to ImGUI viewport */
	TRefCountPtr< CImGUIDrawData >		drawDataBuffers[ IMGUI_DRAWBUFFERS_COUNT ];		/**< Buffers of ImDrawData for draw. In one buffer write, from one read */
	uint32								indexCurrentBuffer;								/**< Index of current buffer */
};

/**
 * @ingroup UI
 * @brief ImGUI Popup menu
 */
class CImGUIPopup : public TSharedFromThis<CImGUIPopup>
{
public:
	/**
	 * @brief Constructor
	 * 
	 * @param InName	Popup name
	 */
	CImGUIPopup( const std::wstring& InName = TEXT( "NewPopup" ) );

	/**
	 * @brief Tick ImGUI popup
	 * Need call in main thread
	 */
	void Tick();

	/**
	 * @brief Open popup
	 */
	FORCEINLINE void Open()
	{
		if ( !bOpen )
		{
			bNeedOpen = true;
		}
	}

	/**
	 * @brief Close popup
	 */
	FORCEINLINE void Close()
	{
		if ( bOpen )
		{
			bNeedClose = true;
		}
	}

	/**
	 * @brief Get layer name
	 * @return Return layer name
	 */
	FORCEINLINE std::wstring GetName() const
	{
		return name;
	}

	/**
	 * @brief Is opened popup
	 * @return Return TRUE if popup is opened
	 */
	FORCEINLINE bool IsOpen() const
	{
		return bOpen;
	}

protected:
	/**
	 * @brief Method tick interface of a popup
	 */
	virtual void OnTick() = 0;

	/**
	 * @brief Set popup size
	 * @param InSize	Popup size
	 */
	FORCEINLINE void SetSize( const Vector2D& InSize )
	{
		pendingSize = InSize;
		bPendingChangeSize = true;
	}

private:
	bool				bOpen;				/**< Is open popup */
	bool				bNeedClose;			/**< Is need close popup */
	bool				bNeedOpen;			/**< Is need open popup */
	bool				bPendingChangeSize;	/**< Is need update size */
	Vector2D			pendingSize;		/**< Padding size */
	std::wstring		name;				/**< Popup name */
};

/**
 * @ingroup UI
 * @brief ImGUI layer
 */
class CImGUILayer : public TSharedFromThis<CImGUILayer>
{
public:
	/**
	 * @brief Constructor
	 * 
	 * @param InName	Layer name
	 */
	CImGUILayer( const std::wstring& InName = TEXT( "NewLayer" ) );

	/**
	 * @brief Destructor
	 */
	virtual ~CImGUILayer();

	/**
	 * @brief Init
	 */
	virtual void Init();

	/**
	 * @brief Tick ImGUI layer
	 * Need call in main thread
	 */
	void Tick();

	/**
	 * @brief Handle layer event
	 *
	 * @param	OutLayerEvent Occurred layer event
	 * @return Return TRUE if queue of events not empty, else will return FALSE
	 */
	bool PollEvent( WindowEvent& OutLayerEvent );

	/**
	 * @brief Process event
	 *
	 * @param InWindowEvent			Window event
	 */
	virtual void ProcessEvent( struct WindowEvent& InWindowEvent );

	/**
	 * @brief Open popup
	 * @warning Supported opened only one popup in whole time
	 *
	 * @param InArgs	Arguments for construct popup
	 * @return Return pointer to opened popup
	 */
	template< typename ObjectType, typename... ArgTypes >
	FORCEINLINE TSharedPtr<CImGUIPopup> OpenPopup( ArgTypes&&... InArgs )
	{
		popup = MakeSharedPtr<ObjectType>( InArgs... );
		popup->Open();
		return popup;
	}

	/**
	 * @brief Close current popup
	 */
	FORCEINLINE void CloseCurrentPopup()
	{
		if ( popup )
		{
			popup->Close();
		}
	}

	/**
	 * @brief Get current popup
	 * @return Return current opened popup, in case when him is not opened will return NULL
	 */
	FORCEINLINE TSharedPtr<CImGUIPopup> GetCurrentPopup() const
	{
		return popup;
	}

	/**
	 * @brief Get layer name
	 * @return Return layer name
	 */
	FORCEINLINE std::wstring GetName() const
	{
		return name;
	}

	/**
	 * @brief Set visibility of the layer
	 * @param InVisibility	Is visible the layer
	 */
	FORCEINLINE void SetVisibility( bool InVisibility )
	{
		bVisibility = InVisibility;
		OnVisibilityChanged( InVisibility );
	}

	/**
	 * @brief Set layer padding
	 * @param InPadding		New layer padding
	 */
	FORCEINLINE void SetPadding( const Vector2D& InPadding )
	{
		padding = InPadding;
	}

	/**
	 * @brief Is layer inited
	 * @return Return TRUE if layer is inited
	 */
	FORCEINLINE bool IsInit() const
	{
		return bInit;
	}

	/**
	 * @brief Is visible layer
	 * @return Return TRUE if layer is visible
	 */
	FORCEINLINE bool IsVisibility() const
	{
		return bVisibility;
	}

	/**
	 * @brief Is focused layer
	 * @return Return TRUE if layer is focused
	 */
	FORCEINLINE bool IsFocused() const
	{
		return bFocused;
	}

	/**
	 * @brief Is hovered layer
	 * @return Return TRUE if layer is hovered
	 */
	FORCEINLINE bool IsHovered() const
	{
		return bHovered;
	}

	/**
	 * @brief Get layer size by X
	 * @return Return layer size by X
	 */
	FORCEINLINE float GetSizeX() const
	{
		return size.x;
	}

	/**
	 * @brief Get layer size by Y
	 * @return Return layer size by Y
	 */
	FORCEINLINE float GetSizeY() const
	{
		return size.y;
	}

	/**
	 * @brief Get layer padding
	 * @return Return layer padding
	 */
	FORCEINLINE Vector2D GetPadding() const
	{
		return padding;
	}

protected:
	/**
	 * @brief Enumeration of ImGUI layer flags
	 */
	enum ELayerFlags
	{
		LF_DestroyOnHide		= 1 << 30	/**< Is need destroy layer after him is hided */
	};

	/**
	 * @brief Destroy ImGUILayer and remove him from tick list
	 */
	void Destroy();

	/**
	 * @brief Method tick interface of a layer
	 */
	virtual void OnTick() = 0;

	/**
	 * @brief Method called when in the layer is changed visibility
	 * @param InNewVisibility		New visibility
	 */
	virtual void OnVisibilityChanged( bool InNewVisibility );

	/**
	 * @brief Set layer size
	 * @param InSize	Layer size
	 */
	FORCEINLINE void SetSize( const Vector2D& InSize )
	{
		size = InSize;
		bPendingChangeSize = true;
	}

	/**
	 * @brief Set layer position
	 * @param InPosition	Layer position
	 */
	FORCEINLINE void SetPosition( const Vector2D& InPosition )
	{
		position = InPosition;
		bPendingChangePos = true;
	}

	/**
	 * @brief Set viewport ID
	 * @param InViewportID		Viewport ID
	 */
	FORCEINLINE void SetViewportID( ImGuiID InViewportID )
	{
		viewportID = InViewportID;
		bPendingChangeViewportID = true;
	}

	/**
	 * @brief Set rounding
	 * @param InRounding	Layer rounding
	 */
	FORCEINLINE void SetRounding( float InRounding )
	{
		rounding = InRounding;
	}

	/**
	 * @brief Set border size
	 * @param InBorderSize		Layer border size
	 */
	FORCEINLINE void SetBorderSize( float InBorderSize )
	{
		borderSize = InBorderSize;
	}

	uint32						flags;				/**< ImGUI window flags */

private:
	/**
	 * @brief Update ImGUI events
	 */
	void UpdateEvents();

	bool						bInit;						/**< Is inited layer */
	bool						bVisibility;				/**< Is visible layer */
	bool						bFocused;					/**< Is focused layer */
	bool						bHovered;					/**< Is hovered layer */
	bool						bPendingChangeSize;			/**< Is need update size of layer */
	bool						bPendingChangePos;			/**< Is need update position of layer */
	bool						bPendingChangeViewportID;	/**< Is need update change viewport ID of layer */
	float						rounding;					/**< Layer rounding */
	float						borderSize;					/**< Layer border size */
	Vector2D					size;						/**< Layer size */
	Vector2D					position;					/**< Layer position */
	ImGuiID						viewportID;					/**< Viewport ID */
	Vector2D					padding;					/**< Layer padding */
	std::wstring				name;						/**< Layer name */
	std::stack<WindowEvent>	events;						/**< Stack of ImGUI events who need process */
	TSharedPtr<CImGUIPopup>		popup;						/**< Current opened popup in layer */
};

/**
 * @ingroup UI
 * @brief Class for work with ImGUI and initialize her on platforms
 */
class CImGUIEngine
{
public:
	/**
	 * @brief Constructor
	 */
	CImGUIEngine();

	/**
	 * @brief Destructor
	 */
	~CImGUIEngine();

	/**
	 * @brief Initialize ImGUI
	 */
	void Init();

	/**
	 * @brief Shutdown ImGUI on platform
	 */
	void Shutdown();

	/**
	 * @brief Update logic
	 *
	 * @param InDeltaTime	Delta time
	 */
	void Tick( float InDeltaSeconds );

	/**
	 * @brief Add ImGUI layer to tick
	 * @param InImGUILayer	ImGUI layer
	 */
	FORCEINLINE void AddLayer( const TSharedPtr<CImGUILayer>& InImGUILayer )
	{
		Assert( InImGUILayer );
		layers.push_back( InImGUILayer );
	}

	/**
	 * @brief Remove ImGUI layer from tick
	 * @param InImGUILayer	ImGUI layer
	 */
	FORCEINLINE void RemoveLayer( const TSharedPtr<CImGUILayer>& InImGUILayer )
	{
		Assert( InImGUILayer );
		for ( uint32 index = 0, count = layers.size(); index < count; ++index )
		{
			if ( layers[index] == InImGUILayer )
			{
				layers.erase( layers.begin() + index );
				return;
			}
		}
	}

	/**
	 * @brief Process event for ImGUI
	 * 
	 * @param[in] InWindowEvent Window event
	 */
	void ProcessEvent( struct WindowEvent& InWindowEvent );

	/**
	 * @brief Set show cursor
	 * @param InShow	Is need show cursor
	 */
	FORCEINLINE void SetShowCursor( bool InShow = true )
	{
		bShowCursor = InShow;
	}

	/**
	 * @brief Begin draw commands for render ImGUI
	 */
	void BeginDraw();

	/**
	 * @brief End draw commands for render ImGUI
	 */
	void EndDraw();

	/**
	 * @brief Open new ImGUI window
	 * @param[in] InViewport Pointer to viewport of ImGUI
	 */
	void OpenWindow( ImGuiViewport* InViewport );

	/**
	 * @brief Close ImGUI window
	 * @param[in] InViewport Pointer to viewport of ImGUI
	 */
	void CloseWindow( ImGuiViewport* InViewport );

	/**
	 * @brief Is show cursor
	 * @return Return TRUE if cursor is showed, otherwise will return FALSE
	 */
	FORCEINLINE bool IsShowCursor() const
	{
		return bShowCursor;
	}

	/**
	 * @brief Lock texture
	 * @note Locked texture will be unlocked in EndDraw(). This method you need use in ImGui::Image*(), otherwise will be game crashed
	 * 
	 * @param InTexture2D	Texture 2D
	 * @return Return locked texture 2D
	 */
	FORCEINLINE ImGUILockedTexture2D LockTexture( Texture2DRHIParamRef_t InTexture2D )
	{
		if ( InTexture2D )
		{
			InTexture2D->AddRef();
			lockedTextures.push_back( InTexture2D );
		}

		return ImGUILockedTexture2D{ InTexture2D };
	}

	/**
	 * @brief Get style color
	 * 
	 * @param InColorType	Color type (see ImGuiColors)
	 * @return Return style color for InColorType
	 */
	FORCEINLINE const ImVec4& GetStyleColor( EImGuiColors InColorType ) const
	{
		Assert( InColorType != IGC_Num );
		return styleColors[( uint32 )InColorType];
	}

private:
	/**
	 * @brief Init theme
	 */
	void InitTheme();

	bool										bShowCursor;			/**< Is need show cursor */
	struct ImGuiContext*						imguiContext;			/**< Pointer to ImGUI context */
	ImVec4										styleColors[IGC_Num];	/**< ImGui style colors */
	std::vector<CImGUIWindow*>					windows;				/**< Array of windows ImGUI */
	std::vector<TSharedPtr<CImGUILayer>>		layers;					/**< Array of ImGUI layers */
	std::list<Texture2DRHIParamRef_t>			lockedTextures;			/**< List of locked textures */
};

#endif // WITH_IMGUI
#endif // !IMGUIENGINE_H
