#include "Render/Shaders/TestShader.h"

IMPLEMENT_SHADER_TYPE( FTestVertexShader, TEXT( "Test.hlsl" ), TEXT( "VS" ), SF_Vertex );
IMPLEMENT_SHADER_TYPE( FTestPixelShader, TEXT( "Test.hlsl" ), TEXT( "PS" ), SF_Pixel );