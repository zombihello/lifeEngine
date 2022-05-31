#include "Render/GlobalConstantsHelper.h"

void SetGlobalConstants( SGlobalConstantBufferContents& OutGlobalContents, const CSceneView& InSceneView )
{
	OutGlobalContents.viewMatrix				= InSceneView.GetViewMatrix();
	OutGlobalContents.projectionMatrix			= InSceneView.GetProjectionMatrix();
	OutGlobalContents.viewProjectionMatrix		= InSceneView.GetViewProjectionMatrix();
}