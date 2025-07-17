#ifndef _SAMPLER_INC_
#define _SAMPLER_INC_


struct Settings{
	int column_index;
	int modification_index;
	
    uint ChunkSizeX;
    uint ChunkSizeY;
    uint ChunkSizeZ;

    float VoxelsPerMeter;
    uint quality;
    int seed;
    bool enableCaves;
    float amp;
    float caveDensity;
    float grassOffset;
};

STRUCT Settings New_Settings(){
	STRUCT Settings s;
	
	s.column_index = 0;
	s.modification_index = -1;
	s.ChunkSizeX = 0;
	s.ChunkSizeY = 0;
	s.ChunkSizeZ = 0;
	s.VoxelsPerMeter = 0;
	s.quality = 0;
	s.seed = 0;
	s.enableCaves = false;
	s.amp = 0;
	s.caveDensity = 0;
	s.grassOffset = 0;
	
	return s;
}

struct IsoSample{
	float iso;
	int type_id;
};

struct IsoNormalSample{
	STRUCT IsoSample iso_type;
	fvec3 normal;
};


#include "samplers/Terrain_Sampler.inc"
//#include "samplers/Sphere_Sampler.inc"

STRUCT IsoSample Combine_ISO(STRUCT Settings s, ivec3 voxel_pos, fvec3 world_pos, float iso_3D, float height, fvec4 edit)
{
	
	if (edit.x > 0.1){
		//Print_4("changed ISO 1 (%i, %i, %i): %f \n", voxel_pos.x, voxel_pos.y, voxel_pos.z, edit.y);
	}
	
	bool change_iso = RountToInt(round(edit.x)) == 1;
	float edited_iso = edit.y;
	bool change_type = RountToInt(round(edit.z)) == 1;
	int edited_type = RountToInt(round(edit.w));
	
	
	float caveDensity = 5.0f;
	
	float iso_result = 0;
	int type_res = grass(); //grass
	
	float raw_height_iso = height - world_pos.y;
	float height_iso = clamp(raw_height_iso, -2.0f, 2.0f);
	//height_iso = iso_invert(height_iso);
	
	float raw_cave_iso = iso_3D;//;sphere_sample_5(10, FVEC3(5, 17, 0), world_pos.x, world_pos.y, world_pos.z);
	float cave_iso = clamp(raw_cave_iso, -5.0f, 5.0f);
	//cave_iso = iso_invert(cave_iso);
	
	if (raw_cave_iso > 0.0 && world_pos.y < (height - 0.0f)){
		iso_result = clamp(clamp(height_iso, -5.0f, 5.0f) - cave_iso, -5.0f, 5.0f);
	}
	else{
		iso_result = height_iso;
	}
	iso_result = height_iso;
	
	
	if (change_iso){
		float old_iso = iso_result;
		iso_result = iso_result + edited_iso;
		//printf("changed ISO (%i, %i, %i): %f - %f \n", voxel_pos.x, voxel_pos.y, voxel_pos.z, old_iso, iso_result);
	}
	
	
	if (world_pos.y < (height - 5)){
		type_res = stone(); // Stone
	}
	else if (world_pos.y < (height - 2)){
		type_res = dirt(); // dirt
	}
	
	if (edited_type){
		//iso_result = iso_result + edited_iso;
	}
	
	STRUCT IsoSample sample_res;
	sample_res.iso = iso_result;
	sample_res.type_id = type_res;
	return sample_res;

}


#endif // _SAMPLER_INC_