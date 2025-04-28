#define USL_DIRECTX_UNITY3D 0
#define USL_VULKAN 0
#define USL_OPENCL 1
#include "../libUSL/USL.inc"
#include "Voxel_Lib/iso_field_lib.inc"

void kernel main_cl(
	global const STRUCT Static_Settings* p_in_static_settings, 
	global const STRUCT Run_Settings* p_in_run_settings,
	global fvec4* p_out_data,
	global float* p_out_debug_data
)
{
	in_static_settings = p_in_static_settings;
	in_run_settings = p_in_run_settings;
	out_data = p_out_data;
	out_debug_data = p_out_debug_data;

	iso_field_gen_main(get_global_id(0));
}


