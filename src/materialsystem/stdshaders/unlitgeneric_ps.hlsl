#include "common.hlsl"

void MainPS() : SV_Target
{
#if FILL_RED_COLOR
	#if !USE_GREEN_CHANNEL
		return float4( 1.f, 0.f, 0.f, 1.f );
	#else
		return float4( 0.f, 1.f, 0.f, 1.f );
	#endif // USE_GREEN_CHANNEL
#else
	return float4( 0.f, 0.f, 0.f, 1.f );
#endif // FILL_RED_COLOR	
}