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
#include "System/Delegate.h"
#include "WorldEd.h"

/**
 * @ingroup WorldEd
 * @brief Enumeration of gizmo type
 */
enum EGizmoType
{
	GT_None,			/**< None */
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
	 * @brief Delegate for called event when update all gizmo
	 */
	DECLARE_MULTICAST_DELEGATE( COnUpdateAllGizmo, bool /*InEnable*/, const Vector& /*InLocation*/ );

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
		if ( bEnabled == InEnable )
		{
			return;
		}

		bEnabled	= InEnable;
		currentAxis = A_None;
	}

	/**
	 * @brief Is enabled gizmo
	 * @return Return is enabled gizmo
	 */
	FORCEINLINE bool IsEnabled() const
	{
		return bEnabled;
	}

	/**
	 * @brief Set current axis
	 * @param InNewCurrentAxis		New current axis
	 */
	FORCEINLINE void SetCurrentAxis( uint32 InNewCurrentAxis )
	{
		currentAxis = InNewCurrentAxis;
	}

	/**
	 * @brief Get current axis
	 * @return Return current axis (see EAxis)
	 */
	FORCEINLINE uint32 GetCurrentAxis() const
	{
		return currentAxis;
	}

	/**
	 * @brief Get axis X end in screen space
	 * @return Return axis X end in screen space
	 */
	FORCEINLINE const Vector2D& GetAxisXEnd() const
	{
		return axisXEnd;
	}

	/**
	 * @brief Get axis Y end in screen space
	 * @return Return axis Y end in screen space
	 */
	FORCEINLINE const Vector2D& GetAxisYEnd() const
	{
		return axisYEnd;
	}

	/**
	 * @brief Get axis Z end in screen space
	 * @return Return axis Z end in screen space
	 */
	FORCEINLINE const Vector2D& GetAxisZEnd() const
	{
		return axisZEnd;
	}

	/**
	 * @brief Get location gizmo in screen space
	 * @return Return location gizmo in screen space
	 */
	FORCEINLINE const Vector2D& GetScreenLocation() const
	{
		return screenLocation;
	}

	/**
	 * @brief Get event of update all gizmo
	 * @return Return event of update all gizmo
	 */
	FORCEINLINE static COnUpdateAllGizmo& OnUpdateAllGizmo()
	{
		return onUpdateAllGizmo;
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
	 * @brief Render rotate gizmo. Must be call in render thread
	 *
	 * @param InViewportRHI		Viewport RHI
	 * @param InSceneView		Scene view
	 * @param InScene			Scene
	 */
	void Render_Rotate( ViewportRHIRef_t InViewportRHI, class CSceneView* InSceneView, class CScene* InScene );

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
	 * @param OutAxisEnd		Output axis end in screen coords
	 * @param InScale			Scale of axis
	 * @param InIsCubeHead		Is need cube head in axis
	 */
	void Render_Axis( const class CSceneView* InSceneView, struct SSceneDepthGroup& InSDG, EAxis InAxis, const Matrix& InMatrix, const TAssetHandle<CMaterial>& InMaterial, const CColor& InColor, Vector2D& OutAxisEnd, float InScale, bool InIsCubeHead = false );

	/**
	 * Event when editor mode is changed
	 * @param InEditorMode	New editor mode
	 */
	void OnEditorModeChanged( EEditorMode InEditorMode );

	/**
	 * Event when need update all gizmo
	 * 
	 * @param InEnable		Is need enable gizmo
	 * @param InLocation	New location
	 */
	void OnUpdateGizmo( bool InEnable, const Vector& InLocation );

	bool														bEnabled;						/**< Is enabled gizmo */
	EGizmoType													type;							/**< Gizmo type */
	Vector														location;						/**< Gizmo location */
	Vector2D													screenLocation;					/**< Screen location */
	CColor														axisColorX;						/**< Axis color X */
	CColor														axisColorY;						/**< Axis color Y */
	CColor														axisColorZ;						/**< Axis color Z */
	CColor														currentAxisColor;				/**< Current axis color */
	uint32														currentAxis;					/**< Current axies (see EAxis) */
	TAssetHandle<CMaterial>										axisMaterialX;					/**< Axis material X */
	TAssetHandle<CMaterial>										axisMaterialY;					/**< Axis material Y */
	TAssetHandle<CMaterial>										axisMaterialZ;					/**< Axis material Z */
	Vector2D													axisXEnd;						/**< Axis X end in screen coord */
	Vector2D													axisYEnd;						/**< Axis Y end in screen coord */
	Vector2D													axisZEnd;						/**< Axis Z end in screen coord */
	SEditorDelegates::COnEditorModeChanged::DelegateType_t*		editorModeChangedDelegate;		/**< Editor mode changed delegate */
	COnUpdateAllGizmo::DelegateType_t*							updateAllGizmoDelegate;			/**< Update all gizmo delegate */
	static COnUpdateAllGizmo									onUpdateAllGizmo;				/**< Event when update all gizmo */
};

#endif // !GIZMO_H