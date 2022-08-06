/**
 * @file
 * @addtogroup WorldEd WorldEd
 *
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */

#ifndef GIZMO_H
#define GIZMO_H

#include "Math/Math.h"
#include "Math/Color.h"
#include "Render/Material.h"
#include "System/Package.h"

/**
 * @ingroup WorldEd
 * @brief Enumeration of gizmo type
 */
enum EGizmoType
{
	GT_Translate,		/**< Gizmo translate */
	GT_Rotate,			/**< Gizmo rotate */
	GT_Scale,			/**< Gizmo scale */
};

/**
 * @ingroup WorldEd
 * @brief Gizmo tool
 */
class CGizmo
{
public:
	/**
	 * @brief Constructor
	 */
	CGizmo();

	/**
	 * @brief Destructor
	 */
	~CGizmo();

	/**
	 * @brief Init gizmo
	 */
	void Init();

	/**
	 * @brief Draw viewport. Must be call in render thread
	 *
	 * @param InViewportRHI		Viewport RHI
	 * @param InSceneView		Scene view
	 * @param InScene			Scene
	 */
	void Draw_RenderThread( ViewportRHIRef_t InViewportRHI, class CSceneView* InSceneView, class CScene* InScene );

	/**
	 * @brief Set type
	 * @param InType	Gizmo type
	 */
	FORCEINLINE void SetType( EGizmoType InType )
	{
		type = InType;
	}

	/**
	 * @brief Get type
	 * @return Return type of gizmo
	 */
	FORCEINLINE EGizmoType GetType() const
	{
		return type;
	}

	/**
	 * @brief Set location
	 * @param InNewLocation		New location
	 */
	FORCEINLINE void SetLocation( const Vector& InNewLocation )
	{
		location = InNewLocation;
	}

	/**
	 * @brief Get location
	 * @return Return location of gizmo
	 */
	FORCEINLINE const Vector& GetLocation() const
	{
		return location;
	}

	/**
	 * @brief Set enable gizmo
	 * @param InEnable		Enable gizmo
	 */
	FORCEINLINE void SetEnable( bool InEnable )
	{
		bEnabled = InEnable;
	}

	/**
	 * @brief Is enabled gizmo
	 * @return Return is enabled gizmo
	 */
	FORCEINLINE bool IsEnabled() const
	{
		return bEnabled;
	}

private:
	/**
	 * @brief Render translate gizmo. Must be call in render thread
	 * 
	 * @param InViewportRHI		Viewport RHI
	 * @param InSceneView		Scene view
	 * @param InScene			Scene
	 */
	void Render_Translate( ViewportRHIRef_t InViewportRHI, class CSceneView* InSceneView, class CScene* InScene );

	/**
	 * @brief Render scale gizmo. Must be call in render thread
	 *
	 * @param InViewportRHI		Viewport RHI
	 * @param InSceneView		Scene view
	 * @param InScene			Scene
	 */
	void Render_Scale( ViewportRHIRef_t InViewportRHI, class CSceneView* InSceneView, class CScene* InScene );

	/**
	 * @brief Render axis for translate gizmo
	 * 
	 * @param InSceneView		Scene view
	 * @param InSDG				Scene depth group
	 * @param InAxis			Axis type
	 * @param InMatrix			Matrix
	 * @param InMaterial		Material
	 * @param InColor			Color of axis
	 * @param InScale			Scale of axis
	 * @param InIsCubeHead		Is need cube head in axis
	 */
	void Render_Axis( const class CSceneView* InSceneView, struct SSceneDepthGroup& InSDG, EAxis InAxis, const Matrix& InMatrix, const TAssetHandle<CMaterial>& InMaterial, const CColor& InColor, float InScale, bool InIsCubeHead = false );

	bool						bEnabled;		/**< Is enabled gizmo */
	EGizmoType					type;			/**< Gizmo type */
	Vector						location;		/**< Gizmo location */
	CColor						axisColorX;		/**< Axis color X */
	CColor						axisColorY;		/**< Axis color Y */
	CColor						axisColorZ;		/**< Axis color Z */
	TAssetHandle<CMaterial>		axisMaterialX;	/**< Axis material X */
	TAssetHandle<CMaterial>		axisMaterialY;	/**< Axis material Y */
	TAssetHandle<CMaterial>		axisMaterialZ;	/**< Axis material Z */
};

#endif // !GIZMO_H