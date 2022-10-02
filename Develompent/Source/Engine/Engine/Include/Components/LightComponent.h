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
	 * @brief Set light color
	 * @param InLightColor	Light color
	 */
	FORCEINLINE void SetLightColor( const CColor& InLightColor )
	{
		lightColor = InLightColor;
	}

	/**
	 * @brief Set specular color
	 * @param InSpecularColor	Specular color
	 */
	FORCEINLINE void SetSpecularColor( const CColor& InSpecularColor )
	{
		specularColor = InSpecularColor;
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
		return bEnabled;
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
	 * @brief Get specular color
	 * @return Return specular color
	 */
	FORCEINLINE const CColor& GetSpecularColor() const
	{
		return specularColor;
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
	CColor				specularColor;	/**< Specular color */
	float				intensivity;	/**< intensivity */
};

#endif // !LIGHTCOMPONENT_H