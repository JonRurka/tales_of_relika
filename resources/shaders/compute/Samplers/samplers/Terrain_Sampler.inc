

float cubic_exp(float val){
	float a = -2.71828f;
	float b = 2.52009f;
	float c = 0;
	float d = 3.15069;
	float e = 2.71828182846f;
	return (a * pow(e, val)) + (b * (val * val * val)) + (d * val) + e;
}

float CaveSample(STRUCT Settings s, float x, float y, float z, float scale, float height, float power){
    STRUCT Module cave_module = GetModule_3(x / scale, y / scale, z / scale);
    cave_module.m_seed = s.seed;
    cave_module.m_frequency = 0.15;
    cave_module.m_noiseQuality = INT(s.quality);


    float rValue = 0;

    cave_module = RidgedMulti_GetValue(cave_module);
    //cave_module = Perlin_GetValue(cave_module);

    rValue = cubic_exp(cave_module.Value);
    rValue *= height;

    if (power != 0){
        rValue = pow(rValue, power);
    }

    return rValue;
}

float GetHeight(STRUCT Settings s, float x, float y)
{
	//return x / 5.0;
	//return 5;
	
	x /= 100.0f;
	y /= 100.0f;
	
	// select biome
	STRUCT Module perlin_select = GetModule_3(x, 0, y); 
	perlin_select.m_seed = s.seed + 1234;
	perlin_select.m_frequency = 0.1;
	perlin_select.m_noiseQuality = INT(s.quality);
	perlin_select.m_persistence = 0.25;
	perlin_select = Perlin_GetValue(perlin_select);
	perlin_select.m_edgeFalloff = 0.125;
	perlin_select.m_lowerBound = 0.0;
	perlin_select.m_upperBound = 1000.0;
	
	
	// Plains
	STRUCT Module perlin_planes = GetModule_3(x, 0, y);
	perlin_planes.m_seed = s.seed;
	perlin_planes.m_frequency = 0.1;
	perlin_planes.m_noiseQuality = INT(s.quality);
	perlin_planes = Perlin_GetValue(perlin_planes);
	perlin_planes.Value = perlin_planes.Value * 40.0f;



	// mountains
	STRUCT Module perlin_mountains = GetModule_3(x, 0, y);
	perlin_mountains.m_seed = s.seed + 4321;
	perlin_mountains.m_frequency = 0.3;
	perlin_mountains.m_noiseQuality = INT(s.quality);
	perlin_mountains = RidgedMulti_GetValue(perlin_mountains);
	perlin_mountains.Value = perlin_mountains.Value * 40.0f;
	
	STRUCT Module rigid_mountains = GetModule_3(x, 0, y);
	rigid_mountains.m_seed = s.seed;
	rigid_mountains.m_frequency = 0.2;
	rigid_mountains.m_noiseQuality = INT(s.quality);
	rigid_mountains = RidgedMulti_GetValue(rigid_mountains);
	rigid_mountains.Value = perlin_mountains.Value + (rigid_mountains.Value * 10.0f);

	


	/*Module mountains = GetModule_3(x, 0, y);
	mountains.m_seed = s.seed;
	mountains.m_frequency = 0.5;
	mountains.m_lacunarity = 2;
	mountains.m_noiseQuality = s.quality;*/

	

	float blendMountains = Select_GetValue(rigid_mountains, perlin_planes, perlin_select);
	
	/*
	if (select_val > 0.0f){
		blendMountains = planes_val;
	}
	else{
		blendMountains = mountain_val;
	}*/

	return blendMountains;
}

float Sample_2D(STRUCT Settings s, float x, float y)
{
	return GetHeight(s, x, y);
}

float Sample_3D(STRUCT Settings s, float x, float y, float z)
{
	return CaveSample(s, x, y, z, 14.0f, 5.0f, 1.0f);
}




