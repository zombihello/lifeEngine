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

#ifndef LIGHTCOMPONENT_H
#define LIGHTCOMPONENT_H

#include "Core/Misc/RefCountPtr.h"
#include "Core/Math/Color.h"
#include "Engine/Components/SceneComponent.h"
#include "Engine/Actors/Actor.h"

/**
 * @ingroup Engine
 * Typedef of pointer to light component
 */
typedef TRefCountPtr<class CLightComponent>		LightComponentRef_t;

/**
 * @ingroup Engine
 * Enumeration of light type
 */
enum ELightType
{
	LT_Unknown = -1,	/**< Unknown light */
	LT_Point,			/**< Point light */
	LT_Spot,			/**< Spot light */
	LT_Directional,		/**< Directional light */
	LT_Num				/**< Number of light types */
};

/**
 * @ingroup Engine
 * Component of base light
 */
class CLightComponent : public CSceneComponent
{
	DECLARE_CLASS( CLightComponent, CSceneComponent, 0, 0 )

public:
	friend class CScene;			// For add and remove lights in scene

	/**
	 * @brief Constructor
	 */
	CLightComponent();

	/**
	 * @brief Destructor
	 */
	virtual ~CLightComponent();

	/**
	 * @brief Serialize component
	 * @param[in] InArchive Archive for serialize
	 */
	virtual void Serialize( class CArchive& InArchive ) override;

	/**
	 * @brief Called when the owning Actor is spawned
	 */
	virtual void Spawned() override;

	/**
	 * @brief Called when this component is destroyed
	 */
	virtual void Destroyed() override;

	/**
	 * @brief Set enable the light component
	 * @param InEnabled		Is enabled the light component
	 */
	FORCEINLINE void SetEnabled( bool InEnabled )
	{
		bEnabled = InEnabled;
	}

	/**
	 * @brief Set light color
	 * @param InLightColor	Light color
	 */
	FORCEINLINE void SetLightColor( const CColor& InLightColor )
	{
		lightColor = InLightColor;
	}

	/**
	 * @brief Set intensivity
	 * @param InIntensivity		Intensivity
	 */
	FORCEINLINE void SetIntensivity( float InIntensivity )
	{
		intensivity = InIntensivity;
	}

	/**
	 * @brief Get scene
	 * @return Return scene. If light not added to scene return NULL
	 */
	FORCEINLINE class CScene* GetScene() const
	{
		return scene;
	}

	/**
	 * @brief Get light type
	 * Need override the method by child for setting light type
	 * 
	 * @return Return light type
	 */
	virtual ELightType GetLightType() const;

	/**
	 * @brief Is enabled
	 * @return Return TRUE if the light component is enabled
	 */
	FORCEINLINE bool IsEnabled() const
	{
		return bEnabled && ( GetOwner() ? GetOwner()->IsVisibility() : true );
	}

	/**
	 * @brief Get light color
	 * @return Return light color
	 */
	FORCEINLINE const CColor& GetLightColor() const
	{
		return lightColor;
	}

	/**
	 * @brief Get intensivity
	 * @return Return intensivity
	 */
	FORCEINLINE float GetIntensivity() const
	{
		return intensivity;
	}

protected:
	bool				bEnabled;		/**< Is enabled the light component */
	class CScene*		scene;			/**< The current scene where the primitive is located  */
	CColor				lightColor;		/**< Light color */
	float				intensivity;	/**< intensivity */
};

#endif // !LIGHTCOMPONENT_H