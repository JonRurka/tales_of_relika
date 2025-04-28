#define USL_DIRECTX_UNITY3D 0
#define USL_VULKAN 0
#define USL_OPENCL 1
#include "../libUSL/USL.inc"
#include "Voxel_Lib/smoothrender_async_stitch_lib.inc"

void kernel main_cl(global const float* in_data, global float* out_data)
{
	smoothrender_async_stitch_main(get_global_id(0));
}



