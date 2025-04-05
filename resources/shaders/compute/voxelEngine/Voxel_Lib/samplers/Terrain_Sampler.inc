
#define UPPER_GROUND_HEIGHT     0
#define SPHERE_Y_OFFSET         120
#define CRATER_BOTTOM           -55
#define CRATER_METER_DIAMETER   (15 * 32) // 480
#define CRATER_METER_RADIUS     (CRATER_METER_DIAMETER / 2) // 240
#define SAND_RING_START         (CRATER_METER_RADIUS - 100) // 150,  start is closer to center.
#define SAND_RING_END           (CRATER_METER_RADIUS - 60) // 175, end is further from center.
#define SAND_RING_DIFF          (SAND_RING_END - SAND_RING_START) // 25



float Perlin_sample_2D(float x, float y, float horizontal_scale, float frequency, float amp, int seed){
    STRUCT Module smooth_bumps = GetModule(x * horizontal_scale, 0, y * horizontal_scale);
    smooth_bumps.m_seed = seed;
    smooth_bumps.m_frequency = frequency;
    smooth_bumps.m_noiseQuality = QUALITY_STD;
    return Perlin_GetValue(smooth_bumps).Value * amp;
}

float sphere_sample(float radius, fvec3 center, float x, float y, float z){

    return radius - sqrt(pow(x - center.x, 2) + pow(y - center.y, 2) + pow(z - center.z, 2));
}

float crator_blend_signoid(float in_height, float start_depth){
    //return max(in_height, start_depth);

    float b_factor = 100;

    float sigmoid = 1.0 / (1.0 + exp(-b_factor * (in_height - start_depth)));

    return in_height * sigmoid + start_depth * (1 - sigmoid);
}

float crator_blend_lerp(float in_height, float start_depth, float end_depth){
    return max(in_height, end_depth);

    if (in_height > start_depth)
        return in_height;

    float diff = abs(start_depth - end_depth);

    in_height = clamp(in_height, start_depth, end_depth);

    float delta = abs(start_depth - in_height) / abs(diff);
    return LERP(start_depth, end_depth, delta);
}

float crater_heightmap_sample(float x, float y, float R, float offset, float default_height, float bottom) {
    float height = default_height;
    if (x * x + y * y <= (R * R)) {
        height = -sqrt(R * R - x * x - y * y) + offset;
    }

    float bottom_start = bottom + 5;
    if (height < bottom_start){
        height = crator_blend_lerp(height, bottom_start, bottom);
    }

    return height;
}

float CaveSample(STRUCT Settings s, float x, float y, float z, float scale, float height, float power){
    STRUCT Module cave_module = GetModule(x / scale, y / scale, z / scale);
    cave_module.m_seed = s.seed;
    cave_module.m_frequency = 0.15;
    cave_module.m_noiseQuality = s.quality;


    float rValue = 0;

    cave_module = RidgedMulti_GetValue(cave_module);
    //cave_module = Perlin_GetValue(cave_module);

    rValue = cave_module.Value;
    rValue *= height;

    if (power != 0){
        rValue = pow(rValue, power);
    }

    return rValue;
}

float parabla_delta_1(float x){
    x = clamp(x, 0, 1);
    float A = -4;
    float B = 4;
    return (A * pow(x, 2)) + (B * x);
}

float GetSandStart(float x, float y){
    fvec2 dir = normalize(FVEC2(x, y));
    return SAND_RING_START + Perlin_sample_2D(dir.x, dir.y, 10, 0.9, 6, 6843);
}

float GetSandEnd(float x, float y){
    fvec2 dir = normalize(FVEC2(x, y));
    return SAND_RING_END + Perlin_sample_2D(dir.x, dir.y, 10, 0.9, 6, 3956);
}

/*bool isInSandBumpRegion(float x, float y){
    // TODO: Add noise.

    

    float center_dist = distance(FVEC2(0, 0), FVEC2(x, y));
    float dir = normalize(center_dist);
    float n1 = Perlin_sample_2D(dir.x, dir.y, 10, 0.9, 10, 6843);
    float n2 = Perlin_sample_2D(dir.x, dir.y, 10, 0.9, 10, 3956);
    return (center_dist > (SAND_RING_START + n1) && 
            center_dist < (SAND_RING_END + n2));
}*/

bool isInSandBumpRegion(float x, float y, float noise_start, float noise_end){
    
    float center_dist = distance(FVEC2(0, 0), FVEC2(x, y));
    float n1 = noise_start;
    float n2 = noise_end;
    return (center_dist > (n1) && 
            center_dist < (n2));
}

float CraterSandRing(STRUCT Settings s, float x, float y, float n1, float n2){
    float amp = 4;
    //return -100;

    if (!isInSandBumpRegion(x, y, n1, n2)){
        return 0;
    }

    float center_dist = distance(FVEC2(0, 0), FVEC2(x, y));

    float horizontal_scale = 0.04;
    STRUCT Module smooth_bumps = GetModule(x * horizontal_scale, 0, y * horizontal_scale);
    smooth_bumps.m_seed = 3095;
    smooth_bumps.m_frequency = 0.8;
    smooth_bumps.m_noiseQuality = s.quality;

    smooth_bumps = Perlin_GetValue(smooth_bumps);

    float diff = abs(n2 - n1);
    float lin_delta = abs(center_dist - (n1)) / diff;
    float parab_delta = parabla_delta_1(lin_delta) * 0.75; // modulates delta so that the 0.5 is max value, and 0, 1 is 0;

    float blend_variation = 0;

    blend_variation = abs(Scale(smooth_bumps.Value, -1, 1, 0, 1) * (amp));
    
    //return parab_delta * 10;
    //return 100;
    //return center_dist - SAND_RING_START;
    //return lin_delta * 10;
    return blend_variation * parab_delta;
}

float CraterTerrainVariation(STRUCT Settings s, float x, float y){
    //return 0;

    float horizontal_scale = 0.03;
    STRUCT Module smooth_bumps = GetModule(x * horizontal_scale, 0, y * horizontal_scale);
    smooth_bumps.m_seed = s.seed;
    smooth_bumps.m_frequency = 0.5;
    smooth_bumps.m_noiseQuality = s.quality;

    smooth_bumps = Perlin_GetValue(smooth_bumps);

    float blend_variation = Scale(smooth_bumps.Value, -1, 1, 0, 1);

    return blend_variation;
}

float MountainTerrainSample(STRUCT Settings s, float x, float y)
{
    //return x/3;
    return 0;

    STRUCT Module perlin_mountains = GetModule(x, 0, y);
    perlin_mountains.m_seed = s.seed;
    perlin_mountains.m_frequency = 0.5;
    perlin_mountains.m_noiseQuality = s.quality;

    /*Module mountains = GetModule(x, 0, y);
    mountains.m_seed = s.seed;
    mountains.m_frequency = 0.5;
    mountains.m_lacunarity = 2;
    mountains.m_noiseQuality = s.quality;*/

    perlin_mountains = Perlin_GetValue(perlin_mountains);

    float blendMountains = perlin_mountains.Value;// +mountains.Value;

    return blendMountains;
}

float MountainTerrainExample(STRUCT Settings s, float x, float y){
    float horizontal_scale = 0.03;
    float amp = 20;
    float offset = 2;
    return MountainTerrainSample(s, (x * (horizontal_scale)), (y * (horizontal_scale))) * amp + offset;
}


float CraterTerrain(STRUCT Settings s, float x, float y){

    float upper_ground_height = UPPER_GROUND_HEIGHT;
    float sphere_mid_offset = SPHERE_Y_OFFSET;
    float crater_bottom = CRATER_BOTTOM;

    //float crater_chunk_size = 8;
    float crater_meter_size = CRATER_METER_DIAMETER;

    float sphere_iso = min(crater_heightmap_sample(x, y, crater_meter_size / 2.0, sphere_mid_offset, upper_ground_height, crater_bottom), upper_ground_height);



    return sphere_iso;
}

float GetHeight(STRUCT Settings s, float x, float y)
{
    //return 0;

    float base_crater = CraterTerrain(s, x, y);

    float horizontal_scale = 0.03;
    float amp = 4.0;
    float base_variation = (CraterTerrainVariation(s, x, y) * amp) - amp / 2;

    float result = 0;

    float n1 = GetSandStart(x, y);
    float n2 = GetSandEnd(x, y);

    float sand_ring_variation = 0;
    if (isInSandBumpRegion(x, y, n1, n2)){
        fvec2 dir = normalize(FVEC2(x, y));
        fvec2 interp_start = dir * n1;
        fvec2 interp_end = dir * n2;
        float center_dist = distance(FVEC2(0, 0), FVEC2(x, y));
        float h1 = CraterTerrain(s, interp_start.x, interp_start.y);
        float h2 = CraterTerrain(s, interp_end.x, interp_end.y);
        float lin_delta = (center_dist - n1) / (n2 - n1);
        float lin_height = LERP(h1, h2, lin_delta);
        
        sand_ring_variation = lin_height + CraterSandRing(s, x, y, n1, n2);

        result = sand_ring_variation + base_variation;
    }
    else {
        result = base_crater + base_variation + sand_ring_variation;
    }

    return result;


}


float Sample_2D(STRUCT Settings s, float x, float y)
{
    return GetHeight(s, x, y);
}

float Sample_3D(STRUCT Settings s, float x, float y, float z)
{
    //return CaveSample(s, x, y, z, 14.0, 10, 1);
	
    float radius = (s.ChunkSizeX / 2) - 2;
    fvec3 sphere_center = FVEC3((s.ChunkSizeX / 2) - 2, (s.ChunkSizeY / 2) - 2, (s.ChunkSizeZ / 2) - 2);
    float val = sphere_sample(radius, sphere_center, x, y, z);
    return val;
}

