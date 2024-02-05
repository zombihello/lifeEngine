/**
 * @file
 * @addtogroup WorldEd World editor
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

#ifndef LEVELVIEWPORTWINDOW_H
#define LEVELVIEWPORTWINDOW_H

#include "Core/Misc/RefCountPtr.h"
#include "Engine/Render/Viewport.h"
#include "Engine/Render/RenderTarget.h"
#include "UI/ImGUI/ImGUIEngine.h"
#include "UI/ImGUI/ImGuizmo.h"
#include "Render/EditorLevelViewportClient.h"
#include "Widgets/ViewportWidget.h"

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