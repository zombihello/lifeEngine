/**
 * @file
 * @addtogroup WorldEd World editor
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef LEVELVIEWPORTWINDOW_H
#define LEVELVIEWPORTWINDOW_H

#include "Misc/RefCountPtr.h"
#include "ImGUI/ImGUIEngine.h"
#include "Render/Viewport.h"
#include "Render/RenderTarget.h"
#include "Render/EditorLevelViewportClient.h"
#include "Widgets/ViewportWidget.h"
#include "ImGUI/ImGuizmo.h"

/**
 * @ingroup WorldEd
 * @brief Level viewport window
 */
class CLevelViewportWindow : public CImGUILayer
{
public:
	/**
	 * @brief Constructor
	 *
	 * @param InName			Window name
	 * @param InVisibility		Default visibility of the viewport window. If is FALSE him will not draw
	 * @param InViewportType	Viewport type
	 */
	CLevelViewportWindow( const std::wstring& InName, bool InVisibility = true, ELevelViewportType InViewportType = LVT_Perspective );

	/**
	 * @brief Init
	 */
	virtual void Init() override;

	/**
	 * @brief Process event
	 *
	 * @param InWindowEvent			Window event
	 */
	virtual void ProcessEvent( struct WindowEvent& InWindowEvent ) override;

protected:
	/**
	 * @brief Method tick interface of a layer
	 */
	virtual void OnTick() override;

	/**
	 * @brief Method called when in the layer is changed visibility
	 * @param InNewVisibility		New visibility
	 */
	virtual void OnVisibilityChanged( bool InNewVisibility ) override;

private:
	/**
	 * @brief Draw popup menu
	 */
	void DrawPopupMenu();

	bool							bGuizmoUsing;			/**< Is using ImGuizmo now */
	ImGuizmo::OPERATION				guizmoOperationType;	/**< ImGuizmo operation type */
	ImGuizmo::MODE					guizmoModeType;			/**< ImGuizmo mode type */
	ImVec2							viewportCursorPos;		/**< Viewport cursor position */
	CEditorLevelViewportClient		viewportClient;			/**< Viewport client */
	CViewportWidget					viewportWidget;			/**< Viewport widget */
};

#endif // !LEVELVIEWPORTWINDOW_H