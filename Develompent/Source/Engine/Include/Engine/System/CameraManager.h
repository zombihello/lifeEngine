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

#ifndef CAMERAMANAGER_H
#define CAMERAMANAGER_H

#include "Core/Misc/RefCountPtr.h"
#include "Engine/Components/CameraComponent.h"

/**
 * @ingroup Engine
 * @brief Manager cameras in engine
 */
class CCameraManager
{
public:
    /**
	 * Begin play of game in world
	 */
    virtual void BeginPlay();

    /**
     * End play of game in world
     */
    virtual void EndPlay();

    /**
     * @brief Process event
     *
     * @param InWindowEvent Window event
     */
    void ProcessEvent( struct WindowEvent& InWindowEvent );

    /**
     * @brief Set active camera for render frame
     * @param InCameraComponent Camera component
     */
    FORCEINLINE void SetActiveCamera( CCameraComponent* InCameraComponent )
    {
        if ( activeCamera )
        {
            activeCamera->bIsActive = false;
        }

        activeCamera = InCameraComponent;

        if ( activeCamera )
        {
            activeCamera->bIsActive = true;
        }
    }

    /**
     * @brief Get active camera
     * @return Return active camera. If not seted returing nullptr
     */
    FORCEINLINE TRefCountPtr< CCameraComponent > GetActiveCamera() const
    {
        return activeCamera;
    }

private:
    TRefCountPtr< CCameraComponent >        activeCamera;       /**< Current active camera */
};

#endif // !CAMERAMANAGER_H
