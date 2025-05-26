#define USL_DIRECTX_UNITY3D 0
#define USL_VULKAN 0
#define USL_OPENCL 1
#include "../libUSL/USL.inc"
#include "Voxel_Lib/unify_fields_lib.inc"

void kernel main_cl(
	global STRUCT Static_Settings* p_in_static_settings, 
	global STRUCT Run_Settings* p_in_run_settings,
	global float* p_in_heightmap_data,
	global fvec4* p_in_iso_data, 
	global STRUCT Materials* p_in_material_data,
	global STRUCT ISO_Material* P_out_data,
	global fvec4* p_out_debug_data
)
{
	in_static_settings = p_in_static_settings;
	in_run_settings = p_in_run_settings;
	in_heightmap_data = p_in_heightmap_data;
	in_iso_data = p_in_iso_data;
	in_material_data = p_in_material_data;
	out_data = P_out_data;
	out_debug_data = p_out_debug_data;

	unify_fields_main(get_global_id(0));
}
