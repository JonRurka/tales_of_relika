#ifndef _SMOOTHRENDER_MARK_INC_
#define _SMOOTHRENDER_MARK_INC_

#include "voxel_lib_core.inc"

read_buffer_struct(Static_Settings, 0, in_static_settings)
read_buffer(ivec4, 1, in_run_settings)

read_buffer_int(2, in_counts_data)	

readwrite_buffer_int(3, out_stitch_map_data)
readwrite_buffer(ivec4, 4, out_stitch_map_offsets)
readwrite_buffer(ivec4, 5, out_counts_data)

readwrite_buffer(fvec4, 6, out_debug_data)


void SetStitchMap(uint voxel_index, int poly_buffer_index){
	set_buffer_scalar(out_stitch_map_data, voxel_index, poly_buffer_index);
}

void SetStitchOffset(uint grid_global_idx, ivec4 offset_data){
	set_buffer_vector(out_stitch_map_offsets, grid_global_idx, offset_data);
}

int GetCount(uint index){
	return get_buffer_scalar(in_counts_data, index);
}

void Set_Debug_Data(uint index, fvec4 data){
	set_buffer_vector(out_debug_data, index, data);
}

void CreateStitchMap(uint batch_index, uint batchesPerGroup, uint fullChunkSize){
	
    uint batch_offset = (batch_index) * fullChunkSize;

    int buffer_index = 0;
	uint v_index = 0;
    for (v_index = 0; v_index < fullChunkSize; ++v_index){
        
        int poly_count = GetCount(batch_offset + v_index);

        if (poly_count == 0){
            continue;
        }

        SetStitchMap(batch_offset + v_index, buffer_index);
        
        buffer_index += poly_count;
    }

	set_buffer_vector(out_counts_data, batch_index, IVEC4(buffer_index, 0, batch_index, 0));
	//Set_Debug_Data(batch_index, FVEC4(buffer_index, INT(v_index), 1, 1));
}

void CreateStitchMapAsync(uint idx, uint grid_size, int batch_index, uint row_index, ivec2 row_coords, uvec4 chunkSize, uint fullChunkSize)
{
	uint batch_offset = UINT(batch_index) * fullChunkSize;
	int buffer_index = 0;
	uint x = 0;
	int row_y = row_coords.x;
	int row_z = row_coords.y;
	for (x = 0; x < chunkSize.x; ++x){
		uint v_index = C_3D_to_1D(INT(x), row_y, row_z, chunkSize.x, chunkSize.y);
		int poly_count = GetCount(batch_offset + v_index);
		if (poly_count > 0){
			SetStitchMap(batch_offset + v_index, buffer_index);
			buffer_index = buffer_index + poly_count;
		}
	}
	if (buffer_index != 0){
		//Print_4("%i: Out Offset (y: %i, z: %i): %i\n", row_index, row_y, row_z, buffer_index);
	}
	SetStitchOffset(idx, IVEC4(0, buffer_index, 0, 0));
}


void smoothrender_mark_main(uint batch_index){
	//Print("old mark\n");

    STRUCT Static_Settings static_settings = GET_STATIC_SETTINGS();

    // TODO: disable with run settings if location.w is -1.

    CreateStitchMap(batch_index, static_settings.batches[0], UINT(static_settings.FullChunkSize[0]));
	
	//if (batch_index == 0)
	//	Set_Debug_Data(batch_index, FVEC4(1, 1, 1, 1));
}

void smoothrender_mark_async_main(uint idx){
	
    STRUCT Static_Settings static_settings = GET_STATIC_SETTINGS();
	uvec4 chunkSize = static_settings.ChunkSize;
	
	uint grid_size = (chunkSize.x * chunkSize.y);
	
	uint inst_index = idx / grid_size;
	//STRUCT Run_Settings run_settings = GET_RUN_SETTINGS(0);
	ivec4 loc = RUN_SETTINGS_LOCATION(0); //run_settings.Location;
	int batch_index = loc.w;
	uint row_index = idx % grid_size;
	ivec2 row_coords = C_1D_to_2D(row_index, chunkSize.y);
	
	CreateStitchMapAsync(idx, grid_size, batch_index, row_index, row_coords, chunkSize, static_settings.FullChunkSize[0]);
}

#endif // _SMOOTHRENDER_MARK_INC_