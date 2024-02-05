/**
 * @file
 * @addtogroup Engine Engine
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

#ifndef APOINTLIGHT_H
#define APOINTLIGHT_H

#include "Engine/Actors/Light.h"
#include "Engine/Components/SpriteComponent.h"
#include "Engine/Components/PointLightComponent.h"

 /**
  * @ingroup Engine
  * Actor of point light
  */
class APointLight : public ALight
{
	DECLARE_CLASS( APointLight, ALight, 0, 0 )

public:
	/**
	 * Constructor
	 */
	APointLight();

#if WITH_EDITOR
	/**
	 * @brief Get path to icon of actor for exploer level in WorldEd
	 * @return Return path to actor icon from Sys_BaseDir()
	 */
	virtual std::wstring GetActorIcon() const override;
#endif // WITH_EDITOR

	/**
	 * Get point light component
	 * @return Return point light component
	 */
	FORCEINLINE TRefCountPtr<CPointLightComponent> GetPointLightComponent() const
	{
		return pointLightComponent;
	}

private:
	TRefCountPtr<CPointLightComponent>			pointLightComponent;		/**< Point light component */

#if WITH_EDITOR
	TRefCountPtr<CSpriteComponent>				gizmoComponent;				/**< Gizmo component */
#endif // WITH_EDITOR
};

#endif // !APOINTLIGHT_H