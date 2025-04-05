#ifndef _TYPES_INC_
#define _TYPES_INC_


// OpenCL requires the 'struct' keyword before any variable declaration of a custom struct.
#ifdef OPENCL
#define STRUCT struct
#else
#define STRUCT 
#endif

#if defined(VULKAN)

#define fvector2 vec2
#define fvector3 vec3
#define fvector4 vec4

#define ivector2 ivec2
#define ivector3 ivec3
#define ivector4 ivec4

#define uvector2 uvec2
#define uvector3 uvec3
#define uvector4 uvec4

#else

#define fvector2 float2
#define fvector3 float3
#define fvector4 float4

#define ivector2 float2
#define ivector3 float3
#define ivector4 float4

#define uvector2 uint2
#define uvector3 uint3
#define uvector4 uint4

#endif

#endif //_TYPES_INC_