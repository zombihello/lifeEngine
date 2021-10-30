// This is used as a header, shared between C++ and HLSL, to help avoid duplication and magic numbers.

#ifndef CPP_HLSL_GLUE_HLSL
#define CPP_HLSL_GLUE_HLSL 0

#ifndef __cplusplus
	typedef float4		FVector4D;
	typedef float4x4	FMatrix;
	typedef float3		FVector;
	typedef float2		FVector2D;
#else
	#include "Math/Math.h"
#endif // !__cplusplus

#endif // !CPP_HLSL_GLUE_HLSL