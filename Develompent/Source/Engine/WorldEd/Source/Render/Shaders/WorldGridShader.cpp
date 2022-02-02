#include "Render/Shaders/WorldGridShader.h"

IMPLEMENT_SHADER_TYPE( FWorldGridVertexShader, TEXT( "WorldGridShaders.hlsl" ), TEXT( "MainVS" ), SF_Vertex );
IMPLEMENT_SHADER_TYPE( FWorldGridPixelShader, TEXT( "WorldGridShaders.hlsl" ), TEXT( "MainPS" ), SF_Pixel );