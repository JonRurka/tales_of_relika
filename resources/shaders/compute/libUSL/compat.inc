#ifndef _COMPAT_INC_
#define _COMPAT_INC_

#include "Types.inc"

//
// Compatibility functions
//

// https://stackoverflow.com/questions/7610631/glsl-mod-vs-hlsl-fmod
#define FMOD(x, y) (x - y * floor(x/y))


#define FABS(val) abs(val)


#define SQR_MAG(val) dot(val, val)

float sqrMagnitude(fvec3 val) {
	//return val.x * val.x + val.y * val.y + val.z * val.z;
	return dot(val, val);
}

#define VECTOR_ANGLE (out_val, from, to) 				\
    float num = sqrt(SQR_MAG(from) * SQR_MAG(to)); 		\
    if (num < 0.0000000000000001) 						\
    { 													\
        out_val = 0; 									\
    } 													\
	else { 												\
		float num2 = clamp(dot(from, to) / num, -1, 1); \
		out_val = acos(num2) * 57.29578f; 				\
	}

float vector_angle(fvec3 from, fvec3 to){
    float num = sqrt(SQR_MAG(from) * SQR_MAG(to));
    if (num < 0.0000000000000001)
    {
        return 0;
    }
    float num2 = clamp(dot(from, to) / num, -1, 1);
    return acos(num2) * 57.29578f;
}

#define TRIANGLE_NORMAL(v1, v2, v3) normalize(cross(v2 - v1, v3 - v1))

#define EQUALS_3D(v1, v2) (v1.x == v2.x && v1.y == v2.y && v1.z == v2.z)

#if USL_VULKAN == 1

#define LERP(__start__, __end__, __delta__) mix(__start__, __end__, __delta__)

#elif USL_OPENCL == 1

#define LERP(__start__, __end__, __delta__) lerp(__start__, __end__, __delta__)


#elif USL_DIRECTX_UNITY3D == 1

#define LERP(__start__, __end__, __delta__) lerp(__start__, __end__, __delta__)


#endif



#endif // _COMPAT_INC_