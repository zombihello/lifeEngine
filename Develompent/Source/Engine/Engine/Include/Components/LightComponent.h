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
#include "Components/SceneComponent.h"

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
	LT_Unknown,		/**< Unknown light */
	LT_Point,		/**< Point light */
	LT_Spot,		/**< Spot light */
	LT_Directional	/**< Directional light */
};

/**
 * @ingroup Engine
 * Component of base light
 */
class CLightComponent : public CSceneComponent
{
	DECLARE_CLASS( CLightComponent, CSceneComponent )

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
	 * @brief Get scene
	 * @return Return scene. If light not added to scene return NULL
	 */
	FORCEINLINE class CScene* GetScene() const
	{
		return scene;
	}

	/**
	 * @brief Get light type
	 * @return Return light type
	 */
	FORCEINLINE ELightType GetLightType() const
	{
		return lightType;
	}

	/**
	 * @brief Is enabled
	 * @return Return TRUE if the light component is enabled
	 */
	FORCEINLINE bool IsEnabled() const
	{
		return bEnabled;
	}

protected:
	bool				bEnabled;		/**< Is enabled the light component */
	class CScene*		scene;			/**< The current scene where the primitive is located  */
	ELightType			lightType;		/**< Light type */
};

#endif // !LIGHTCOMPONENT_H