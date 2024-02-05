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

#ifndef PLAYERSTART_H
#define PLAYERSTART_H

#include "Engine/Actors/Actor.h"
#include "Engine/Components/SpriteComponent.h"
#include "Engine/Components/ArrowComponent.h"

/**
 * @ingroup Engine
 * Actor for spawn player controller in world
 */
class APlayerStart : public AActor
{
	DECLARE_CLASS( APlayerStart, AActor, 0, 0 )

public:
	/**
	 * Constructor
	 */
	APlayerStart();

	/**
	 * Destructor
	 */
	virtual ~APlayerStart();

	/**
	 * Overridable native event for when play begins for this actor
	 */
	virtual void BeginPlay() override;

#if WITH_EDITOR
	/**
	 * @brief Get path to icon of actor for exploer level in WorldEd
	 * @return Return path to actor icon from Sys_BaseDir()
	 */
	virtual std::wstring GetActorIcon() const override;

private:
	TRefCountPtr<CSpriteComponent>		gizmoComponent;				/**< Gizmo component */
	TRefCountPtr<CArrowComponent>		arrowComponent;				/**< Arrow component */
#endif // WITH_EDITOR
};

#endif // !PLAYERSTART_H