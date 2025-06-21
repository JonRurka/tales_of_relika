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


#include "samplers/Terrain_Sampler.inc"
//#include "samplers/Sphere_Sampler.inc"


#endif // _SAMPLER_INC_