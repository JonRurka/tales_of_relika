#define USL_DIRECTX_UNITY3D 0
#define USL_VULKAN 0
#define USL_OPENCL 1
#define RUNTIME_INSERTED_TYPES
#include "../libUSL/USL.inc"
#include "Voxel_Lib/smoothrender_construct_lib.inc"

void kernel main_cl(
	global STRUCT Static_Settings* p_in_static_settings, 
	global STRUCT Run_Settings* p_in_run_settings,
	
	__constant fvec4* p_in_locOffset,
	__constant fvec4* p_in_globalOffsets,
	__constant ivec4* p_in_directionOffsets,
	
	__constant int* p_in_edgeTable_data,
	__constant int* p_in_TriTable_data,
	
	global float* p_in_heightmap_data,
	global fvec4* p_in_height_neighboor_data,
	
	global fvec4* p_in_iso_type,
	global fvec4* p_in_normal,
	
	global fvec4* p_out_vertex_data,
	global fvec4* p_out_normal_data,
	global int* p_out_triangles_data,
	global int* p_out_counts_data,
	global int* p_out_stitch_map_data,
	
	global fvec4* p_out_debug_data
)
{
	in_static_settings = p_in_static_settings;
	in_run_settings = p_in_run_settings;
	in_locOffset = p_in_locOffset;
	in_globalOffsets = p_in_globalOffsets;
	in_directionOffsets = p_in_directionOffsets;
	in_edgeTable_data = p_in_edgeTable_data;
	in_TriTable_data = p_in_TriTable_data;
	in_heightmap_data = p_in_heightmap_data;
	in_height_neighboor_data = p_in_height_neighboor_data;
	in_iso_type = p_in_iso_type;
	in_normal = p_in_normal;
	out_vertex_data = p_out_vertex_data;
	out_normal_data = p_out_normal_data;
	out_triangles_data = p_out_triangles_data;
	out_counts_data = p_out_counts_data;
	out_stitch_map_data = p_out_stitch_map_data;
	out_debug_data = p_out_debug_data;

	smoothrender_construct_main(get_global_id(0));
}



