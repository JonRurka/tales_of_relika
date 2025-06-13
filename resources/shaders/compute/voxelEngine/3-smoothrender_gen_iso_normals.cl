#define USL_DIRECTX_UNITY3D 0
#define USL_VULKAN 0
#define USL_OPENCL 1
#define RUNTIME_INSERTED_TYPES
#include "../libUSL/USL.inc"
#include "Voxel_Lib/smoothrender_gen_iso_normals.inc"

void kernel main_cl(
	global STRUCT Static_Settings* p_in_static_settings, 
	global STRUCT Run_Settings* p_in_run_settings,
	
	global float* p_in_heightmap_data,
	global fvec4* p_in_height_neighboor_data,
	global fvec4* p_out_iso_type,
	global fvec4* p_out_normal
)
{
	in_static_settings = p_in_static_settings;
	in_run_settings = p_in_run_settings;
	in_run_settings = p_in_run_settings;
	in_heightmap_data = p_in_heightmap_data;
	in_height_neighboor_data = p_in_height_neighboor_data;
	out_iso_type = p_out_iso_type;
	out_normal = p_out_normal;

	smoothrender_gen_iso_normal_main(get_global_id(0));
}



