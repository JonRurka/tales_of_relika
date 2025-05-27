#ifndef _COMPAT_INC_
#define _COMPAT_INC_

#include "Types.inc"

//
// Compatibility functions
//

// https://stackoverflow.com/questions/7610631/glsl-mod-vs-hlsl-fmod
#define FMOD(x, y) (x - y * floor(x/y))

#define FRAC(x) (x - floor(x))

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
		float dot_val1 = dot(from, to);					\
		float num2 = clamp(dot_val1 / num, -1.0f, 1.0f);	\
		out_val = acos(num2) * 57.29578f; 				\
	}

float vector_angle(fvec3 from, fvec3 to){
    float num = sqrt(SQR_MAG(from) * SQR_MAG(to));
    if (num < 0.0000000000000001)
    {
        return 0;
    }
	float dot_val2 = dot(from, to);
    float num2 = clamp(dot_val2 / num, -1.0f, 1.0f);
    return acos(num2) * 57.29578f;
}

#define TRIANGLE_NORMAL(v1, v2, v3) normalize(cross((v2 - v1).xyz, (v3 - v1).xyz))

#define EQUALS_3D(v1, v2) (v1.x == v2.x && v1.y == v2.y && v1.z == v2.z)

#if USL_VULKAN == 1

#define FABS(val) abs(val)
#define LERP(_start_, _end_, _delta_) mix(_start_, _end_, _delta_)

#elif USL_OPENCL == 1

#define FABS(val) fabs(val)
#define LERP(_start_, _end_, _delta_) lerp(_start_, _end_, _delta_)
#define RountToInt(x) (convert_int(x))

#elif USL_DIRECTX_UNITY3D == 1

#define FABS(val) abs(val)
#define LERP(_start_, _end_, _delta_) lerp(_start_, _end_, _delta_)


#endif



#endif // _COMPAT_INC_