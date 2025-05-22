#include "SmoothVoxelBuilder.h"

#include "marchingcubes_arrays.h"

#include "Logger.h"
#include "Utilities.h"

#include <chrono>

using namespace VoxelEngine;

#define USE_CACHE false

#define USE_HEIGHTMAP_CACHE false

#define COMPUTE_TRIANGES true

#define VECTOR4_SIZE (sizeof(float) * 4)

#define VK_EXT ".comp"
#define CL_EXT ".cl"

#if VOXEL_RUNTIME == VOXEL_RUNTIME_OPENCL
#define EXT CL_EXT
#elif VOXEL_RUNTIME == VOXEL_RUNTIME_VULKAN
#define EXT VK_EXT
#endif


#define BASE_RESOURCE_DIR "compute::voxelEngine::"

glm::fvec4 DirectionOffsets_f(int index) {
    return (glm::fvec4)MarchingCubesArrays::directionOffsets[index];
}

glm::ivec4 DirectionOffsets_i(int index) {
    return MarchingCubesArrays::directionOffsets[index];
}

int Hash_3D(int x, int y, int z) {
    return x ^ y << 2 ^ z >> 2;
}

int Hash_2D(int x, int y) {
    return Hash_3D(x, y, 0);
}

/*#define MAX_HEIGHTMAPS_X 10000000
int C_2D_To_1D(int x, int y) {
    return y * MAX_HEIGHTMAPS_X + x;
}*/

int* get_null() {
    return nullptr;
}

void All_Zero(glm::vec4* data, int num, std::string marker) {
    bool all_zero = true;
    for (int i = 0; i < num; i++) {
        if (data[i].x != 0 ||
            data[i].y != 0 ||
            data[i].z != 0) {
            all_zero = false;
        }
    }

    if (all_zero) {
        //printf("%s: All zero!!\n", marker.c_str());
    }
}

int GetBatchNumIndex(SmoothVoxelBuilder::Run_Settings* group_start, int numGroups, glm::ivec3 search) {
    for (int i = 0; i < numGroups; i++) {
        if (search.x == group_start[i].Location.x &&
            search.y == group_start[i].Location.y &&
            search.z == group_start[i].Location.z) {
            
            return i;
        }
        //printf("(%i, %i, %i)\n",
        //    group_start[i].Location.x, group_start[i].Location.y, group_start[i].Location.z);
    }
    return -1;
}

void SmoothVoxelBuilder::Init(ChunkSettings* settings)
{
    //printf("break\n");
    //int* test = get_null();
    //int test1 = *test;
    //printf("%i\n", test1);
    Settings p_settings = *settings->GetSettings();

    float vpm = p_settings.getFloat("voxelsPerMeter");

    m_static_settings.VoxelsPerMeter = glm::vec4(vpm, vpm, vpm, vpm);
    m_static_settings.ChunkMeterSize.x = p_settings.getInt("chunkMeterSizeX");
    m_static_settings.ChunkMeterSize.y = p_settings.getInt("chunkMeterSizeY");
    m_static_settings.ChunkMeterSize.z = p_settings.getInt("chunkMeterSizeZ");

    m_WorkGroups = p_settings.getInt("WorkGroups");

    m_numBatchGroups = p_settings.getInt("TotalBatchGroups");
    m_numBatchesPerGroup = p_settings.getInt("BatchesPerGroup");
    m_totalBatches = m_numBatchGroups * m_numBatchesPerGroup;

    m_numColumns = p_settings.getInt("MaxColumns");
    m_activeColumIndex.resize(m_numColumns);

    m_run_settings = new Run_Settings[m_totalBatches + 1];

    m_shaderDir = p_settings.getString("programDir");

    m_invert_tris = p_settings.getInt("InvertTrianges") >= 1 ? true : false;
    
    CalculateVariables();

    InitializeComputePrograms();
    CreateComputeBuffers();
    FinalizePrograms();
}

glm::vec4* verts_tmp;
glm::vec4* norm_tmp;
int* tris_tmp;

void SmoothVoxelBuilder::SetRunSettings(std::vector<glm::ivec3> locations) {
    m_run_settings[0].Location = glm::ivec4(0); // first element is metadata
    int stop = std::min(m_totalBatches, (int)locations.size());
    for (int i = 0; i < std::min(m_totalBatches, (int)locations.size()); i++) {
        m_run_settings[i + 1].Location = glm::ivec4(locations[i].x, locations[i].y, locations[i].z, i);
    }
}

glm::dvec4 SmoothVoxelBuilder::Render(ChunkRenderOptions* options)
{
    //int cx = options.X;
    //int cy = options.Y;
    //int cz = options.Z;

    /*int xStart = cx * m_static_settings.ChunkSizeX;
    int xEnd = cx * m_static_settings.ChunkSizeX + m_static_settings.ChunkSizeX;

    int yStart = cy * m_static_settings.ChunkSizeY;
    int yEnd = cy * m_static_settings.ChunkSizeY + m_static_settings.ChunkSizeY;

    int zStart = cz * m_static_settings.ChunkSize.z;
    int zEnd = cz * m_static_settings.ChunkSize.z + m_static_settings.ChunkSize.z;*/
    
    /*m_run_settings.X = cx;
    m_run_settings.Y = cy;
    m_run_settings.Z = cz;*/

    //SetRunSettings(options->locations);

    //m_run_settings.Location = glm::ivec4(cx, cy, cz, 0);

    //verts_tmp = data->Vertex;
    //norm_tmp = data->Normal;
    //tris_tmp = data->Triangles;

    glm::dvec4 times = DoRender();

    extract_cached = false;

    //Extract(data, counts);
    return times;
}

glm::dvec4 SmoothVoxelBuilder::Generate(ChunkGenerationOptions* options)
{
    /*
    float Data[DATA_SIZE] = { 0 };
    for (int i = 0; i < DATA_SIZE; i++)
        Data[i] = i + 1;

    m_in_Buffer->SetData(Data);

    m_program_compute.Execute(DATA_SIZE, 0, 0);

    m_out_Buffer->GetData(Data);

    for (int i = 0; i < DATA_SIZE; i++)
    {
        printf("res '%i': %f\n", i, Data[i]);
    }
    */

    SetRunSettings(options->locations);



    glm::dvec4 result = glm::dvec4(0,0,0,0);
    double buffer_writes_time = 0;

    auto start_buffer_writes = std::chrono::high_resolution_clock::now();
    int start_index = 0;//(i * m_numBatchesPerGroup);
    Run_Settings* start = m_run_settings + start_index;
    m_in_run_settings_buffer->SetData(start, (m_totalBatches + 1) * sizeof(Run_Settings));
    auto end_buffer_writes = std::chrono::high_resolution_clock::now();
    buffer_writes_time += std::chrono::duration<double>(end_buffer_writes - start_buffer_writes).count();



    for (int i = 0; i < m_numBatchGroups; i++) {

        //auto start_buffer_writes = std::chrono::high_resolution_clock::now();
        //int start_index = (i * m_numBatchesPerGroup);
        //Run_Settings* start = m_run_settings + start_index;
        //m_in_run_settings_buffer->SetData(start, m_numBatchesPerGroup * sizeof(Run_Settings));
        //auto end_buffer_writes = std::chrono::high_resolution_clock::now();
        //buffer_writes_time += std::chrono::duration<double>(end_buffer_writes - start_buffer_writes).count();




        //auto start_t = std::chrono::high_resolution_clock::now();
        //auto end_t = std::chrono::high_resolution_clock::now();
        //auto duration = std::chrono::duration<double>(end_t - start_t).count();
        //result.w += duration;
        //for (int j = 0; j < m_numBatchesPerGroup; j++) {
        //    printf("Running Batch %i (%i)\n", start[j].Location.w, start_index);
        //}

        //GenerateHeightmap(i, start);
        //result.x += GenerateISOField(i, start);
        //GenerateMaterialField(i, start);
        result.x += AssembleUnifiedField(i, start);
        result.y += Construct(i, start);
    }
    result.z = buffer_writes_time;

    return result;
}

std::vector<glm::ivec4> SmoothVoxelBuilder::GetSize()
{
    counts_cache.resize(m_totalBatches);

    m_out_counts_buffer->GetData(counts_cache.data());

    int running_total = 0;
    for (int i = 0; i < counts_cache.size(); i++) {
        counts_cache[i].y = running_total;
        running_total += counts_cache[i].x;
    }

    return counts_cache;
}

void SmoothVoxelBuilder::ReleaseHeightmap(int x, int z)
{

    int index = Hash_2D(x, z);

    if (m_heightmap_cache.count(index) <= 0) {
        return;
    }

    m_unused_heightmaps.push(m_heightmap_cache[index]);
    m_heightmap_cache.erase(index);
}

void SmoothVoxelBuilder::SetHeightmapsEnabled(bool enabled)
{
}

IComputeBuffer* SmoothVoxelBuilder::Get_Tranfer_Buffer(int size, int stride, bool external)
{
    IComputeBuffer* res = m_controller->NewReadWriteBuffer(size, stride, external);
    return res;
}

IComputeController* SmoothVoxelBuilder::Get_Compute_Controller()
{
    return m_controller;
}

void SmoothVoxelBuilder::Dispose()
{
    ComputeInterface::DisposePlatform(ComputeInterface::VULKAN);
}

void SmoothVoxelBuilder::CalculateVariables()
{
    m_static_settings.ChunkSize.x = (int)(m_static_settings.ChunkMeterSize.x * m_static_settings.VoxelsPerMeter.x);
    m_static_settings.ChunkSize.y = (int)(m_static_settings.ChunkMeterSize.y * m_static_settings.VoxelsPerMeter.y);
    m_static_settings.ChunkSize.z = (int)(m_static_settings.ChunkMeterSize.z * m_static_settings.VoxelsPerMeter.z);
    m_static_settings.FullChunkSize[0] = m_static_settings.ChunkSize.x * m_static_settings.ChunkSize.y * m_static_settings.ChunkSize.z;
    m_static_settings.FullChunkSize[1] = (m_static_settings.ChunkSize.x + 1) * (m_static_settings.ChunkSize.y + 1) * (m_static_settings.ChunkSize.z + 1);
    m_static_settings.half_[0] = ((1.0f / (float)m_static_settings.VoxelsPerMeter.x) / 2.0f);
    m_static_settings.SideLength.x = m_static_settings.ChunkMeterSize.x / (float)m_static_settings.ChunkSize.x;
    m_static_settings.SideLength.y = m_static_settings.ChunkMeterSize.y / (float)m_static_settings.ChunkSize.y;
    m_static_settings.SideLength.z = m_static_settings.ChunkMeterSize.z / (float)m_static_settings.ChunkSize.z;

    m_static_settings.skipDist[0] = 1 / (float)m_static_settings.VoxelsPerMeter.x;
    //skipDist = Math.RoundToInt(1 / (float)VoxelsPerMeter);
    
    locOffset[0] = glm::fvec4(0, 0, m_static_settings.SideLength.z, 0);
    locOffset[1] = glm::fvec4(m_static_settings.SideLength.x, 0, m_static_settings.SideLength.z, 0);
    locOffset[2] = glm::fvec4(m_static_settings.SideLength.x, 0, 0, 0);
    locOffset[3] = glm::fvec4(0, 0, 0, 0);
    locOffset[4] = glm::fvec4(0, m_static_settings.SideLength.y, m_static_settings.SideLength.z, 0);
    locOffset[5] = glm::fvec4(m_static_settings.SideLength.x, m_static_settings.SideLength.y, m_static_settings.SideLength.z, 0);
    locOffset[6] = glm::fvec4(m_static_settings.SideLength.x, m_static_settings.SideLength.y, 0, 0);
    locOffset[7] = glm::fvec4(0, m_static_settings.SideLength.y, 0, 0);

    // Could technically be configured to all different voxels per meter in each axis.
    globalOffsets[0] = DirectionOffsets_f(0) * m_static_settings.skipDist[0];
    globalOffsets[1] = DirectionOffsets_f(1) * m_static_settings.skipDist[0];
    globalOffsets[2] = DirectionOffsets_f(2) * m_static_settings.skipDist[0];
    globalOffsets[3] = DirectionOffsets_f(3) * m_static_settings.skipDist[0];
    globalOffsets[4] = DirectionOffsets_f(4) * m_static_settings.skipDist[0];
    globalOffsets[5] = DirectionOffsets_f(5) * m_static_settings.skipDist[0];
    globalOffsets[6] = DirectionOffsets_f(6) * m_static_settings.skipDist[0];
    globalOffsets[7] = DirectionOffsets_f(7) * m_static_settings.skipDist[0];
    
    m_static_settings.batches = glm::ivec4(m_numBatchesPerGroup, m_numBatchGroups, m_totalBatches, 0);
}

void SmoothVoxelBuilder::InitializeComputePrograms()
{
    IComputeProgram::FileType type;
    if (m_voxel_runtime == VOXEL_RUNTIME_OPENCL) {
        type = IComputeProgram::FileType::Text_Data;

        m_device_cl = Utilities::Get_Recommended_Device();
        Logger::LogDebug(LOG_POS("InitializeComputePrograms"), "Using OpenCL Compute Device: %s", m_device_cl.name);

        m_controllerInfo.device = &m_device_cl;
        m_controllerInfo.platform = m_device_cl.platform;
        m_controllerInfo.SetProgramDir("");

        m_controller = ComputeInterface::GetComputeController(ComputeInterface::OpenCL, m_controllerInfo);
    }
    else if (m_voxel_runtime == VOXEL_RUNTIME_VULKAN) {
        type = IComputeProgram::FileType::Binary_Data;

        // TODO: Smarter logic for getting device.
        std::vector<Vulkan_Device_Info> devices = ComputeInterface::GetSupportedDevices_Vulkan();
        m_device_vk = devices[1];
        Logger::LogDebug(LOG_POS("InitializeComputePrograms"), "Using Vulkan Compute Device: %s", m_device_vk.Name);

        printf("Creating programs...\n");
        printf(m_shaderDir.c_str());

        m_controllerInfo.device = &m_device_vk;
        m_controllerInfo.SetProgramDir("");
        //m_controllerInfo.SetProgramDir("C:/Users/Jon/Source/cpp_libs/VoxelEngine/shaders/Vulkan/test");

        m_controller = ComputeInterface::GetComputeController(ComputeInterface::VULKAN, m_controllerInfo);
    }
    else {
        Logger::LogFatal(LOG_POS("InitializeComputePrograms"), "Invalid voxel runtime");
        return;
    }

    // program 1
    
    //m_program_compute = new VoxelComputeProgram(m_controller, PROGRAM);

    m_program_heightmap = new VoxelComputeProgram(m_controller, BASE_RESOURCE_DIR + PROGRAM_HEIGHTMAP + EXT, m_WorkGroups, type);
    m_program_iso_field = new VoxelComputeProgram(m_controller, BASE_RESOURCE_DIR + PROGRAM_ISO_FIELD + EXT, m_WorkGroups, type);
    //m_program_material_field = new VoxelComputeProgram(m_controller, PROGRAM_MATERIAL_FIELD, m_WorkGroups);
    m_program_unify_fields = new VoxelComputeProgram(m_controller, BASE_RESOURCE_DIR + PROGRAM_UNIFY_FIELDS + EXT, m_WorkGroups, type);

    //m_program_smoothrender_createvertlist = new VoxelComputeProgram(m_controller, PROGRAM_SMOOTH_RENDER_CREATE_VERTLIST, m_WorkGroups);
    //m_program_smoothrender_createmesh = new VoxelComputeProgram(m_controller, PROGRAM_SMOOTH_RENDER_CREATE_MESH, m_WorkGroups);


    m_program_smoothrender_construct = new VoxelComputeProgram(m_controller, BASE_RESOURCE_DIR + PROGRAM_SMOOTH_RENDER_CONSTRUCT + EXT, m_WorkGroups, type);
    m_program_smoothrender_mark = new VoxelComputeProgram(m_controller, BASE_RESOURCE_DIR + PROGRAM_SMOOTH_RENDER_MARK + EXT, m_WorkGroups, type);
    m_program_smoothrender_mark_offsets = new VoxelComputeProgram(m_controller, BASE_RESOURCE_DIR + PROGRAM_SMOOTH_RENDER_MARK_OFFSETS + EXT, m_WorkGroups, type);
    //m_program_smoothrender_stitch = new VoxelComputeProgram(m_controller, PROGRAM_SMOOTH_RENDER_STITCH, 1);
    m_program_smoothrender_stitch_async = new VoxelComputeProgram(m_controller, BASE_RESOURCE_DIR + PROGRAM_SMOOTH_RENDER_STITCH_ASYNC + EXT, m_WorkGroups, type);
}

void SmoothVoxelBuilder::CreateComputeBuffers()
{

    // Create buffers.

    //m_in_Buffer = m_controller->NewReadBuffer(DATA_SIZE, sizeof(float));
    //m_out_Buffer = m_controller->NewWriteBuffer(DATA_SIZE, sizeof(float));

    m_in_static_settings_buffer = m_controller->NewReadBuffer(1, sizeof(Static_Settings));
    m_in_run_settings_buffer = m_controller->NewReadWriteBuffer(m_totalBatches + 1, sizeof(Run_Settings));

    in_locOffset_buffer = m_controller->NewReadBuffer(8, sizeof(glm::fvec4));
    in_globalOffsets_buffer = m_controller->NewReadBuffer(8, sizeof(glm::fvec4));

    in_directionOffsets_buffer = m_controller->NewReadWriteBuffer(8, sizeof(glm::ivec4));

    in_edgeTable_Buffer = m_controller->NewReadBuffer(256, sizeof(int));
    in_TriTable_Buffer = m_controller->NewReadBuffer(4096, sizeof(int));

    // 32768
    int expanded_chunk_size_1 = (m_static_settings.ChunkSize.x + 1) * (m_static_settings.ChunkSize.y + 1) * (m_static_settings.ChunkSize.z + 1);
    int expanded_chunk_size_2 = (m_static_settings.ChunkSize.x + 2) * (m_static_settings.ChunkSize.y + 2) * (m_static_settings.ChunkSize.z + 2);
    int expanded_heightmap_size = (m_static_settings.ChunkSize.x + 2) * (m_static_settings.ChunkSize.y + 2);
    m_heightmap_data_buffer = m_controller->NewReadWriteBuffer(expanded_heightmap_size * m_numBatchesPerGroup, sizeof(float)); // 4356 bytes / chunk
    m_iso_field_buffer = m_controller->NewReadWriteBuffer(expanded_chunk_size_2 * m_numBatchesPerGroup, sizeof(glm::fvec4)); // 574,992 bytes / chunk
    m_material_buffer = m_controller->NewReadWriteBuffer(expanded_chunk_size_2 * m_numBatchesPerGroup, sizeof(glm::fvec4) * 2); // 1,048,576 bytes / chunk
    m_iso_mat_buffer = m_controller->NewReadWriteBuffer(expanded_chunk_size_1 * m_numBatchesPerGroup, sizeof(ISO_Material)); // 1,572,864 bytes / chunk
    // size of above: 3,200,788 bytes / chunk


    //m_vertList_buffer = m_controller->NewReadWriteBuffer(m_static_settings.FullChunkSize[0] * 12, sizeof(glm::fvec4));
    //m_cubeIndex_buffer = m_controller->NewReadWriteBuffer(m_static_settings.FullChunkSize[0], sizeof(int));

    m_trans_vertex_buffer = m_controller->NewReadWriteBuffer(m_static_settings.FullChunkSize[0] * 15 * m_totalBatches, sizeof(glm::fvec4)); // 7,864,320 bytes / chunk
    m_trans_normal_buffer = m_controller->NewReadWriteBuffer(m_static_settings.FullChunkSize[0] * 15 * m_totalBatches, sizeof(glm::fvec4)); // 7,864,320 bytes / chunk
    m_trans_triangles_buffer = m_controller->NewReadWriteBuffer(m_static_settings.FullChunkSize[0] * 15 * m_totalBatches, sizeof(int)); // 1,966,080 bytes / chunk
    m_trans_counts_buffer = m_controller->NewReadWriteBuffer(m_static_settings.FullChunkSize[0] * m_totalBatches, sizeof(int)); // 131,072 bytes / chunk
    
    m_stitch_map_buffer = m_controller->NewReadWriteBuffer(m_static_settings.FullChunkSize[0] * m_totalBatches, sizeof(int)); // 131,072 bytes / chunk
    // size of above: 17,956,864
    m_stitch_map_offset_buffer = m_controller->NewReadWriteBuffer((m_static_settings.ChunkSize.x * m_static_settings.ChunkSize.y) * m_totalBatches, sizeof(glm::uvec4));


    //const int max_size = UINT16_MAX * sizeof(glm::vec4);
    m_out_vertex_buffer = m_controller->NewReadWriteBuffer(Max_Verts * m_totalBatches, VECTOR4_SIZE);
    m_out_normal_buffer = m_controller->NewReadWriteBuffer(Max_Verts * m_totalBatches, VECTOR4_SIZE);
    m_out_triangles_buffer = m_controller->NewReadWriteBuffer(Max_Verts * m_totalBatches, sizeof(int));
    m_out_counts_buffer = m_controller->NewReadWriteBuffer(m_totalBatches, VECTOR4_SIZE);


    // Create heightmap cache buffers
    for (int i = 0; i < m_numColumns; i++) {
        IComputeBuffer* heightmap = m_controller->NewReadBuffer(expanded_heightmap_size, sizeof(float));
        m_unused_heightmaps.push(heightmap);
    }

    //m_out_debug_buffer_Unify_Fields   = new ComputeBuffer(expanded_chunk_size_1 * m_numBatchesPerGroup, VECTOR4_SIZE);
    //m_out_debug_buffer_Unify_Fields   = new ComputeBuffer(1, sizeof(float));
    //m_out_debug_buffer_Construct      = new ComputeBuffer(m_static_settings.FullChunkSize[0] * m_numBatchesPerGroup, VECTOR4_SIZE);
    //m_out_debug_buffer_Construct      = new ComputeBuffer(1, VECTOR4_SIZE);
    //m_out_debug_buffer_Mark           = new ComputeBuffer(1, VECTOR4_SIZE);
    //m_out_debug_buffer_Stitch_async   = new ComputeBuffer(1, VECTOR4_SIZE);
    

    m_out_debug_buffer_Heightmap = m_controller->NewWriteBuffer(expanded_chunk_size_1 * m_numBatchesPerGroup, sizeof(float));
    m_out_debug_buffer_ISO_Field = m_controller->NewWriteBuffer(expanded_chunk_size_1 * m_numBatchesPerGroup, VECTOR4_SIZE);
    //m_out_debug_buffer_Unify_Fields = m_controller->NewWriteBuffer(expanded_chunk_size_1 * m_numBatchesPerGroup, VECTOR4_SIZE);
    m_out_debug_buffer_Unify_Fields = m_controller->NewWriteBuffer(1, sizeof(float));
    m_out_debug_buffer_Construct = m_controller->NewWriteBuffer(m_static_settings.FullChunkSize[0] * m_numBatchesPerGroup, VECTOR4_SIZE);
    m_out_debug_buffer_Mark = m_controller->NewWriteBuffer(1, VECTOR4_SIZE);
    m_out_debug_buffer_Stitch_async = m_controller->NewWriteBuffer(m_static_settings.FullChunkSize[0] * m_numBatchesPerGroup, VECTOR4_SIZE);


    //out_Debug_Grid_buffer = m_controller->NewReadWriteBuffer(m_static_settings.FullChunkSize[0] * 8, sizeof(glm::fvec4));
    //out_Debug_Grid_buffer = m_controller->NewReadWriteBuffer(m_static_settings.FullChunkSize[1] * m_totalBatches, sizeof(glm::fvec4));
    out_Debug_Grid_buffer = m_controller->NewReadWriteBuffer(m_numBatchesPerGroup * m_static_settings.FullChunkSize[0], sizeof(glm::fvec4));


    // Set Buffer Data
    m_in_static_settings_buffer->SetData(&m_static_settings);

    in_locOffset_buffer->SetData(locOffset);
    in_globalOffsets_buffer->SetData(globalOffsets);

    in_directionOffsets_buffer->SetData((void*)MarchingCubesArrays::directionOffsets);

    in_edgeTable_Buffer->SetData((void*)MarchingCubesArrays::edgeTable);
    in_TriTable_Buffer->SetData((void*)MarchingCubesArrays::triTable);

    //ISO_Material* zero_data = new ISO_Material[expanded_chunk_size * m_numBatchesPerGroup];
    //int* zero_data2 = new int[UINT16_MAX];
    //ZeroMemory(zero_data, expanded_chunk_size * m_numBatchesPerGroup * sizeof(ISO_Material));
    //ZeroMemory(zero_data2, UINT16_MAX * sizeof(int));

    //zero_data2[6] = 7;

    //m_iso_mat_buffer->SetData(zero_data);
    //m_out_triangles_buffer->SetData(zero_data2);

    //delete[] zero_data;
    //delete[] zero_data2;

    // Bind buffers.

    //m_program_compute.AddBuffer(0, m_in_Buffer);
    //m_program_compute.AddBuffer(1, m_out_Buffer);


    m_program_heightmap->AddBuffer(0, m_in_static_settings_buffer);
    m_program_heightmap->AddBuffer(1, m_in_run_settings_buffer);
    m_program_heightmap->AddBuffer(2, m_heightmap_data_buffer);
    m_program_heightmap->AddBuffer(3, m_out_debug_buffer_Heightmap);


    m_program_iso_field->AddBuffer(0, m_in_static_settings_buffer);
    m_program_iso_field->AddBuffer(1, m_in_run_settings_buffer);
    m_program_iso_field->AddBuffer(2, m_iso_field_buffer);
    m_program_iso_field->AddBuffer(3, m_out_debug_buffer_ISO_Field);

    //m_program_material_field

    m_program_unify_fields->AddBuffer(0, m_in_static_settings_buffer);
    m_program_unify_fields->AddBuffer(1, m_in_run_settings_buffer);
    m_program_unify_fields->AddBuffer(2, m_heightmap_data_buffer);
    m_program_unify_fields->AddBuffer(3, m_iso_field_buffer);
    m_program_unify_fields->AddBuffer(4, m_material_buffer);
    m_program_unify_fields->AddBuffer(5, m_iso_mat_buffer);
    m_program_unify_fields->AddBuffer(6, m_out_debug_buffer_Unify_Fields);


    // ##### OLD ####
    /*
    m_program_smoothrender_createvertlist->AddBuffer(0, m_in_static_settings_buffer);
    m_program_smoothrender_createvertlist->AddBuffer(1, m_in_run_settings_buffer);
    m_program_smoothrender_createvertlist->AddBuffer(2, in_locOffset_buffer);
    m_program_smoothrender_createvertlist->AddBuffer(3, in_globalOffsets_buffer);
    m_program_smoothrender_createvertlist->AddBuffer(4, in_directionOffsets_buffer);
    m_program_smoothrender_createvertlist->AddBuffer(5, in_edgeTable_Buffer);
    m_program_smoothrender_createvertlist->AddBuffer(6, m_iso_mat_buffer);
    m_program_smoothrender_createvertlist->AddBuffer(7, m_vertList_buffer);
    m_program_smoothrender_createvertlist->AddBuffer(8, m_cubeIndex_buffer);
    //m_program_smoothrender_createvertlist->AddBuffer(9, out_Debug_Grid_buffer);
    // */
    // ##############

    m_program_smoothrender_construct->AddBuffer(0, m_in_static_settings_buffer);
    m_program_smoothrender_construct->AddBuffer(1, m_in_run_settings_buffer);
    m_program_smoothrender_construct->AddBuffer(2, in_locOffset_buffer);
    m_program_smoothrender_construct->AddBuffer(3, in_globalOffsets_buffer);
    m_program_smoothrender_construct->AddBuffer(4, in_directionOffsets_buffer);
    m_program_smoothrender_construct->AddBuffer(5, in_edgeTable_Buffer);
    m_program_smoothrender_construct->AddBuffer(6, in_TriTable_Buffer);
    m_program_smoothrender_construct->AddBuffer(7, m_iso_mat_buffer);
    m_program_smoothrender_construct->AddBuffer(8, m_trans_vertex_buffer);
    m_program_smoothrender_construct->AddBuffer(9, m_trans_normal_buffer);
    m_program_smoothrender_construct->AddBuffer(10, m_trans_triangles_buffer);
    m_program_smoothrender_construct->AddBuffer(11, m_trans_counts_buffer);
    m_program_smoothrender_construct->AddBuffer(12, m_stitch_map_buffer);
    m_program_smoothrender_construct->AddBuffer(13, m_out_debug_buffer_Construct);

    // ##### OLD ####
    /*
    m_program_smoothrender_createmesh->AddBuffer(0, m_in_static_settings_buffer);
    m_program_smoothrender_createmesh->AddBuffer(1, m_in_run_settings_buffer);
    m_program_smoothrender_createmesh->AddBuffer(2, in_TriTable_Buffer);
    m_program_smoothrender_createmesh->AddBuffer(3, in_edgeTable_Buffer);
    m_program_smoothrender_createmesh->AddBuffer(4, m_vertList_buffer);
    m_program_smoothrender_createmesh->AddBuffer(5, m_cubeIndex_buffer);
    m_program_smoothrender_createmesh->AddBuffer(6, m_out_vertex_buffer);
    m_program_smoothrender_createmesh->AddBuffer(7, m_out_normal_buffer);
    m_program_smoothrender_createmesh->AddBuffer(8, m_out_triangles_buffer);
    m_program_smoothrender_createmesh->AddBuffer(9, m_out_counts_buffer);
    */
    // ##############

    m_program_smoothrender_mark->AddBuffer(0, m_in_static_settings_buffer);
    m_program_smoothrender_mark->AddBuffer(1, m_in_run_settings_buffer);
    m_program_smoothrender_mark->AddBuffer(2, m_trans_counts_buffer);
    m_program_smoothrender_mark->AddBuffer(3, m_stitch_map_buffer);
    m_program_smoothrender_mark->AddBuffer(4, m_stitch_map_offset_buffer);
    m_program_smoothrender_mark->AddBuffer(5, m_out_counts_buffer);
    m_program_smoothrender_mark->AddBuffer(6, m_out_debug_buffer_Mark);


    m_program_smoothrender_mark_offsets->AddBuffer(0, m_in_static_settings_buffer);
    m_program_smoothrender_mark_offsets->AddBuffer(1, m_stitch_map_offset_buffer);
    m_program_smoothrender_mark_offsets->AddBuffer(2, m_out_counts_buffer);

    /*m_program_smoothrender_stitch->AddBuffer(0, m_in_static_settings_buffer);
    m_program_smoothrender_stitch->AddBuffer(1, m_in_run_settings_buffer);
    m_program_smoothrender_stitch->AddBuffer(2, m_trans_vertex_buffer);
    m_program_smoothrender_stitch->AddBuffer(3, m_trans_normal_buffer);
    m_program_smoothrender_stitch->AddBuffer(4, m_trans_triangles_buffer);
    m_program_smoothrender_stitch->AddBuffer(5, m_trans_counts_buffer);
    m_program_smoothrender_stitch->AddBuffer(6, m_out_vertex_buffer);
    m_program_smoothrender_stitch->AddBuffer(7, m_out_normal_buffer);
    m_program_smoothrender_stitch->AddBuffer(8, m_out_triangles_buffer);
    m_program_smoothrender_stitch->AddBuffer(9, m_out_counts_buffer);*/

    m_program_smoothrender_stitch_async->AddBuffer(0,  m_in_static_settings_buffer);
    m_program_smoothrender_stitch_async->AddBuffer(1,  m_in_run_settings_buffer);
    m_program_smoothrender_stitch_async->AddBuffer(2,  m_trans_vertex_buffer);
    m_program_smoothrender_stitch_async->AddBuffer(3,  m_trans_normal_buffer);
    m_program_smoothrender_stitch_async->AddBuffer(4,  m_trans_triangles_buffer);
    m_program_smoothrender_stitch_async->AddBuffer(5,  m_trans_counts_buffer);
    m_program_smoothrender_stitch_async->AddBuffer(6,  m_stitch_map_buffer);
    m_program_smoothrender_stitch_async->AddBuffer(7,  m_stitch_map_offset_buffer);
    m_program_smoothrender_stitch_async->AddBuffer(8,  m_out_vertex_buffer);
    m_program_smoothrender_stitch_async->AddBuffer(9,  m_out_normal_buffer);
    m_program_smoothrender_stitch_async->AddBuffer(10, m_out_triangles_buffer);
    m_program_smoothrender_stitch_async->AddBuffer(11, m_out_counts_buffer);
    m_program_smoothrender_stitch_async->AddBuffer(12, m_out_debug_buffer_Stitch_async);
    
    

}

void SmoothVoxelBuilder::FinalizePrograms()
{
    //m_program_compute.Finalize();
    
    m_program_heightmap->Finalize();
    m_program_iso_field->Finalize();
    //m_program_material_field->Finalize();
    m_program_unify_fields->Finalize();

    //m_program_smoothrender_createvertlist->Finalize();
    //m_program_smoothrender_createmesh->Finalize();

    m_program_smoothrender_construct->Finalize();
    m_program_smoothrender_mark->Finalize();
    m_program_smoothrender_mark_offsets->Finalize();
    //m_program_smoothrender_stitch->Finalize();
    m_program_smoothrender_stitch_async->Finalize();
}


// Noise Generation

double SmoothVoxelBuilder::GenerateHeightmap(int group, Run_Settings* group_set)
{
    if (USE_HEIGHTMAP_CACHE)
    {
        return GenerateHeightmap_do_cache(group, group_set);
    }
    else {
        return GenerateHeightmap_dont_cache(group, group_set);
    }
}

double SmoothVoxelBuilder::GenerateHeightmap_do_cache(int group, Run_Settings* group_set)
{
    auto start = std::chrono::high_resolution_clock::now();

    int heightmap_size = (m_static_settings.ChunkSize.x + 2) * (m_static_settings.ChunkSize.y + 2);

    std::vector<Run_Settings> heightmap_runs;
    std::vector<Run_Settings> duplicate_runs;
    std::vector<int> execution_runs_list;

    bool hasDisabledChunks = false;
    for (int i = 0; i < m_numBatchesPerGroup; i++) {
        auto chunk_location = group_set[i].Location;
        int col_index = Hash_2D(chunk_location.x, chunk_location.z);

        if (m_heightmap_cache.count(col_index) <= 0) {

            // column does not exist. check if we can create one. 
            // if not, disable generation for this chunk
            if (!m_unused_heightmaps.empty()) {
                IComputeBuffer* chached_heightmap = m_unused_heightmaps.front();
                m_unused_heightmaps.pop();

                m_heightmap_cache[col_index] = chached_heightmap;

                Run_Settings settings;
                settings.Location = glm::ivec4(chunk_location.x, chunk_location.z, i, col_index);

                execution_runs_list.push_back(i);
                heightmap_runs.push_back(settings);

                //m_program_heightmap->Execute(m_numBatchesPerGroup * heightmap_size, 0, 0);

                //m_heightmap_data_buffer->CopyTo(chached_heightmap, i * heightmap_size, 0, heightmap_size * sizeof(float));
                m_activeColumIndex[i] = col_index;
            }
            else {
                // chunk can not be created.
                group_set[i].Location = glm::ivec4(INT32_MIN, INT32_MIN, INT32_MIN, -1);
                hasDisabledChunks = true;
            }

        }
        else {
            // just use cached chunk
            if (col_index != m_activeColumIndex[i]) {

                // TODO: This needs to check to make sure the heightmap isn't in the runs list to be generated.
                auto find_res = std::find(execution_runs_list.begin(), execution_runs_list.end(), i);
                if (find_res != execution_runs_list.end()) {
                    // contains col_index
                    int src_i = execution_runs_list[find_res - execution_runs_list.begin()];
                    Run_Settings settings;
                    settings.Location = glm::ivec4(src_i, i, col_index, 0);
                    duplicate_runs.push_back(settings);
                }
                else {
                    // does not contain col_index
                    IComputeBuffer* chached_heightmap = m_heightmap_cache[col_index];

                    chached_heightmap->CopyTo(m_heightmap_data_buffer, 0, i * heightmap_size * sizeof(float), heightmap_size * sizeof(float));
                }



                m_activeColumIndex[i] = col_index;
            }
        }

        // Otherwise, it's already loaded in the active index.
    }

    if (hasDisabledChunks) {
        m_in_run_settings_buffer->SetData(group_set, m_numBatchesPerGroup * sizeof(Run_Settings));
    }

    // For each hightmap that needs to be generated, generate and load into the respective heightmap cache.
    if (heightmap_runs.size() > 0) {
        m_heightmap_run_settings_buffer->SetData(heightmap_runs.data(), heightmap_runs.size() * sizeof(Run_Settings));
        m_program_heightmap->Execute(heightmap_runs.size() * heightmap_size, 0, 0);

        for (int i = 0; i < heightmap_runs.size(); i++) {
            Run_Settings run = heightmap_runs[i];
            IComputeBuffer* chached_heightmap = m_heightmap_cache[run.Location.w];
            m_heightmap_data_buffer->CopyTo(chached_heightmap, run.Location.z * heightmap_size * sizeof(float), 0, heightmap_size * sizeof(float));
        }
    }

    // If more than one chunk in the batch group needs to be generated for a single column, just
    // copy the first one to each slot of the active buffer from the cached one.
    // 
    // TODO: instead of copying, configure the chunk gen to refer to one of the heightmap "slots"
    //      in the active buffer.
    if (duplicate_runs.size() > 0) {
        for (int i = 0; i < duplicate_runs.size(); i++) {
            Run_Settings run = duplicate_runs[i];
            IComputeBuffer* chached_heightmap = m_heightmap_cache[run.Location.z];
            chached_heightmap->CopyTo(m_heightmap_data_buffer, run.Location.y * heightmap_size * sizeof(float), 0, heightmap_size * sizeof(float));
        }
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double>(end - start).count();
    return duration;
}

double SmoothVoxelBuilder::GenerateHeightmap_dont_cache(int group, Run_Settings* group_start)
{
    auto start = std::chrono::high_resolution_clock::now();


    int heightmap_size = (m_static_settings.ChunkSize.x + 2) * (m_static_settings.ChunkSize.y + 2);

    m_program_heightmap->Execute(m_numBatchesPerGroup * heightmap_size, 0, 0);


    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double>(end - start).count();

    /*int search_i = GetBatchNumIndex(group_start, m_numBatchesPerGroup, glm::ivec3(-1, -1, -1));

    if (search_i == -1) {
        return duration;
    }

    printf("Found heightmap in batch index %i\n", search_i);*/

    //float* Data = new float[heightmap_size];
    //glm::fvec4* Data = new glm::fvec4[heightmap_size];
    //glm::fvec4* Data1 = new glm::fvec4[heightmap_size];

    //out_Debug_Grid_buffer->GetData(Data, 0 * heightmap_size * sizeof(glm::fvec4), heightmap_size * sizeof(glm::fvec4));
    //out_Debug_Grid_buffer->GetData(Data1, 1 * heightmap_size * sizeof(glm::fvec4), heightmap_size * sizeof(glm::fvec4));
    //m_heightmap_data_buffer->GetData(Data, search_i * heightmap_size * sizeof(float), heightmap_size * sizeof(float));

    /*for (int i = 0; i < heightmap_size; i++)
    {
        //printf("height res '%i': %f\n", i, Data[i]);
        //printf("heightmap debug %i: (%f, %f, %f, %f)\n", i,
        //    Data[i].x, Data[i].y, Data[i].z, Data[i].w);

        //printf("heightmap debug %i: (%f, %f, %f, %f), (%f, %f, %f, %f)\n", i,
        //    Data[i].x, Data[i].y, Data[i].z, Data[i].w, 
        //    Data1[i].x, Data1[i].y, Data1[i].z, Data1[i].w);
    }*/

    //delete[] Data;

    return duration;
}

double SmoothVoxelBuilder::GenerateISOField(int group, Run_Settings* group_start)
{
    auto start = std::chrono::high_resolution_clock::now();

    m_program_iso_field->Execute(m_numBatchesPerGroup * (m_static_settings.ChunkSize.x + 2) * (m_static_settings.ChunkSize.y + 2) * (m_static_settings.ChunkSize.z + 2), 0, 0);
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double>(end - start).count();
    /*float* Data = new float[FullChunkSize];
    m_iso_field_buffer->GetData(Data);

    for (int i = 0; i < FullChunkSize; i++)
    {
        printf("res '%i': %f\n", i, Data[i]);
    }

    delete[] Data;*/

    return duration;
}

double SmoothVoxelBuilder::GenerateMaterialField(int group, Run_Settings* group_start)
{
    auto start = std::chrono::high_resolution_clock::now();

    m_program_material_field->Execute(m_numBatchesPerGroup * (m_static_settings.ChunkSize.x + 2) * (m_static_settings.ChunkSize.y + 2) * (m_static_settings.ChunkSize.z + 2), 0, 0);

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double>(end - start).count();
    return duration;
}

double SmoothVoxelBuilder::AssembleUnifiedField(int group, Run_Settings* group_start)
{
    auto start = std::chrono::high_resolution_clock::now();

    //m_program_unify_fields->Execute(m_numBatchesPerGroup * (m_static_settings.ChunkSize.x + 1) * (m_static_settings.ChunkSize.y + 1) * (m_static_settings.ChunkSize.z + 1), 0, 0);
       
    //int search_i = GetBatchNumIndex(group_start, m_numBatchesPerGroup, glm::ivec3(0, 0, 0));

    /*if (search_i == -1) {
        return 0;
    }

    printf("Found chunk in batch index %i\n", search_i);*/

    //glm::vec4* Data_debug1 = new glm::vec4[m_static_settings.FullChunkSize[1]];
    //glm::vec4* Data_debug2 = new glm::vec4[m_static_settings.FullChunkSize[1]];

    
    //ISO_Material* Data_iso1 = new ISO_Material[m_static_settings.FullChunkSize[1]];
    //ISO_Material* Data_iso2 = new ISO_Material[m_static_settings.FullChunkSize[1]];

    //out_Debug_Grid_buffer->GetData(Data_debug1, 0 * m_static_settings.FullChunkSize[1] * sizeof(glm::vec4), m_static_settings.FullChunkSize[1] * sizeof(glm::vec4));
    //out_Debug_Grid_buffer->GetData(Data_debug2, 1 * m_static_settings.FullChunkSize[1] * sizeof(glm::vec4), m_static_settings.FullChunkSize[1] * sizeof(glm::vec4));

    //m_iso_mat_buffer->GetData(Data_iso1, search_i * m_static_settings.FullChunkSize[1] * sizeof(ISO_Material), m_static_settings.FullChunkSize[1] * sizeof(ISO_Material));
    //m_iso_mat_buffer->GetData(Data_iso2, 1 * m_static_settings.FullChunkSize[1] * sizeof(ISO_Material), m_static_settings.FullChunkSize[1] * sizeof(ISO_Material));

    //for (int i = 0; i < m_static_settings.FullChunkSize[1]; i++) {
        //printf("res grid '%i': (%f, %f, %f, %f), (%f, %f, %f, %f)\n", i,
        //    Data_debug1[i].x, Data_debug1[i].y, Data_debug1[i].z, Data_debug1[i].w,
        //    Data_debug2[i].x, Data_debug2[i].y, Data_debug2[i].z, Data_debug2[i].w
        //);

        /*if (Data_iso1[i].final_iso.w > -1) {
            printf("res grid '%i': (%f, %f, %f, %f)\n", i,
                Data_iso1[i].final_iso.x, Data_iso1[i].final_iso.y, Data_iso1[i].final_iso.z, Data_iso1[i].final_iso.w
            );
        }*/

        //printf("res grid '%i': (%f, %f, %f, %f), (%f, %f, %f, %f)\n", i,
        //    Data_iso1[i].final_iso.x, Data_iso1[i].final_iso.y, Data_iso1[i].final_iso.z, Data_iso1[i].final_iso.w,
        //    Data_iso2[i].final_iso.x, Data_iso2[i].final_iso.y, Data_iso2[i].final_iso.z, Data_iso2[i].final_iso.w
        //);
    //}

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double>(end - start).count();
    return duration;
}


// Chunk Rendering



double SmoothVoxelBuilder::Construct(int group, Run_Settings* group_start)
{
    auto start = std::chrono::high_resolution_clock::now();
    /*for (int i = 0; i < m_numBatchGroups; i++) {
        m_in_run_settings_buffer->SetData(&m_run_settings[i * m_numBatchesPerGroup], m_numBatchesPerGroup * sizeof(Run_Settings));
        m_program_smoothrender_construct->Execute(m_numBatchesPerGroup * m_static_settings.FullChunkSize[0], 0, 0);
    }*/
    m_program_smoothrender_construct->Execute(m_numBatchesPerGroup * m_static_settings.FullChunkSize[0], 0, 0);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double>(end - start).count();
    double construct_t = duration;

    //int search_i = GetBatchNumIndex(group_start, m_numBatchesPerGroup, glm::ivec3(0, 0, 0));

    //if (search_i == -1) {
    //    return 0;
    //}

    //printf("Found chunk in batch index %i\n", search_i);

    //glm::vec4* debug_data = new glm::vec4[m_static_settings.FullChunkSize[0]];

    //m_out_debug_buffer_Construct->GetData(debug_data, (group * m_numBatchesPerGroup + search_i) * m_static_settings.FullChunkSize[0] * VECTOR4_SIZE, m_static_settings.FullChunkSize[0] * VECTOR4_SIZE);

    //int* Data_count1 = new int[m_static_settings.FullChunkSize[0]];
    //int* Data_count2 = new int[m_static_settings.FullChunkSize[0]];

    //m_trans_counts_buffer->GetData(Data_count1, (group * m_numBatchesPerGroup + search_i) * m_static_settings.FullChunkSize[0] * sizeof(int), m_static_settings.FullChunkSize[0] * sizeof(int));
    //m_trans_counts_buffer->GetData(Data_count2, (group * m_numBatchesPerGroup + 1) * m_static_settings.FullChunkSize[0] * sizeof(int), m_static_settings.FullChunkSize[0] * sizeof(int));

    //for (int i = 0; i < m_static_settings.FullChunkSize[0]; i++) {
        //Logger::LogDebug(LOG_POS("Construct"), "(%f, %f, %f, %f)", 
        //    debug_data[i].x, debug_data[i].y, debug_data[i].z, debug_data[i].w);

        //printf("res '%i': %i ; %i\n", i, Data_count1[i], Data_count2[i]);
        //if (Data_count1[i] > 0)
        //    printf("res '%i': %i\n", i, Data_count1[i]);
    //}

    //delete[] debug_data;

    return construct_t;
}

glm::dvec4 SmoothVoxelBuilder::DoRender()
{
    //m_program_smoothrender_createvertlist->Execute(m_static_settings.FullChunkSize[0], 0, 0);
    //m_program_smoothrender_createmesh->Execute(1, 0, 0);

    /*auto start = std::chrono::high_resolution_clock::now();
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double>(end - start).count();
    double construct_t = duration;*/

    double buffer_writes_time = 0;

    
    //m_in_run_settings_buffer->SetData(m_run_settings);

    //m_program_smoothrender_mark->Execute(m_totalBatches, 0, 0);
    auto start_buffer_writes = std::chrono::high_resolution_clock::now();

    //m_in_run_settings_buffer->SetData(m_run_settings, m_totalBatches * sizeof(Run_Settings));
    
    auto end_buffer_writes = std::chrono::high_resolution_clock::now();
    buffer_writes_time += std::chrono::duration<double>(end_buffer_writes - start_buffer_writes).count();

    auto start = std::chrono::high_resolution_clock::now();

    m_program_smoothrender_mark->Execute((m_static_settings.ChunkSize.x * m_static_settings.ChunkSize.y) * m_totalBatches, 0, 0);

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double>(end - start).count();
    double mark_t = duration;




    start = std::chrono::high_resolution_clock::now();

    //printf("CPU Execute Mark Offsets: %i\n", m_totalBatches);
    m_program_smoothrender_mark_offsets->Execute(m_totalBatches, 0, 0);

    end = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration<double>(end - start).count();
    double mark_offsets_t = duration;

    /*
    int grid_size = (m_static_settings.ChunkSize.x * m_static_settings.ChunkSize.y);
    glm::ivec4* debug_data = new glm::ivec4[grid_size * m_totalBatches];

    m_stitch_map_offset_buffer->GetData(debug_data);

    printf("### BEGIN CPU DEBUG PRINT ###\n");
    for (int i = 0; i < grid_size; i++) {
        //if (debug_data[i].x > 0)
            printf("CPU Debug: %i: (%i, %i)\n", i, debug_data[i].x, debug_data[i].y);
    }
    printf("### END CPU DEBUG PRINT ###\n");*/

    //m_out_debug_buffer_Mark->GetData(debug_data);

    //for (int i = 0; i < m_totalBatches; i++) {
    //    Logger::LogDebug(LOG_POS("DoRender"), "res '%i': %f, %f, %f\n", i, debug_data[i].x, debug_data[i].y, debug_data[i].z);
    //}

    //delete[] debug_data;
    
    //glm::ivec4* Data_count1 = new glm::ivec4[m_totalBatches];
    //int* Data_count2 = new int[m_totalBatches];

    //m_out_counts_buffer->GetData(Data_count1);
    //m_trans_counts_buffer->GetData(Data, 0 * m_static_settings.FullChunkSize[0] * sizeof(int), m_static_settings.FullChunkSize[0] * sizeof(int));
    //m_trans_counts_buffer->GetData(Data2, 0 * m_static_settings.FullChunkSize[0] * sizeof(int), m_static_settings.FullChunkSize[0] * sizeof(int));

    /*for (int i = 0; i < m_totalBatches; i++) {
        printf("res '%i': %i, %i, %i\n", i, Data_count1[i].x, Data_count1[i].y, Data_count1[i].z);
    }*/


    duration = 0;
    for (int i = 0; i < m_numBatchGroups; i++) {

        //auto start_buffer_writes = std::chrono::high_resolution_clock::now();
        //int start_index = (i * m_numBatchesPerGroup);
        //Run_Settings* start = m_run_settings + start_index;
        //m_in_run_settings_buffer->SetData(start, m_numBatchesPerGroup * sizeof(Run_Settings));
        //auto end_buffer_writes = std::chrono::high_resolution_clock::now();
        //buffer_writes_time += std::chrono::duration<double>(end_buffer_writes - start_buffer_writes).count();
        



        //m_in_run_settings_buffer->SetData(&m_run_settings[i * m_numBatchesPerGroup], m_numBatchesPerGroup * sizeof(Run_Settings));

        /*for (int i = 0; i < m_numBatchesPerGroup; i++) {
            printf("Constructing (%i, %i, %i)\n",
                start[i].Location.x, start[i].Location.y, start[i].Location.z);
        }*/

        auto start_time = std::chrono::high_resolution_clock::now();
        m_program_smoothrender_stitch_async->Execute(m_numBatchesPerGroup * m_static_settings.FullChunkSize[0], 0, 0);
        auto end_time = std::chrono::high_resolution_clock::now();
        duration += std::chrono::duration<double>(end_time - start_time).count();
        //glm::vec4* debug_data = new glm::vec4[UINT16_MAX * m_totalBatches];

        //m_out_debug_buffer_Stitch_async->GetData(debug_data);
        //m_out_vertex_buffer->GetData(debug_data);

        //m_numBatchesPerGroup * m_static_settings.FullChunkSize[0]
        //for (int i = 0; i < 100; i++) {
            //Logger::LogDebug(LOG_POS("DoRender"), "res '%i': (%f, %f, %f, %f)", i, 
            //    debug_data[i].x, debug_data[i].y, debug_data[i].z, debug_data[i].w);
        //}

        //delete[] debug_data;



        //glm::vec4* Data = new glm::vec4[6144];

        //m_out_vertex_buffer->GetData(Data, 3 * 6144 * sizeof(glm::vec4), counts.x * sizeof(glm::vec4));


        //All_Zero(out_vertex, counts.x, "Extract");

    }
    
    double stitch_t = duration;

    return glm::dvec4(mark_t, mark_offsets_t, stitch_t, buffer_writes_time);
}

glm::ivec4 AddMeshEntry(glm::vec4 vert, glm::vec4 norm, int tris, glm::ivec4 counts) {

    verts_tmp[counts.x] = vert;
    norm_tmp[counts.y] = norm;

    tris_tmp[counts.z] = tris;

    counts.x++;
    counts.y++;
    counts.z++;

    return counts;
}

glm::vec4 GetVertList(glm::vec4* vertlist, int voxelIndex, int vertIndex) {

    int index = (voxelIndex * 12) + vertIndex;

    return vertlist[index];
}

int Get_Tritable(int index) {
    return MarchingCubesArrays::triTable[index];
}

glm::ivec4 SmoothVoxelBuilder::ProcessVoxel(glm::vec4* vertlist, int* cubeIndex, int c_index, glm::ivec4 counts)
{
    
    glm::vec4 vertList[12];

    vertList[0] = GetVertList(vertlist, c_index, 0);
    vertList[1] = GetVertList(vertlist, c_index, 1);
    vertList[2] = GetVertList(vertlist, c_index, 2);
    vertList[3] = GetVertList(vertlist, c_index, 3);

    vertList[4] = GetVertList(vertlist, c_index, 4);
    vertList[5] = GetVertList(vertlist, c_index, 5);
    vertList[6] = GetVertList(vertlist, c_index, 6);
    vertList[7] = GetVertList(vertlist, c_index, 7);

    vertList[8] = GetVertList(vertlist, c_index, 8);
    vertList[9] = GetVertList(vertlist, c_index, 9);
    vertList[10] = GetVertList(vertlist, c_index, 10);
    vertList[11] = GetVertList(vertlist, c_index, 11);

    int cube_index = cubeIndex[c_index];
    int edge_ent = MarchingCubesArrays::edgeTable[cube_index];
    
    //printf("cube index 1: %i\n", cube_index);
    //printf("edge_ent 1: %i\n", edge_ent);

    if (edge_ent == 0) {
        return counts;
    }

    printf("cube index 2: %i\n", cube_index);
    printf("edge_ent 2: %i\n", edge_ent);

    for (int i = 0; Get_Tritable(cube_index * 16 + i) != -1; i += 3, counts.z += 3) {

        int curTris = counts.z;

        glm::vec4 p1 = vertList[Get_Tritable(cube_index * 16 + (i + 0))];
        glm::vec4 p2 = vertList[Get_Tritable(cube_index * 16 + (i + 1))];
        glm::vec4 p3 = vertList[Get_Tritable(cube_index * 16 + (i + 2))];

        glm::vec4 n1 = glm::vec4();// normalSample(p1);
        int t1 = curTris + 2;
        counts = AddMeshEntry(p1, n1, t1, counts);

        glm::vec4 n2 = glm::vec4();// normalSample(p2);
        int t2 = curTris + 1;
        counts = AddMeshEntry(p1, n1, t2, counts);

        glm::vec4 n3 = glm::vec4();// normalSample(p3);
        int t3 = curTris + 0;
        counts = AddMeshEntry(p1, n1, t3, counts);

    }

    return counts;


}


glm::ivec4 SmoothVoxelBuilder::BuildMesh()
{
    int FullChunkSize = m_static_settings.FullChunkSize[0];

    glm::vec4* vertlist = new glm::vec4[FullChunkSize * 12];
    int* cubeIndex = new int[FullChunkSize];

    m_vertList_buffer->GetData(vertlist);
    m_cubeIndex_buffer->GetData(cubeIndex);

    glm::ivec4 counts = glm::ivec4(0, 0, 0, 0);
    for (int c_index = 0; c_index < FullChunkSize; c_index++) {
        counts = ProcessVoxel(vertlist, cubeIndex, c_index, counts);
    }

    return counts;
}

void SmoothVoxelBuilder::Extract(glm::vec4* out_vertex, glm::vec4* out_normal, unsigned int* out_trianges, glm::ivec4 counts)
{

    if (counts.x > 0) {

        if (!USE_CACHE) {
            Logger::LogDebug(LOG_POS("Extract"), "Running extract: %i, %i", counts.y * VECTOR4_SIZE, counts.x * VECTOR4_SIZE);
            //int start = 
            //printf("Extract: %i, %i, %i, %i\n", counts.x, counts.z, counts.y, counts.w);
            m_out_vertex_buffer->GetData(out_vertex, counts.y * VECTOR4_SIZE, counts.x * VECTOR4_SIZE);
            m_out_normal_buffer->GetData(out_normal, counts.y * VECTOR4_SIZE, counts.x * VECTOR4_SIZE);

            //m_out_triangles_buffer->GetData(out_trianges, counts.z * counts.x * sizeof(int), counts.x * sizeof(int));
            //All_Zero(out_vertex, counts.x, "Extract");

            if (COMPUTE_TRIANGES && out_trianges != nullptr) {

                for (int i = 0; i < counts.x; i += 3) {
                    unsigned int tris_start = i;

                    if (m_invert_tris) {
                        out_trianges[tris_start + 0] = tris_start + 0;
                        out_trianges[tris_start + 1] = tris_start + 1;
                        out_trianges[tris_start + 2] = tris_start + 2;
                    }
                    else
                    {
                        out_trianges[tris_start + 0] = tris_start + 2;
                        out_trianges[tris_start + 1] = tris_start + 1;
                        out_trianges[tris_start + 2] = tris_start + 0;
                    }
                }
            }
        }
        else {
            if (!extract_cached) {
                int total_size = 0;
                for (int i = 0; i < counts_cache.size(); i++) {
                    total_size += counts_cache[i].x;
                }

                vertex_cache = new glm::vec4[total_size];
                normal_cache = new glm::vec4[total_size];
                tris_cache = new int[total_size];

                //printf("Reading %i into cache.\n", total_size);
                m_out_vertex_buffer->GetData(vertex_cache, total_size * sizeof(glm::vec4));
                m_out_normal_buffer->GetData(normal_cache, total_size * sizeof(glm::vec4));

                extract_cached = true;
            }

            for (int i = 0; i < counts.x; i += 3) {
                int tris_start = i;
                out_trianges[tris_start + 0] = tris_start + 2;
                out_trianges[tris_start + 1] = tris_start + 1;
                out_trianges[tris_start + 2] = tris_start + 0;
            }

            std::memcpy((void*)out_vertex, (void*)(vertex_cache + (counts.y * sizeof(glm::vec4))), counts.x * sizeof(glm::vec4));
            std::memcpy((void*)out_normal, (void*)(normal_cache + (counts.y * sizeof(glm::vec4))), counts.x * sizeof(glm::vec4));
        }
    }

    // 75: count 3, 
    
    int expanded_chunk_size = (m_static_settings.ChunkSize.x + 1) * (m_static_settings.ChunkSize.y + 1) * (m_static_settings.ChunkSize.z + 1);

    //int* Data_t = new int[UINT16_MAX * 2];
    //int* Data2 = new int[m_static_settings.FullChunkSize[0]];
    //glm::fvec4* Data = new glm::fvec4[UINT16_MAX * 2];
    //glm::fvec4* Data_o = new glm::fvec4[m_static_settings.FullChunkSize[0] * 15];
    //glm::fvec4* Data = new glm::fvec4[m_static_settings.FullChunkSize[0]];
    //glm::fvec4* Data_o = new glm::fvec4[m_static_settings.FullChunkSize[0]];
    //glm::fvec4* Data_all = new glm::fvec4[m_static_settings.FullChunkSize[0] * 2];
    //ISO_Material* Data2 = new ISO_Material[expanded_chunk_size];
    //ISO_Material* Data2_o = new ISO_Material[expanded_chunk_size];
    //ISO_Material* Data2_all = new ISO_Material[expanded_chunk_size * 2];

    //Static_Settings static_settings2{};

    //m_cubeIndex_buffer->GetData(Data);
    //m_vertList_buffer->GetData(Data);
    //out_Debug_Grid_buffer->GetData(Data_all);
    //out_Debug_Grid_buffer->GetData(Data, 0 * m_static_settings.FullChunkSize[0] * sizeof(glm::vec4), m_static_settings.FullChunkSize[0] * sizeof(glm::vec4));
    //out_Debug_Grid_buffer->GetData(Data_o, 1 * m_static_settings.FullChunkSize[0] * sizeof(glm::vec4), m_static_settings.FullChunkSize[0] * sizeof(glm::vec4));
    //m_iso_mat_buffer->GetData(Data2_all);
    //m_iso_mat_buffer->GetData(Data2, 0 * expanded_chunk_size * sizeof(ISO_Material), expanded_chunk_size * sizeof(ISO_Material));
    //m_iso_mat_buffer->GetData(Data2_o, 1 * expanded_chunk_size * sizeof(ISO_Material), expanded_chunk_size * sizeof(ISO_Material));
    //m_iso_field_buffer->GetData(Data, 0 * expanded_chunk_size * sizeof(glm::fvec4), expanded_chunk_size * sizeof(glm::fvec4));
    //m_iso_field_buffer->GetData(Data_o, 1 * expanded_chunk_size * sizeof(glm::fvec4), expanded_chunk_size * sizeof(glm::fvec4));
    //in_directionOffsets_buffer->GetData(Data);
    //m_trans_vertex_buffer->GetData(Data, 0 * m_static_settings.FullChunkSize[0] * 15 * sizeof(glm::fvec4), m_static_settings.FullChunkSize[0] * 15 * sizeof(glm::fvec4));
    //m_trans_vertex_buffer->GetData(Data_o, 1 * m_static_settings.FullChunkSize[0] * 15 * sizeof(glm::fvec4), m_static_settings.FullChunkSize[0] * 15 * sizeof(glm::fvec4));
    //m_out_triangles_buffer->GetData(Data, m_static_settings.FullChunkSize[0] * sizeof(int));
    //m_trans_counts_buffer->GetData(Data, 0 * m_static_settings.FullChunkSize[0] * sizeof(int), m_static_settings.FullChunkSize[0] * sizeof(int));
    //m_trans_counts_buffer->GetData(Data2, 0 * m_static_settings.FullChunkSize[0] * sizeof(int), m_static_settings.FullChunkSize[0] * sizeof(int));
    //m_stitch_map_buffer->GetData(Data);

    //m_out_triangles_buffer->GetData(Data_t);

    //m_in_static_settings_buffer->GetData(&static_settings2);

    //printf("size_x: %i, size_y: %i, size_z: %i", static_settings2.ChunkSizeX, static_settings2.ChunkSizeY, static_settings2.ChunkSizeZ);

    

    /*for (int i = 0; i < 888 * 2; i++)
    {
        //printf("res '%i': %i\n", i, Data_t[i]);
        //printf("res '%i': %i == %i\n", i, Data[i], Data2[i]);
        if (i % 15 == 0) {
            //printf("res '%i': %i\n", i, Data2[i / 15]);
            //printf("res iso '%i': (%f, %f, %f, %f)\n", i, Data2[i / 8].final_iso.x, Data2[i / 8].final_iso.y, Data2[i / 8].final_iso.z, Data2[i / 8].final_iso.w);
        }
        //printf("res grid '%i': (%f, %f, %f, %f), %i\n", i, Data[i].x, Data[i].y, Data[i].z, Data[i].w, 0);
        //printf("res grid '%i': (%f, %f, %f, %f)\n", i, out_vertex[i].x, out_vertex[i].y, out_vertex[i].z, out_vertex[i].w);
        //printf("res grid '%i': (%f, %f, %f, %f)\n", i, Data[i].x, Data[i].y, Data[i].z, Data[i].w);
        //printf("res '%i': %f == %f\n", i, Data2[i].final_iso.w, Data2_o[i].final_iso.w);

        //if (Data[i / 15] > 0) {
            //printf("res2 '%i': (%f, %f, %f, %f) == (%f, %f, %f, %f)\n", i,
            //    Data[i].x, Data[i].y, Data[i].z, Data[i].w,
            //    Data[UINT16_MAX + i].x, Data[UINT16_MAX + i].y, Data[UINT16_MAX + i].z, Data[UINT16_MAX + i].w);
        //}

        //printf("res '%i': (%f, %f, %f, %f) == (%f, %f, %f, %f)\n", i,
        //    Data[i].x, Data[i].y, Data[i].z, Data[i].w,
        //    Data_o[i].x, Data_o[i].y, Data_o[i].z, Data_o[i].w);

        //printf("res '%i': (%f, %f, %f, %f) == (%f, %f, %f, %f)\n", i,
        //    Data2[i].final_iso.x, Data2[i].final_iso.y, Data2[i].final_iso.z, Data2[i].final_iso.w,
        //    Data2_o[i].final_iso.x, Data2_o[i].final_iso.y, Data2_o[i].final_iso.z, Data2_o[i].final_iso.w);

        //printf("res '%i': %f\n", i, Data2_all[i].final_iso.w);
    }*/

    //printf("m_numBatchGroups: %i, m_numBatchesPerGroup: %i, m_totalBatches: %i\n", 
    //    m_numBatchGroups, m_numBatchesPerGroup, m_totalBatches);

    //delete[] Data; 


}

void SmoothVoxelBuilder::Extract(IComputeBuffer* out_vertex, IComputeBuffer* out_normal, IComputeBuffer* out_trianges, glm::ivec4 counts)
{
    if (counts.x <= 0) {
        return;
    }

    //Logger::LogDebug(LOG_POS("Extract"), "Running extract: %i, %i", counts.y * VECTOR4_SIZE, counts.x * VECTOR4_SIZE);
    //int start = 
    //printf("Extract: %i, %i, %i, %i\n", counts.x, counts.z, counts.y, counts.w);

    //m_out_vertex_buffer->GetData(out_vertex, counts.y * VECTOR4_SIZE, counts.x * VECTOR4_SIZE);
    //m_out_normal_buffer->GetData(out_normal, counts.y * VECTOR4_SIZE, counts.x * VECTOR4_SIZE);

    if (m_out_vertex_buffer != nullptr)
        m_out_vertex_buffer->CopyTo(out_vertex, counts.y * VECTOR4_SIZE, 0, counts.x * VECTOR4_SIZE);

    if (m_out_normal_buffer != nullptr)
        m_out_normal_buffer->CopyTo(out_normal, counts.y * VECTOR4_SIZE, 0, counts.x * VECTOR4_SIZE);

    if (out_trianges != nullptr)
        m_out_triangles_buffer->CopyTo(out_trianges, counts.z * counts.x * sizeof(int), 0, counts.x * sizeof(int));
}


