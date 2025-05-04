#define USL_DIRECTX_UNITY3D 0
#define USL_VULKAN 0
#define USL_OPENCL 1
#include "../libUSL/USL.inc"
#include "Voxel_Lib/smoothrender_mark_lib.inc"

void kernel main_cl(
	global STRUCT Static_Settings* p_in_static_settings,
	global ivec4* p_in_run_settings,
	
	global int* p_in_counts_data,
	
	global int* p_out_stitch_map_data,
	global ivec4* p_out_stitch_map_offsets,
	global ivec4* p_out_counts_data,
	
	global fvec4* p_out_debug_data
)
{
	in_static_settings = p_in_static_settings;
	in_run_settings = p_in_run_settings;
	in_counts_data = p_in_counts_data;
	out_stitch_map_data = p_out_stitch_map_data;
	out_stitch_map_offsets = p_out_stitch_map_offsets;
	out_counts_data = p_out_counts_data;
	out_debug_data = p_out_debug_data;
	
	smoothrender_mark_async_main(get_global_id(0));
}

