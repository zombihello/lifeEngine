#include "vertexfactory.hlsl"

void MainVS( in VertexFactoryInput vfInput, out float4 screenPosition : SV_POSITION )
{
	screenPosition = VertexFactory_GetLocalPosition( vfInput );
}