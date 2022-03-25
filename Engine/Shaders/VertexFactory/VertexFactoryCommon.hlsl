#ifndef VERTEXFACTORYCOMMON_H
#define VERTEXFACTORYCOMMON_H 0

/* Matrix for convert from local to world coord system */
#if !USE_INSTANCING
	float4x4        localToWorldMatrix;
#endif // !USE_INSTANCING

#endif // !VERTEXFACTORYCOMMON_H