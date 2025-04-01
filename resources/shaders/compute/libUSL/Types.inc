#ifndef _TYPES_INC_
#define _TYPES_INC_


// OpenCL requires the 'struct' keyword before any variable declaration of a custom struct.
#if USL_OPENCL == 1
#define STRUCT struct
#else
#define STRUCT 
#endif

#if USL_VULKAN == 1

#define fvec2 vec2
#define fvec3 vec3
#define fvec4 vec4

#define ivec2 ivec2
#define ivec3 ivec3
#define ivec4 ivec4

#define uvec2 uvec2
#define uvec3 uvec3
#define uvec4 uvec4

#define fmat4x4 mat4x4
#define fmat3x3 mat3x3

#define INT(val) int(val)

#define UINT(val) uint(val)

#define FLOAT(val) float(val)

#else // OpenCL or DirectComp

#define fvec2 float2
#define fvec3 float3
#define fvec4 float4

#define ivec2 int2
#define ivec3 int3
#define ivec4 int4

#define uvec2 uint2
#define uvec3 uint3
#define uvec4 uint4

#define fmat4x4 float4x4
#define fmat3x3 float3x3

#define INT(val) (int)(val)

#define UINT(val) (uint)(val)

#define FLOAT(val) (float)(val)


#endif // OpenCL or DirectComp


#if USL_DIRECTX_UNITY3D == 1 || USL_VULKAN == 1



#define FVEC2(x, y) fvec2(x, y)

#define IVEC2(x, y) ivec2(x, y)

#define UVEC2(x, y) uvec2(x, y)


#define FVEC3(x, y, z) fvec3(x, y, z)

#define IVEC3(x, y, z) ivec3(x, y, z)

#define UVEC3(x, y, z) uvec3(x, y, z)


#define FVEC4(x, y, z, w) fvec4(x, y, z, w)

#define IVEC4(x, y, z, w) ivec4(x, y, z, w)

#define UVEC4(x, y, z, w) uvec4(x, y, z, w)



#elif USL_OPENCL == 1

#define FVEC2(x, y) (fvec2)(x, y)

#define IVEC2(x, y) (ivec2)(x, y)

#define UVEC2(x, y) (uvec2)(x, y)


#define FVEC3(x, y, z) (fvec3)(x, y, z)

#define IVEC3(x, y, z) (ivec3)(x, y, z)

#define UVEC3(x, y, z) (uvec3)(x, y, z)


#define FVEC4(x, y, z, w) (fvec4)(x, y, z, w)

#define IVEC4(x, y, z, w) (ivec4)(x, y, z, w)

#define UVEC4(x, y, z, w) (uvec4)(x, y, z, w)

#endif

#define FVEC2_ZERO FVEC2(0, 0)

#define IVEC2_ZERO IVEC2(0, 0)

#define UVEC2_ZERO UVEC2(0, 0)


#define FVEC3_ZERO FVEC3(0, 0, 0)

#define IVEC3_ZERO IVEC3(0, 0, 0)

#define UVEC3_ZERO UVEC3(0, 0, 0)


#define FVEC4_ZERO FVEC4(0, 0, 0, 0)

#define IVEC4_ZERO IVEC4(0, 0, 0, 0)

#define UVEC4_ZERO UVEC4(0, 0, 0, 0)


#if USL_VULKAN == 1

#define CONST_ARRAY const

#elif USL_DIRECTX_UNITY3D == 1

#define CONST_ARRAY static const

#endif
// TODO: OpenCL


#endif // _TYPES_INC_