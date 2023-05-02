#include "Render/Shaders/DepthOnlyShader.h"
#include "Render/VertexFactory/VertexFactory.h"
#include "Render/Material.h"
#include "Render/Scene.h"

IMPLEMENT_SHADER_TYPE( template<>, TDepthOnlyVertexShader<false>, TEXT( "DepthOnlyVertexShader.hlsl" ), TEXT( "MainVS" ), SF_Vertex, true );
IMPLEMENT_SHADER_TYPE( template<>, TDepthOnlyVertexShader<true>, TEXT( "DepthOnlyVertexShader.hlsl" ), TEXT( "MainVS" ), SF_Vertex, true );
IMPLEMENT_SHADER_TYPE(, CDepthOnlyPixelShader, TEXT( "DepthOnlyPixelShader.hlsl" ), TEXT( "MainPS" ), SF_Pixel, true );