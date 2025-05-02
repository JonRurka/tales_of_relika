#define USL_DIRECTX_UNITY3D 0
#define USL_VULKAN 0
#define USL_OPENCL 1
#include "../libUSL/USL.inc"
#include "Voxel_Lib/smoothrender_async_stitch_lib.inc"

void kernel main_cl(
	global STRUCT Static_Settings* p_in_static_settings, 
	global STRUCT Run_Settings* p_in_run_settings,
	
	global fvec4* p_in_vertex_data,
	global fvec4* p_in_normal_data,
	global int* p_in_tris_data,
	
	global int* p_in_counts_data,
	global int* p_in_stitch_map_data,
	__constant ivec4* p_in_stitch_map_offsets,
	
	global fvec4* p_out_vertex_data,
	global fvec4* p_out_normal_data,
	global int* p_out_triangles_data,
	
	global ivec4* p_out_counts_data,
	
	global fvec4* p_out_debug_data
)
{
	in_static_settings = p_in_static_settings;
	in_run_settings = p_in_run_settings;
	
	in_vertex_data = p_in_vertex_data;
	in_normal_data = p_in_normal_data;
	in_tris_data = p_in_tris_data;
	
	in_counts_data = p_in_counts_data;
	in_stitch_map_data = p_in_stitch_map_data;
	in_stitch_map_offsets = p_in_stitch_map_offsets;
	
	out_vertex_data = p_out_vertex_data;
	out_normal_data = p_out_normal_data;
	out_triangles_data = p_out_triangles_data;
	
	out_counts_data = p_out_counts_data;
	
	out_debug_data = p_out_debug_data;

	smoothrender_async_stitch_main(get_global_id(0));
}



