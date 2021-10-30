#include "Render/GlobalConstantsHelper.h"

void SetGlobalConstants( FGlobalConstantBufferContents& OutGlobalContents, const FSceneView& InSceneView )
{
	OutGlobalContents.viewMatrix				= InSceneView.GetViewMatrix();
	OutGlobalContents.projectionMatrix			= InSceneView.GetProjectionMatrix();
	OutGlobalContents.viewProjectionMatrix		= InSceneView.GetViewProjectionMatrix();
}