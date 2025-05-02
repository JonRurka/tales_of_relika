#define USL_DIRECTX_UNITY3D 0
#define USL_VULKAN 0
#define USL_OPENCL 1
#include "../libUSL/USL.inc"

read_buffer_struct(Static_Settings, 0, in_static_settings)
readwrite_buffer(ivec4, 1, out_stitch_map_offsets)

ivec4 GetStitchOffset(uint grid_global_idx){
	return get_buffer_vector(out_stitch_map_offsets, grid_global_idx);
}

void SetStitchOffset(uint grid_global_idx, ivec4 offset_data){
	set_buffer_vector(out_stitch_map_offsets, grid_global_idx, offset_data);
}


void kernel main_cl(
	global STRUCT Static_Settings* p_in_static_settings, 
	global ivec4* p_out_stitch_map_offsets
)
{
	in_static_settings = p_in_static_settings;
	out_stitch_map_offsets = p_out_stitch_map_offsets;
	
	STRUCT Static_Settings static_settings = GET_STATIC_SETTINGS();
	uvec4 chunkSize = static_settings.ChunkSize;
	uint grid_size = (chunkSize.x * chunkSize.y);
	uint inst_index = idx;
	
	uint start = inst_index * grid_size;
	uint offset = 0;
	for (int i = 0; i < grid_size; ++i){
		uint g_index = start + i;
		ivec4 grid_entry = GetStitchOffset(g_index);
		grid_entry.x = offset;
		offset += grid_entry.y;
		SetStitchOffset(g_index, grid_entry);
	}
}

