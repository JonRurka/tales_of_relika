
#undef USE_HEIGHTMAP
#undef USE_MATERIALS
#undef GEN_CRATER_MAP

#define USE_HEIGHTMAP 0
#define USE_MATERIALS 0
#define GEN_CRATER_MAP 0

float sphere_sample_5(float radius, fvec3 center, float x, float y, float z){

    return radius - sqrt(pow(x - center.x, 2) + pow(y - center.y, 2) + pow(z - center.z, 2));
}

float Sample_2D(STRUCT Settings s, float x, float y)
{
    return 5;
}

float Sample_3D(STRUCT Settings s, float x, float y, float z)
{
	//float height = 3;
	//return height - y;
	
	/*if (y > 3)
	{
		return -1;
	}
	else{
		return 1;
	}*/
	
    //float radius = (s.ChunkSizeX / 2) - 2;
	float radius = 15;
    //fvec3 sphere_center = FVEC3((s.ChunkSizeX / 2) - 2, (s.ChunkSizeY / 2) - 2, (s.ChunkSizeZ / 2) - 2);
	fvec3 sphere_center = FVEC3(16, 16, 16);
    float val = sphere_sample_5(radius, sphere_center, x, y, z);
    return val;
}
