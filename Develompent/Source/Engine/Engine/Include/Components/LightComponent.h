/**
 * @file
 * @addtogroup Engine Engine
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef LIGHTCOMPONENT_H
#define LIGHTCOMPONENT_H

#include "Misc/RefCountPtr.h"
#include "Math/Color.h"
#include "Components/SceneComponent.h"
#include "Actors/Actor.h"

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
	 * @brief Called before destroying the object
	 * This is called immediately upon deciding to destroy the object, to allow the object to begin an
	 * asynchronous cleanup process
	 */
	virtual void BeginDestroy() override;

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