#include "Render/Shaders/BasePassShader.h"

IMPLEMENT_SHADER_TYPE( FBasePassVertexShader, TEXT( "BasePassVertexShader.hlsl" ), TEXT( "MainVS" ), SF_Vertex );
IMPLEMENT_SHADER_TYPE( FBasePassPixelShader, TEXT( "BasePassPixelShader.hlsl" ), TEXT( "MainPS" ), SF_Pixel );