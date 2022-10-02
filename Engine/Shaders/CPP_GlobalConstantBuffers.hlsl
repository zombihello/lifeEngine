// This is used as a header, shared between C++ and HLSL, to help avoid duplication and magic numbers.

#ifndef CPP_GLOBALCONSTANTBUFFERS_HLSL
#define CPP_GLOBALCONSTANTBUFFERS_HLSL 0

#include "CPP_HLSL_Glue.hlsl"

#ifdef __cplusplus
struct SGlobalConstantBufferContents
#else
cbuffer GlobalConstants : register( SOB_GlobalConstants )
#endif // __cplusplus
{
	Matrix			viewMatrix;
	Matrix			projectionMatrix;
	Matrix			viewProjectionMatrix;
	Matrix			invViewMatrix;
	Matrix			invProjectionMatrix;
	Matrix			invViewProjectionMatrix;
	Vector			position;
};

#endif // !CPP_GLOBALCONSTANTBUFFERS_HLSL