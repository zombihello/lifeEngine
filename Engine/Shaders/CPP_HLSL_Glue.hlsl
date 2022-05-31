// This is used as a header, shared between C++ and HLSL, to help avoid duplication and magic numbers.

#ifndef CPP_HLSL_GLUE_HLSL
#define CPP_HLSL_GLUE_HLSL 0

#ifndef __cplusplus
	typedef float4		Vector4D;
	typedef float4x4	Matrix;
	typedef float3		Vector;
	typedef float2		Vector2D;
#else
	#include "Math/Math.h"
#endif // !__cplusplus

#endif // !CPP_HLSL_GLUE_HLSL