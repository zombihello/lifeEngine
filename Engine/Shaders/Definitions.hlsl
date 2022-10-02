/**
 * Definitions.hlsl: Defines undefined defines to 0.
 * 
 * Copyright Broken Singularity, All Rights Reserved.
 * Authors: Yehor Pohuliaka (zombiHello)
 */
 
 #ifndef DEFINITIONS_H
 #define DEFINITIONS_H 0
 
 #ifndef USE_INSTANCING
 #define USE_INSTANCING						0
 #endif // !USE_INSTANCING
 
#ifndef POINT_LIGHT
#define POINT_LIGHT                         0
#endif // !POINT_LIGHT

#ifndef SPOT_LIGHT
#define SPOT_LIGHT                          0
#endif // !SPOT_LIGHT

#ifndef DIRECTIONAL_LIGHT
#define DIRECTIONAL_LIGHT                   0
#endif // !DIRECTIONAL_LIGHT

#ifndef ORTHOGRAPHIC_PROJECTION
#define ORTHOGRAPHIC_PROJECTION             0
#endif // !ORTHOGRAPHIC_PROJECTION

 #endif // !DEFINITIONS_H