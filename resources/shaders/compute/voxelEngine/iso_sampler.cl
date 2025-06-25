#define USL_DIRECTX_UNITY3D 0
#define USL_VULKAN 0
#define USL_OPENCL 1
#define RUNTIME_INSERTED_TYPES
#include "../libUSL/USL.inc"
#include "Voxel_Lib/iso_sample_lib.inc"

void kernel main_cl(
	global STRUCT Static_Settings* p_in_static_settings, 
	
	global STRUCT Voxel_Location* p_in_voxel_locations,
	
	global float* p_in_heightmap_data,
	global fvec4* p_in_height_neighboor_data,
	global fvec4* p_in_modification_data,
	
	global fvec4* p_out_iso_data
)
{
	in_static_settings = p_in_static_settings;
	in_voxel_locations = p_in_voxel_locations;
	in_heightmap_data = p_in_heightmap_data;
	in_height_neighboor_data = p_in_height_neighboor_data;
	in_modification_data = p_in_modification_data;
	out_iso_data = p_out_iso_data;
	
	iso_sample_main(get_global_id(0));
}



