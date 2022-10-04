// This is used as a header, shared between C++ and HLSL, to help avoid duplication and magic numbers.

#ifndef CPP_GLOBALCONSTANTBUFFERS_HLSL
#define CPP_GLOBALCONSTANTBUFFERS_HLSL 0

#include "CPP_HLSL_Glue.hlsl"

#ifdef __cplusplus
MS_ALIGN( 16 ) struct SGlobalConstantBufferContents
#else
cbuffer GlobalConstants : register( SOB_GlobalConstants )
#endif // __cplusplus
{
	Matrix			viewMatrix;
	Matrix			projectionMatrix;
	Matrix			viewProjectionMatrix;
	Matrix			invViewProjectionMatrix;
	Vector4D		position;
	Vector4D		screenAndBufferSize;

#ifdef __cplusplus
} GCC_ALIGN( 16 );
#else
};
#endif // __cplusplus

#endif // !CPP_GLOBALCONSTANTBUFFERS_HLSL