#include "Misc/EngineGlobals.h"
#include "System/BaseEngine.h"
#include "Render/WorldGridDrawingPolicy.h"
#include "Render/VertexFactory/WorldGridVertexFactory.h"
#include "Render/Shaders/WireframeShader.h"

void FWorldGridDrawingPolicy::InitInternal( class FVertexFactory* InVertexFactory, class FMaterial* InMaterial, float InDepthBias /* = 0.f */ )
{
	FMeshDrawingPolicy::InitInternal( InVertexFactory, InMaterial, InDepthBias );

	//
	// Override vertex and pixel shader, because material not save shaders for world grid
	//
	// Getting vertex shader
	{
		FShaderMetaType*		shaderMetaType = InMaterial->GetShaderType( SF_Vertex );
		if ( shaderMetaType )
		{
			vertexShader = GShaderManager->FindInstance( shaderMetaType->GetName(), FWorldGridVertexFactory::staticType.GetHash() );
		}
		else
		{
			vertexShader = nullptr;
		}
	}

	// Getting pixel shader
	{
		FShaderMetaType*		shaderMetaType = InMaterial->GetShaderType( SF_Pixel );
		if ( shaderMetaType )
		{
			pixelShader = GShaderManager->FindInstance( shaderMetaType->GetName(), FWorldGridVertexFactory::staticType.GetHash() );
		}
		else
		{
			pixelShader = nullptr;
		}
	}
}