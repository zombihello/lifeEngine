// This is used as a header, shared between C++ and HLSL, to help avoid duplication and magic numbers.

#ifndef CPP_GLOBALCONSTANTBUFFERS_HLSL
#define CPP_GLOBALCONSTANTBUFFERS_HLSL 0

#include "CPP_HLSL_Glue.hlsl"

#ifdef __cplusplus
struct FGlobalConstantBufferContents
#else
#include "Common.hlsl"
cbuffer GlobalConstants : register( SOB_GlobalConstants )
#endif // __cplusplus
{
	FMatrix			viewMatrix;
	FMatrix			projectionMatrix;
	FMatrix			viewProjectionMatrix;
};

#endif // !CPP_GLOBALCONSTANTBUFFERS_HLSL