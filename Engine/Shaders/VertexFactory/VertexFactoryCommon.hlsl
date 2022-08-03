#ifndef VERTEXFACTORYCOMMON_H
#define VERTEXFACTORYCOMMON_H 0

#if !USE_INSTANCING
	/* Matrix for convert from local to world coord system */
	float4x4        localToWorldMatrix;
	
	/* Hit proxy Id */
	#if ENABLE_HITPROXY
		float4			hitProxyId;
	#endif // ENABLE_HITPROXY
	
	/* Color overlay for mark selected actors */
	#if WITH_EDITOR
		float4			colorOverlay;
	#endif // WITH_EDITOR
#endif // !USE_INSTANCING

#endif // !VERTEXFACTORYCOMMON_H