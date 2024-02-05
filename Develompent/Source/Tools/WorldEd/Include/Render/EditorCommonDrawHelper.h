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

#ifndef EDITORCOMMONDRAWHELPER_H
#define EDITORCOMMONDRAWHELPER_H

#include "Core/Math/Color.h"
#include "Engine/Render/Viewport.h"

/**
 * @ingroup WorldEd
 * @brief Implements some basic functionality used by all editor viewports
 */
class CEditorCommonDrawHelper
{
public:
	/**
	 * @brief Constructor
	 */
	CEditorCommonDrawHelper();

	/**
	 * @brief Draws the viewport grid
	 * 
	 * @param InSceneView			Scene view
	 * @param InViewportType		Viewport type
	 * @param InScene				Scene
	 */
	void DrawGrid( const class CSceneView* InSceneView, ELevelViewportType InViewportType, class CScene* InScene );

private:
	/**
	 * @brief Draw grid section
	 * 
	 * @param InViewportLocX		Viewport location by X
	 * @param InViewportGridY		Viewport grid by Y
	 * @param InStart				Coords of start grid
	 * @param InEnd					Coords of end grid
	 * @param InStartX				Reference to changing component in InStart
	 * @param InEndX				References to changing component in InEnd
	 * @param InAxis				Axis ID (X = 0, Y = 1, Z = 2)
	 * @param InIsAlphaCase			Is alpha case?
	 * @param InSceneView			Scene view
	 * @param InSDG					Scene depth group
	 */
	void DrawGridSection( int32 InViewportLocX, int32 InViewportGridY, Vector& InStart, Vector& InEnd, float& InStartX, float& InEndX, int32 InAxis, bool InIsAlphaCase, const class CSceneView* InSceneView, struct SceneDepthGroup& InSDG );

	bool		bDrawColoredOrigin;			/**< Is need draw colored origins */
	bool		bDrawWorldBox;				/**< Is need draw world box */
	float		perspectiveGridSize;		/**< Perspective grid size */
	CColor		gridColorHi;				/**< Grid color height */
	CColor		gridColorLo;				/**< Grid color low */
	CColor		worldBoxColor;				/**< World box color */
};

#endif // !EDITORCOMMONDRAWHELPER_H