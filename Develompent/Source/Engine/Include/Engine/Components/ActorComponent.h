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

#ifndef ACTORCOMPONENT_H
#define ACTORCOMPONENT_H

#include "Core/Misc/Object.h"
#include "Core/Misc/Class.h"
#include "Core/Misc/RefCounted.h"
#include "Engine/Misc/EngineTypes.h"
#include "Core/Misc/Property.h"

/**
 * @ingroup Engine
 * ActorComponent is the base class for components that define reusable behavior that can be added to different types of Actors.
 * ActorComponents that have a transform are known as SceneComponents and those that can be rendered are PrimitiveComponents.
 */
class CActorComponent : public CObject, public CRefCounted
{
	DECLARE_CLASS( CActorComponent, CObject, 0, 0 )

public:
	/**
	 * Constructor
	 */
	CActorComponent();

	/**
	 * Destructor
	 */
	virtual ~CActorComponent();

	/**
	 * Begins Play for the component.
	 * Called when the owning Actor begins play or when the component is created if the Actor has already begun play.
	 */
	virtual void BeginPlay();

	/**
	 * End play for the component.
	 * Called when owning actor eng play
	 */
	virtual void EndPlay();

	/**
	 * Function called every frame on this ActorComponent. Override this function to implement custom logic to be executed every frame.
	 * 
	 * @param[in] InDeltaTime The time since the last tick.
	 */
	virtual void TickComponent( float InDeltaTime );

	/**
	 * @brief Called when the owning Actor is spawned
	 */
	virtual void Spawned();

	/**
	 * @brief Called when this component is destroyed
	 */
	virtual void Destroyed();

	/**
	 * @brief Get owner
	 * @return Return owner
	 */
	class AActor* GetOwner() const;

#if WITH_EDITOR
	/**
	 * Set editor only
	 * @param InEditorOnly	Is component only for editor
	 */
	FORCEINLINE void SetEditorOnly( bool InEditorOnly )
	{
		bEditorOnly = InEditorOnly;
	}

	/**
	 * Is editor only
	 * @return Return TRUE if component only for editor
	 */
	FORCEINLINE bool IsEditorOnly() const
	{
		return bEditorOnly;
	}
#endif // WITH_EDITOR

private:
#if WITH_EDITOR
	bool					bEditorOnly;	/**< Is component only for editor */
#endif // WITH_EDITOR
};

#endif // !ACTORCOMPONENT_H