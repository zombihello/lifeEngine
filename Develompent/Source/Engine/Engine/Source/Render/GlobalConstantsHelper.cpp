#include "Render/GlobalConstantsHelper.h"

void SetGlobalConstants( SGlobalConstantBufferContents& OutGlobalContents, const CSceneView& InSceneView )
{
	OutGlobalContents.viewMatrix					= InSceneView.GetViewMatrix();
	OutGlobalContents.projectionMatrix				= InSceneView.GetProjectionMatrix();
	OutGlobalContents.viewProjectionMatrix			= InSceneView.GetViewProjectionMatrix();
	OutGlobalContents.invViewMatrix					= InSceneView.GetInvViewMatrix();
	OutGlobalContents.invProjectionMatrix			= InSceneView.GetInvProjectionMatrix();
	OutGlobalContents.invViewProjectionMatrix		= InSceneView.GetInvViewProjectionMatrix();
	OutGlobalContents.position						= InSceneView.GetPosition();
}