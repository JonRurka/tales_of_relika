#ifndef _SAMPLER_DATA_FUNC_
#define _SAMPLER_DATA_FUNC_


float iso_sample_3D_4(float x, float y, float z, STRUCT Settings s){

    return Sample_3D(s, x, y, z);

    //float radius = 10;
    //return sphere_sample_4(radius, x, y, z);
}

float iso_sample_3D_2(fvec3 p, STRUCT Settings s){
    return iso_sample_3D_4(p.x, p.y, p.z, s);
}

fvec4 modification_sample_5(int x, int y, int z, int mod_idx, STRUCT Settings s)
{
	if (mod_idx == -1){
		return FVEC4(0, 0, 0, 0);
	}
	
	int size_x = INT(s.ChunkSizeX) + GRID_PADDING;
	int size_y = INT(s.ChunkSizeY) + GRID_PADDING;
	int size_z = INT(s.ChunkSizeZ) + GRID_PADDING;
	int col_size = size_x * size_y * size_z;
	int chunk_offset_start = mod_idx;
	int local_idx = INT(C_3D_to_1D(x + 0, y + 0, z + 0, size_x, size_y));
	int data_index = chunk_offset_start + local_idx;
	fvec4 val = get_buffer_vector(in_modification_data, data_index);
	
	if (val.x > 0.1){
		//Print_5("mod_sample: (%i, %i, %i) (%i): %f \n", x, y, z, local_idx, val.y);
		/*printf("voxel:(%i, %i, %i), size:(%i, %i), v_idx:%i, iso:%f \n",
		x, y, z,
		size_x, size_y,
		local_idx, val.y);*/
	}
	return val;
}

float height_sample_5(int x, int y, int column_idx, STRUCT Settings s, int n_index)
{
	int size_x = INT(s.ChunkSizeX) + HEIGHTMAP_PADDING;
	int size_z = INT(s.ChunkSizeZ) + HEIGHTMAP_PADDING;
	int col_size = size_x * size_z;
	int col_offset_start = column_idx * col_size;
	int local_idx = INT(C_2D_to_1D(x + HEIGHTMAP_OFFSET, y + HEIGHTMAP_OFFSET, size_z));
	int data_index = col_offset_start + local_idx;
	float val = 0;
	if (n_index > 3){
		val = get_buffer_scalar(in_heightmap_data, data_index);
	}
	else{
		val = get_buffer_vector(in_height_neighboor_data, data_index)[n_index];
	}
	
	return val;
}


STRUCT IsoSample iso_sample_4(STRUCT Settings s, ivec3 p, fvec3 world_pos, int n_index){
    float iso = iso_sample_3D_2(world_pos, s);
    //float height = height_sample_3(world_pos.x, world_pos.z, s);
	float height = height_sample_5(p.x, p.z, s.column_index, s, n_index);
	fvec4 mods = modification_sample_5(p.x, p.y, p.z, s.modification_index , s);

    return Combine_ISO(s, p, world_pos, iso, height, mods);
}

#endif // _SAMPLER_DATA_FUNC_