#define USL_DIRECTX_UNITY3D 0
#define USL_VULKAN 0
#define USL_OPENCL 1
#include "../libUSL/USL.inc"
#include "Voxel_Lib/unify_fields_lib.inc"

void kernel main_cl(
	global const STRUCT Static_Settings* p_in_static_settings, 
	global const STRUCT Run_Settings* p_in_run_settings,
	
	
	
	
)
{
	unify_fields_main(get_global_id(0));
}
