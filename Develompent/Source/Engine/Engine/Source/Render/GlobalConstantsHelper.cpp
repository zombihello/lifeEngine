#include "Render/GlobalConstantsHelper.h"

/*
==================
SetGlobalConstants
==================
*/
void SetGlobalConstants( SGlobalConstantBufferContents& OutGlobalContents, const CSceneView& InSceneView, const Vector4D& InScreenAndBufferSize )
{
	OutGlobalContents.viewMatrix					= InSceneView.GetViewMatrix();
	OutGlobalContents.projectionMatrix				= InSceneView.GetProjectionMatrix();
	OutGlobalContents.viewProjectionMatrix			= InSceneView.GetViewProjectionMatrix();
	OutGlobalContents.invViewProjectionMatrix		= InSceneView.GetInvViewProjectionMatrix();
	OutGlobalContents.position						= Vector4D( InSceneView.GetPosition(), 1.f );
	OutGlobalContents.screenAndBufferSize			= InScreenAndBufferSize;
}