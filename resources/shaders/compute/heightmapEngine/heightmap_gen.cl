#define USL_DIRECTX_UNITY3D 0
#define USL_VULKAN 0
#define USL_OPENCL 1
#include "../libUSL/USL.inc"
#include "heightmap_lib/heightmap_lib.inc"

void kernel main_cl(
	global STRUCT Static_Settings* p_in_static_settings, 
	global STRUCT Run_Settings* p_in_run_settings,
	global float* p_out_height_data,
	global fvec4* p_out_height_neighboor_data,
	global int* p_out_biome_data)
{
	in_static_settings = p_in_static_settings;
	in_run_settings = p_in_run_settings;
	out_height_data = p_out_height_data;
	out_height_neighboor_data = p_out_height_neighboor_data;
	out_biome_data = p_out_biome_data;

	heightmap_gen_main(get_global_id(0));
}