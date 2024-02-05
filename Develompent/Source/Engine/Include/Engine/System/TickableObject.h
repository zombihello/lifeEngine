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

#ifndef TICKABLEOBJECT_H
#define TICKABLEOBJECT_H

#include <vector>

#include "Engine/Render/RenderingThread.h"
#include "Core/Core.h"

/**
 * @ingroup Engine
 * @brief This class provides common registration for tickable objects
 */
class CTickableObject
{
public:
	/**
	 * @brief Registers this instance with the static array of tickable objects
	 *
	 * @param InIsRenderingThreadObject	TRUE if this object is owned by the rendering thread
	 * @param InRegisterImmediately		TRUE if the object should be registered immediately
	 */
	FORCEINLINE CTickableObject( bool InIsRenderingThreadObject = false, bool InRegisterImmediately = true )
		: bRenderingThreadObject( InIsRenderingThreadObject )
	{
		// Register the object unless deferral is requested.
		if ( InRegisterImmediately )
		{
			Register( InIsRenderingThreadObject );
		}
	}

	/**
	 * Removes this instance from the static array of tickable objects
	 */
	virtual ~CTickableObject()
	{
		// Remove object from static array
		if ( bRenderingThreadObject )
		{
			// Make sure that only the rendering thread is attempting remove items from the 'renderingThreadTickableObjects' list
			AssertMsg( IsInRenderingThread(), TEXT( "Game thread attempted remove an object from the RenderingThreadTickableObjects array" ) );
			for ( uint32 index = 0, count = renderingThreadTickableObjects.size(); index < count; ++index )
			{
				if ( renderingThreadTickableObjects[ index ] == this )
				{
					renderingThreadTickableObjects.erase( renderingThreadTickableObjects.begin() + index );
					return;
				}
			}
		}
		else
		{
			// Make sure that only the game thread is attempting remove items from the 'tickableObjects' list
			AssertMsg( IsInGameThread(), TEXT( "Rendering thread attempted remove an object from the TickableObjects array" ) );
			for ( uint32 index = 0, count = tickableObjects.size(); index < count; ++index )
			{
				if ( tickableObjects[ index ] == this )
				{
					tickableObjects.erase( tickableObjects.begin() + index );
					return;
				}
			}
		}
	}

	/**
	 * @brief Registers the object for ticking
	 * @param InIsRenderingThreadObject			TRUE if this object is owned by the rendering thread
	 */
	FORCEINLINE void Register( bool InIsRenderingThreadObject = false )
	{
		// Insert this object into the appropriate list
		bRenderingThreadObject = InIsRenderingThreadObject;
		if ( InIsRenderingThreadObject )
		{
			// Make sure that only the rendering thread is attempting to add items to the 'renderingThreadTickableObjects' list
			AssertMsg( IsInRenderingThread(), TEXT( "Game thread attempted to register an object in the RenderingThreadTickableObjects array" ) );
			renderingThreadTickableObjects.push_back( this );
		}
		else
		{
			// Make sure that only the game thread is attempting to add items to the 'tickableObjects' list
			AssertMsg( IsInGameThread(), TEXT( "Rendering thread attempted to register an object in the TickableObjects array" ) );
			tickableObjects.push_back( this );
		}
	}

	/**
	 * @brief Tick object
	 * @param InDeltaTime		Delta time
	 */
	virtual void Tick( float InDeltaTime ) = 0;

	/**
	 * @brief Is tickable object
	 * @return Return TRUE if class is ready to be ticked, FALSE otherwise
	 */
	virtual bool IsTickable() const = 0;

	static std::vector<CTickableObject*>	tickableObjects;					/**< Static array of tickable objects */
	static std::vector<CTickableObject*>	renderingThreadTickableObjects;		/**< Static array of tickable objects that are ticked from rendering thread */

private:
	bool		bRenderingThreadObject;		/**< Is rendering thread object */
};

#endif // !TICKABLEOBJECT_H
