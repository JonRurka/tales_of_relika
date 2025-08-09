#include "CubeVoxelBuilder.h"

#include "StructureDataStorage.h"
#include "Logger.h"

#define MAX_STORED_CHUNKS 100

#define GRID_PADDING 2
#define GRID_OFFSET 1

#define USE_TRIANGLES false

using namespace VoxelEngine;

#define PUSH_VERTS(v_list, n_list, counts, v, n)  \
do {                                   \
    v_list[counts.x] = v;              \
    n_list[counts.x] = n;              \
    counts.x++;                        \
}while(false)


#define PUSH_TRIS(t_list, counts, t)   \
do {                                   \
    t_list[counts.y] = t;              \
    counts.y++;                        \
}while(false)


namespace {
    glm::ivec3 C_1D_to_3D(uint32_t i, uint32_t width, uint32_t height) {
        int z = int(i / (width * height));
        int y = int((i % (width * height)) / width);
        int x = int(i % width);

        return glm::ivec3(x, y, z);
    }

    uint32_t C_3D_to_1D(int x, int y, int z, uint32_t width, uint32_t height) {
        return z * width * height + y * width + x;
    }
}

void CubeVoxelBuilder::Init(ChunkSettings& settings)
{
    Settings& p_settings = settings.GetSettings();

    float vpm = p_settings.getFloat("voxelsPerMeter");

    m_static_settings.VoxelsPerMeter = glm::vec4(vpm, vpm, vpm, vpm);
    m_static_settings.ChunkMeterSize.x = p_settings.getInt("chunkMeterSizeX");
    m_static_settings.ChunkMeterSize.y = p_settings.getInt("chunkMeterSizeY");
    m_static_settings.ChunkMeterSize.z = p_settings.getInt("chunkMeterSizeZ");

    CalculateVariables();

    m_structure_data = std::make_shared<StructureDataStorage>(
        m_static_settings.ChunkSize.x, m_static_settings.ChunkSize.y, m_static_settings.ChunkSize.z,
        MAX_STORED_CHUNKS
    );
}

glm::dvec4 CubeVoxelBuilder::Render(
    ChunkRenderOptions& options,
    std::vector<glm::vec4>& out_vertex,
    std::vector<glm::vec4>& out_normal,
    std::vector<glm::vec2>& out_uv,
    std::vector<unsigned int>& out_trianges,
    glm::ivec4& counts)
{
    out_vertex.clear();
    out_normal.clear();
    out_uv.clear();
    out_trianges.clear();
    counts = glm::ivec4(0, 0, 0, 0);


    Out_Data out_data(
        counts,
        out_vertex,
        out_normal,
        out_uv,
        out_trianges
    );
    //out_data.out_vertex = out_vertex;
    //out_data.out_normal = out_normal;
    //out_data.out_uv = out_uv;
    //out_data.out_trianges = out_trianges;
    //out_data.out_counts = counts;

    glm::ivec3 chunk = options.locations[0];

    if (!m_structure_data->Has_Chunk(chunk)) {
        return glm::dvec4();
    }

    const std::vector<uint32_t>& data = m_structure_data->Get_Data(chunk);

    for (int x = 0; x < m_static_settings.ChunkSize.x; ++x) 
    {
        for (int y = 0; y < m_static_settings.ChunkSize.y; ++y) 
        {
            for (int z = 0; z < m_static_settings.ChunkSize.z; ++z) 
            {
                process_block(data, chunk, glm::ivec3(x, y, z), out_data);
            }
        }
    }

    counts = out_data.out_counts;
    Logger::LogDebug(LOG_POS("Render"), "vertices: %i", counts.x);

	return glm::dvec4();
}

void CubeVoxelBuilder::CalculateVariables()
{
    m_static_settings.ChunkSize.x = (int)(m_static_settings.ChunkMeterSize.x * m_static_settings.VoxelsPerMeter.x);
    m_static_settings.ChunkSize.y = (int)(m_static_settings.ChunkMeterSize.y * m_static_settings.VoxelsPerMeter.y);
    m_static_settings.ChunkSize.z = (int)(m_static_settings.ChunkMeterSize.z * m_static_settings.VoxelsPerMeter.z);
    m_static_settings.FullChunkSize[0] = m_static_settings.ChunkSize.x * m_static_settings.ChunkSize.y * m_static_settings.ChunkSize.z;
    m_static_settings.FullChunkSize[1] = (m_static_settings.ChunkSize.x + GRID_PADDING) * (m_static_settings.ChunkSize.y + GRID_PADDING) * (m_static_settings.ChunkSize.z + GRID_PADDING);
    m_static_settings.half_[0] = ((1.0f / (float)m_static_settings.VoxelsPerMeter.x) / 2.0f);
    m_static_settings.SideLength.x = m_static_settings.ChunkMeterSize.x / (float)m_static_settings.ChunkSize.x;
    m_static_settings.SideLength.y = m_static_settings.ChunkMeterSize.y / (float)m_static_settings.ChunkSize.y;
    m_static_settings.SideLength.z = m_static_settings.ChunkMeterSize.z / (float)m_static_settings.ChunkSize.z;

    m_static_settings.skipDist[0] = 1 / (float)m_static_settings.VoxelsPerMeter.x;
}

void CubeVoxelBuilder::process_block(const std::vector<uint32_t>& data, glm::ivec3 chunk_coord, glm::ivec3 voxel_coord, Out_Data& out_data)
{
    int v_idx = C_3D_to_1D(voxel_coord.x + GRID_OFFSET, voxel_coord.y + GRID_OFFSET, voxel_coord.z + GRID_OFFSET, m_static_settings.ChunkSize.x + GRID_PADDING, m_static_settings.ChunkSize.y + GRID_PADDING);
    uint32_t raw_block_data = data[v_idx];
    glm::uvec2 block_info = glm::uvec2(Get_Block_Type(raw_block_data), Get_Block_Orientation(raw_block_data));
    
    if (!render_enabled(block_info)) {
        return;
    }

    process_tile_x_plus(data, block_info, chunk_coord, voxel_coord, out_data);
    process_tile_x_neg(data, block_info, chunk_coord, voxel_coord, out_data);

    process_tile_y_plus(data, block_info, chunk_coord, voxel_coord, out_data);
    process_tile_y_neg(data, block_info, chunk_coord, voxel_coord, out_data);

    process_tile_z_plus(data, block_info, chunk_coord, voxel_coord, out_data);
    process_tile_z_neg(data, block_info, chunk_coord, voxel_coord, out_data);
    //Logger::LogDebug(LOG_POS("process_block"), "Render Block (%i, %i, %i)", voxel_coord.x, voxel_coord.y, voxel_coord.z);

}

void CubeVoxelBuilder::process_tile(const std::vector<uint32_t>& data, glm::ivec2 block_info, glm::ivec3 chunk_coord, glm::ivec3 voxel_coord, int tile_index, Out_Data& out_data)
{
    int x = voxel_coord.x;
    int y = voxel_coord.y;
    int z = voxel_coord.z;
    float _sideLength = m_static_settings.SideLength[0];
    int vert_index = out_data.out_counts.x;
    /*
    // 0, 1: x-dir
    if (tile_index == 0) { // +x
        PUSH_VERTS(out_data.out_vertex, out_data.out_counts, glm::vec4(x * _sideLength + _sideLength, y * _sideLength, z * _sideLength, 0));
        PUSH_VERTS(out_data.out_vertex, out_data.out_counts, glm::vec4(x * _sideLength + _sideLength, y * _sideLength + _sideLength, z * _sideLength, 0));

        PUSH_VERTS(out_data.out_vertex, out_data.out_counts, glm::vec4(x * _sideLength + _sideLength, y * _sideLength + _sideLength, z * _sideLength + _sideLength, 0));
        PUSH_VERTS(out_data.out_vertex, out_data.out_counts, glm::vec4(x * _sideLength + _sideLength, y * _sideLength, z * _sideLength + _sideLength, 0));


        PUSH_TRIS(out_data.out_trianges, out_data.out_counts, vert_index + 0);
        PUSH_TRIS(out_data.out_trianges, out_data.out_counts, vert_index + 1);
        PUSH_TRIS(out_data.out_trianges, out_data.out_counts, vert_index + 2);

        PUSH_TRIS(out_data.out_trianges, out_data.out_counts, vert_index + 2);
        PUSH_TRIS(out_data.out_trianges, out_data.out_counts, vert_index + 3);
        PUSH_TRIS(out_data.out_trianges, out_data.out_counts, vert_index + 0);
    }
    else if (tile_index == 1) { // -x
        PUSH_VERTS(out_data.out_vertex, out_data.out_counts, glm::vec4(x * _sideLength, y * _sideLength, z * _sideLength, 0));
        PUSH_VERTS(out_data.out_vertex, out_data.out_counts, glm::vec4(x * _sideLength, y * _sideLength + _sideLength, z * _sideLength, 0));

        PUSH_VERTS(out_data.out_vertex, out_data.out_counts, glm::vec4(x * _sideLength, y * _sideLength + _sideLength, z * _sideLength + _sideLength, 0));
        PUSH_VERTS(out_data.out_vertex, out_data.out_counts, glm::vec4(x * _sideLength, y * _sideLength, z * _sideLength + _sideLength, 0));


        PUSH_TRIS(out_data.out_trianges, out_data.out_counts, vert_index + 0);
        PUSH_TRIS(out_data.out_trianges, out_data.out_counts, vert_index + 2);
        PUSH_TRIS(out_data.out_trianges, out_data.out_counts, vert_index + 1);

        PUSH_TRIS(out_data.out_trianges, out_data.out_counts, vert_index + 3);
        PUSH_TRIS(out_data.out_trianges, out_data.out_counts, vert_index + 2);
        PUSH_TRIS(out_data.out_trianges, out_data.out_counts, vert_index + 0);
    }

    // 2, 3: y-dir
    else if (tile_index == 2) { // +y
        PUSH_VERTS(out_data.out_vertex, out_data.out_counts, glm::vec4(x * _sideLength, y * _sideLength + _sideLength, z * _sideLength, 0));
        PUSH_VERTS(out_data.out_vertex, out_data.out_counts, glm::vec4(x * _sideLength, y * _sideLength + _sideLength, z * _sideLength + _sideLength, 0));

        PUSH_VERTS(out_data.out_vertex, out_data.out_counts, glm::vec4(x * _sideLength + _sideLength, y * _sideLength + _sideLength, z * _sideLength + _sideLength, 0));
        PUSH_VERTS(out_data.out_vertex, out_data.out_counts, glm::vec4(x * _sideLength + _sideLength, y * _sideLength + _sideLength, z * _sideLength, 0));


        PUSH_TRIS(out_data.out_trianges, out_data.out_counts, vert_index + 0);
        PUSH_TRIS(out_data.out_trianges, out_data.out_counts, vert_index + 1);
        PUSH_TRIS(out_data.out_trianges, out_data.out_counts, vert_index + 2);

        PUSH_TRIS(out_data.out_trianges, out_data.out_counts, vert_index + 2);
        PUSH_TRIS(out_data.out_trianges, out_data.out_counts, vert_index + 3);
        PUSH_TRIS(out_data.out_trianges, out_data.out_counts, vert_index + 0);
    }
    else if (tile_index == 3) { // -y
        PUSH_VERTS(out_data.out_vertex, out_data.out_counts, glm::vec4(x * _sideLength, y * _sideLength, z * _sideLength, 0));
        PUSH_VERTS(out_data.out_vertex, out_data.out_counts, glm::vec4(x * _sideLength, y * _sideLength, z * _sideLength + _sideLength, 0));

        PUSH_VERTS(out_data.out_vertex, out_data.out_counts, glm::vec4(x * _sideLength + _sideLength, y * _sideLength, z * _sideLength + _sideLength, 0));
        PUSH_VERTS(out_data.out_vertex, out_data.out_counts, glm::vec4(x * _sideLength + _sideLength, y * _sideLength, z * _sideLength, 0));


        PUSH_TRIS(out_data.out_trianges, out_data.out_counts, vert_index + 0);
        PUSH_TRIS(out_data.out_trianges, out_data.out_counts, vert_index + 2);
        PUSH_TRIS(out_data.out_trianges, out_data.out_counts, vert_index + 1);

        PUSH_TRIS(out_data.out_trianges, out_data.out_counts, vert_index + 3);
        PUSH_TRIS(out_data.out_trianges, out_data.out_counts, vert_index + 2);
        PUSH_TRIS(out_data.out_trianges, out_data.out_counts, vert_index + 0);
    }

    // 4, 5: z-dir
    else if (tile_index == 4) { // +z
        PUSH_VERTS(out_data.out_vertex, out_data.out_counts, glm::vec4(x * _sideLength, y * _sideLength, z * _sideLength + _sideLength, 0));
        PUSH_VERTS(out_data.out_vertex, out_data.out_counts, glm::vec4(x * _sideLength + _sideLength, y * _sideLength, z * _sideLength + _sideLength, 0));

        PUSH_VERTS(out_data.out_vertex, out_data.out_counts, glm::vec4(x * _sideLength + _sideLength, y * _sideLength + _sideLength, z * _sideLength + _sideLength, 0));
        PUSH_VERTS(out_data.out_vertex, out_data.out_counts, glm::vec4(x * _sideLength, y * _sideLength + _sideLength, z * _sideLength + _sideLength, 0));


        PUSH_TRIS(out_data.out_trianges, out_data.out_counts, vert_index + 0);
        PUSH_TRIS(out_data.out_trianges, out_data.out_counts, vert_index + 1);
        PUSH_TRIS(out_data.out_trianges, out_data.out_counts, vert_index + 2);

        PUSH_TRIS(out_data.out_trianges, out_data.out_counts, vert_index + 2);
        PUSH_TRIS(out_data.out_trianges, out_data.out_counts, vert_index + 3);
        PUSH_TRIS(out_data.out_trianges, out_data.out_counts, vert_index + 0);
    }
    else if (tile_index == 5) { // -z
        PUSH_VERTS(out_data.out_vertex, out_data.out_counts, glm::vec4(x * _sideLength, y * _sideLength, z * _sideLength, 0));
        PUSH_VERTS(out_data.out_vertex, out_data.out_counts, glm::vec4(x * _sideLength + _sideLength, y * _sideLength, z * _sideLength, 0));

        PUSH_VERTS(out_data.out_vertex, out_data.out_counts, glm::vec4(x * _sideLength + _sideLength, y * _sideLength + _sideLength, z * _sideLength, 0));
        PUSH_VERTS(out_data.out_vertex, out_data.out_counts, glm::vec4(x * _sideLength, y * _sideLength + _sideLength, z * _sideLength, 0));


        PUSH_TRIS(out_data.out_trianges, out_data.out_counts, vert_index + 0);
        PUSH_TRIS(out_data.out_trianges, out_data.out_counts, vert_index + 2);
        PUSH_TRIS(out_data.out_trianges, out_data.out_counts, vert_index + 1);

        PUSH_TRIS(out_data.out_trianges, out_data.out_counts, vert_index + 3);
        PUSH_TRIS(out_data.out_trianges, out_data.out_counts, vert_index + 2);
        PUSH_TRIS(out_data.out_trianges, out_data.out_counts, vert_index + 0);
    }*/

}

void CubeVoxelBuilder::process_tile_x_plus(const std::vector<uint32_t>& data, glm::ivec2 block_info, glm::ivec3 chunk_coord, glm::ivec3 voxel_coord, Out_Data& out_data)
{
    int x = voxel_coord.x;
    int y = voxel_coord.y;
    int z = voxel_coord.z;
    float _sideLength = m_static_settings.SideLength[0];
    int vert_index = out_data.out_counts.x;

    bool n_filled = neighboor_filed(data, voxel_coord, glm::ivec3(1, 0, 0));
    if (n_filled) {
        return;
    }

    int img_id = get_block_image_id(block_info, 0);

    if (img_id == -1) {
        // TODO: Shouldn't happen.
        return;
    }

    glm::vec4 normal = glm::vec4(1, 0, 0, 0);
    glm::vec2 uv = glm::vec2(img_id, 0);
    glm::vec4 v1 = glm::vec4(x * _sideLength + _sideLength, y * _sideLength, z * _sideLength, img_id);
    glm::vec4 v2 = glm::vec4(x * _sideLength + _sideLength, y * _sideLength + _sideLength, z * _sideLength, img_id);
    glm::vec4 v3 = glm::vec4(x * _sideLength + _sideLength, y * _sideLength + _sideLength, z * _sideLength + _sideLength, img_id);
    glm::vec4 v4 = glm::vec4(x * _sideLength + _sideLength, y * _sideLength, z * _sideLength + _sideLength, img_id);

#if USE_TRIANGLES
    PUSH_VERTS(out_data.out_vertex, out_data.out_normal, out_data.out_counts, v1, normal);
    PUSH_VERTS(out_data.out_vertex, out_data.out_normal, out_data.out_counts, v2, normal);

    PUSH_VERTS(out_data.out_vertex, out_data.out_normal, out_data.out_counts, v3, normal);
    PUSH_VERTS(out_data.out_vertex, out_data.out_normal, out_data.out_counts, v4, normal);


    PUSH_TRIS(out_data.out_trianges, out_data.out_counts, vert_index + 0);
    PUSH_TRIS(out_data.out_trianges, out_data.out_counts, vert_index + 1);
    PUSH_TRIS(out_data.out_trianges, out_data.out_counts, vert_index + 2);

    PUSH_TRIS(out_data.out_trianges, out_data.out_counts, vert_index + 2);
    PUSH_TRIS(out_data.out_trianges, out_data.out_counts, vert_index + 3);
    PUSH_TRIS(out_data.out_trianges, out_data.out_counts, vert_index + 0);
#else

    out_data.out_vertex.push_back(v1);
    out_data.out_vertex.push_back(v2);
    out_data.out_vertex.push_back(v3);

    out_data.out_vertex.push_back(v3);
    out_data.out_vertex.push_back(v4);
    out_data.out_vertex.push_back(v1);

    out_data.out_normal.push_back(normal);
    out_data.out_normal.push_back(normal);
    out_data.out_normal.push_back(normal);
    out_data.out_normal.push_back(normal);
    out_data.out_normal.push_back(normal); 
    out_data.out_normal.push_back(normal);

    out_data.out_uv.push_back(uv);
    out_data.out_uv.push_back(uv);
    out_data.out_uv.push_back(uv);
    out_data.out_uv.push_back(uv);
    out_data.out_uv.push_back(uv);
    out_data.out_uv.push_back(uv);

    out_data.out_counts.x = out_data.out_vertex.size();


#endif

    //Logger::LogDebug(LOG_POS("process_tile"), "Render Tile: %i", out_data.out_counts.x);
}

void CubeVoxelBuilder::process_tile_x_neg(const std::vector<uint32_t>& data, glm::ivec2 block_info, glm::ivec3 chunk_coord, glm::ivec3 voxel_coord, Out_Data& out_data)
{
    int x = voxel_coord.x;
    int y = voxel_coord.y;
    int z = voxel_coord.z;
    float _sideLength = m_static_settings.SideLength[0];
    int vert_index = out_data.out_counts.x;

    if (neighboor_filed(data, voxel_coord, glm::ivec3(-1, 0, 0))) {
        return;
    }

    int img_id = get_block_image_id(block_info, 0);

    glm::vec4 normal = glm::vec4(-1, 0, 0, 0);
    glm::vec2 uv = glm::vec2(img_id, 0);
    glm::vec4 v1 = glm::vec4(x * _sideLength, y * _sideLength, z * _sideLength, img_id);
    glm::vec4 v2 = glm::vec4(x * _sideLength, y * _sideLength + _sideLength, z * _sideLength, img_id);
    glm::vec4 v3 = glm::vec4(x * _sideLength, y * _sideLength + _sideLength, z * _sideLength + _sideLength, img_id);
    glm::vec4 v4 = glm::vec4(x * _sideLength, y * _sideLength, z * _sideLength + _sideLength, img_id);

#if USE_TRIANGLES
    PUSH_VERTS(out_data.out_vertex, out_data.out_normal, out_data.out_counts, v1, normal);
    PUSH_VERTS(out_data.out_vertex, out_data.out_normal, out_data.out_counts, v2, normal);

    PUSH_VERTS(out_data.out_vertex, out_data.out_normal, out_data.out_counts, v3, normal);
    PUSH_VERTS(out_data.out_vertex, out_data.out_normal, out_data.out_counts, v4, normal);


    PUSH_TRIS(out_data.out_trianges, out_data.out_counts, vert_index + 0);
    PUSH_TRIS(out_data.out_trianges, out_data.out_counts, vert_index + 2);
    PUSH_TRIS(out_data.out_trianges, out_data.out_counts, vert_index + 1);

    PUSH_TRIS(out_data.out_trianges, out_data.out_counts, vert_index + 3);
    PUSH_TRIS(out_data.out_trianges, out_data.out_counts, vert_index + 2);
    PUSH_TRIS(out_data.out_trianges, out_data.out_counts, vert_index + 0);
#else

    out_data.out_vertex.push_back(v1);
    out_data.out_vertex.push_back(v3);
    out_data.out_vertex.push_back(v2);

    out_data.out_vertex.push_back(v4);
    out_data.out_vertex.push_back(v3);
    out_data.out_vertex.push_back(v1);

    out_data.out_normal.push_back(normal);
    out_data.out_normal.push_back(normal);
    out_data.out_normal.push_back(normal);
    out_data.out_normal.push_back(normal);
    out_data.out_normal.push_back(normal);
    out_data.out_normal.push_back(normal);

    out_data.out_uv.push_back(uv);
    out_data.out_uv.push_back(uv);
    out_data.out_uv.push_back(uv);
    out_data.out_uv.push_back(uv);
    out_data.out_uv.push_back(uv);
    out_data.out_uv.push_back(uv);

    out_data.out_counts.x = out_data.out_vertex.size();

#endif

    //Logger::LogDebug(LOG_POS("process_tile"), "Render Tile: %i", out_data.out_counts.x);
}

void CubeVoxelBuilder::process_tile_y_plus(const std::vector<uint32_t>& data, glm::ivec2 block_info, glm::ivec3 chunk_coord, glm::ivec3 voxel_coord, Out_Data& out_data)
{
    int x = voxel_coord.x;
    int y = voxel_coord.y;
    int z = voxel_coord.z;
    float _sideLength = m_static_settings.SideLength[0];
    int vert_index = out_data.out_counts.x;

    if (neighboor_filed(data, voxel_coord, glm::ivec3(0, 1, 0))) {
        return;
    }

    int img_id = get_block_image_id(block_info, 0);

    glm::vec4 normal = glm::vec4(0, 1, 0, 0);
    glm::vec2 uv = glm::vec2(img_id, 1);
    glm::vec4 v1 = glm::vec4(x * _sideLength, y * _sideLength + _sideLength, z * _sideLength, img_id);
    glm::vec4 v2 = glm::vec4(x * _sideLength, y * _sideLength + _sideLength, z * _sideLength + _sideLength, img_id);
    glm::vec4 v3 = glm::vec4(x * _sideLength + _sideLength, y * _sideLength + _sideLength, z * _sideLength + _sideLength, img_id);
    glm::vec4 v4 = glm::vec4(x * _sideLength + _sideLength, y * _sideLength + _sideLength, z * _sideLength, img_id);

#if USE_TRIANGLES
    PUSH_VERTS(out_data.out_vertex, out_data.out_normal, out_data.out_counts, v1, normal);
    PUSH_VERTS(out_data.out_vertex, out_data.out_normal, out_data.out_counts, v2, normal);

    PUSH_VERTS(out_data.out_vertex, out_data.out_normal, out_data.out_counts, v3, normal);
    PUSH_VERTS(out_data.out_vertex, out_data.out_normal, out_data.out_counts, v4, normal);


    PUSH_TRIS(out_data.out_trianges, out_data.out_counts, vert_index + 0);
    PUSH_TRIS(out_data.out_trianges, out_data.out_counts, vert_index + 1);
    PUSH_TRIS(out_data.out_trianges, out_data.out_counts, vert_index + 2);

    PUSH_TRIS(out_data.out_trianges, out_data.out_counts, vert_index + 2);
    PUSH_TRIS(out_data.out_trianges, out_data.out_counts, vert_index + 3);
    PUSH_TRIS(out_data.out_trianges, out_data.out_counts, vert_index + 0);
#else

    out_data.out_vertex.push_back(v1);
    out_data.out_vertex.push_back(v2);
    out_data.out_vertex.push_back(v3);

    out_data.out_vertex.push_back(v3);
    out_data.out_vertex.push_back(v4);
    out_data.out_vertex.push_back(v1);

    out_data.out_normal.push_back(normal);
    out_data.out_normal.push_back(normal);
    out_data.out_normal.push_back(normal);
    out_data.out_normal.push_back(normal);
    out_data.out_normal.push_back(normal);
    out_data.out_normal.push_back(normal);

    out_data.out_uv.push_back(uv);
    out_data.out_uv.push_back(uv);
    out_data.out_uv.push_back(uv);
    out_data.out_uv.push_back(uv);
    out_data.out_uv.push_back(uv);
    out_data.out_uv.push_back(uv);

    out_data.out_counts.x = out_data.out_vertex.size();

#endif

    //Logger::LogDebug(LOG_POS("process_tile"), "Render Tile: %i", out_data.out_counts.x);
}

void CubeVoxelBuilder::process_tile_y_neg(const std::vector<uint32_t>& data, glm::ivec2 block_info, glm::ivec3 chunk_coord, glm::ivec3 voxel_coord, Out_Data& out_data)
{
    int x = voxel_coord.x;
    int y = voxel_coord.y;
    int z = voxel_coord.z;
    float _sideLength = m_static_settings.SideLength[0];
    int vert_index = out_data.out_counts.x;

    if (neighboor_filed(data, voxel_coord, glm::ivec3(0, -1, 0))) {
        return;
    }

    int img_id = get_block_image_id(block_info, 0);

    glm::vec4 normal = glm::vec4(0, -1, 0, 0);
    glm::vec2 uv = glm::vec2(img_id, 1);
    glm::vec4 v1 = glm::vec4(x * _sideLength, y * _sideLength, z * _sideLength, img_id);
    glm::vec4 v2 = glm::vec4(x * _sideLength, y * _sideLength, z * _sideLength + _sideLength, img_id);
    glm::vec4 v3 = glm::vec4(x * _sideLength + _sideLength, y * _sideLength, z * _sideLength + _sideLength, img_id);
    glm::vec4 v4 = glm::vec4(x * _sideLength + _sideLength, y * _sideLength, z * _sideLength, img_id);

#if USE_TRIANGLES
    PUSH_VERTS(out_data.out_vertex, out_data.out_normal, out_data.out_counts, v1, normal);
    PUSH_VERTS(out_data.out_vertex, out_data.out_normal, out_data.out_counts, v2, normal);

    PUSH_VERTS(out_data.out_vertex, out_data.out_normal, out_data.out_counts, v3, normal);
    PUSH_VERTS(out_data.out_vertex, out_data.out_normal, out_data.out_counts, v4, normal);


    PUSH_TRIS(out_data.out_trianges, out_data.out_counts, vert_index + 0);
    PUSH_TRIS(out_data.out_trianges, out_data.out_counts, vert_index + 2);
    PUSH_TRIS(out_data.out_trianges, out_data.out_counts, vert_index + 1);

    PUSH_TRIS(out_data.out_trianges, out_data.out_counts, vert_index + 3);
    PUSH_TRIS(out_data.out_trianges, out_data.out_counts, vert_index + 2);
    PUSH_TRIS(out_data.out_trianges, out_data.out_counts, vert_index + 0);
#else

    out_data.out_vertex.push_back(v1);
    out_data.out_vertex.push_back(v3);
    out_data.out_vertex.push_back(v2);

    out_data.out_vertex.push_back(v4);
    out_data.out_vertex.push_back(v3);
    out_data.out_vertex.push_back(v1);

    out_data.out_normal.push_back(normal);
    out_data.out_normal.push_back(normal);
    out_data.out_normal.push_back(normal);
    out_data.out_normal.push_back(normal);
    out_data.out_normal.push_back(normal);
    out_data.out_normal.push_back(normal);

    out_data.out_uv.push_back(uv);
    out_data.out_uv.push_back(uv);
    out_data.out_uv.push_back(uv);
    out_data.out_uv.push_back(uv);
    out_data.out_uv.push_back(uv);
    out_data.out_uv.push_back(uv);

    out_data.out_counts.x = out_data.out_vertex.size();

#endif

    //Logger::LogDebug(LOG_POS("process_tile"), "Render Tile: %i", out_data.out_counts.x);
}

void CubeVoxelBuilder::process_tile_z_plus(const std::vector<uint32_t>& data, glm::ivec2 block_info, glm::ivec3 chunk_coord, glm::ivec3 voxel_coord, Out_Data& out_data)
{
    int x = voxel_coord.x;
    int y = voxel_coord.y;
    int z = voxel_coord.z;
    float _sideLength = m_static_settings.SideLength[0];
    int vert_index = out_data.out_counts.x;

    if (neighboor_filed(data, voxel_coord, glm::ivec3(0, 0, 1))) {
        return;
    }

    int img_id = get_block_image_id(block_info, 0);

    glm::vec4 normal = glm::vec4(0, 0, 1, 0);
    glm::vec2 uv = glm::vec2(img_id, 2);
    glm::vec4 v1 = glm::vec4(x * _sideLength, y * _sideLength, z * _sideLength + _sideLength, img_id);
    glm::vec4 v2 = glm::vec4(x * _sideLength + _sideLength, y * _sideLength, z * _sideLength + _sideLength, img_id);
    glm::vec4 v3 = glm::vec4(x * _sideLength + _sideLength, y * _sideLength + _sideLength, z * _sideLength + _sideLength, img_id);
    glm::vec4 v4 = glm::vec4(x * _sideLength, y * _sideLength + _sideLength, z * _sideLength + _sideLength, img_id);

#if USE_TRIANGLES
    PUSH_VERTS(out_data.out_vertex, out_data.out_normal, out_data.out_counts, v1, normal);
    PUSH_VERTS(out_data.out_vertex, out_data.out_normal, out_data.out_counts, v2, normal);

    PUSH_VERTS(out_data.out_vertex, out_data.out_normal, out_data.out_counts, v3, normal);
    PUSH_VERTS(out_data.out_vertex, out_data.out_normal, out_data.out_counts, v4, normal);


    PUSH_TRIS(out_data.out_trianges, out_data.out_counts, vert_index + 0);
    PUSH_TRIS(out_data.out_trianges, out_data.out_counts, vert_index + 1);
    PUSH_TRIS(out_data.out_trianges, out_data.out_counts, vert_index + 2);

    PUSH_TRIS(out_data.out_trianges, out_data.out_counts, vert_index + 2);
    PUSH_TRIS(out_data.out_trianges, out_data.out_counts, vert_index + 3);
    PUSH_TRIS(out_data.out_trianges, out_data.out_counts, vert_index + 0);
#else

    out_data.out_vertex.push_back(v1);
    out_data.out_vertex.push_back(v2);
    out_data.out_vertex.push_back(v3);

    out_data.out_vertex.push_back(v3);
    out_data.out_vertex.push_back(v4);
    out_data.out_vertex.push_back(v1);

    out_data.out_normal.push_back(normal);
    out_data.out_normal.push_back(normal);
    out_data.out_normal.push_back(normal);
    out_data.out_normal.push_back(normal);
    out_data.out_normal.push_back(normal);
    out_data.out_normal.push_back(normal);

    out_data.out_uv.push_back(uv);
    out_data.out_uv.push_back(uv);
    out_data.out_uv.push_back(uv);
    out_data.out_uv.push_back(uv);
    out_data.out_uv.push_back(uv);
    out_data.out_uv.push_back(uv);

    out_data.out_counts.x = out_data.out_vertex.size();

#endif

    //Logger::LogDebug(LOG_POS("process_tile"), "Render Tile: %i", out_data.out_counts.x);
}

void CubeVoxelBuilder::process_tile_z_neg(const std::vector<uint32_t>& data, glm::ivec2 block_info, glm::ivec3 chunk_coord, glm::ivec3 voxel_coord, Out_Data& out_data)
{
    int x = voxel_coord.x;
    int y = voxel_coord.y;
    int z = voxel_coord.z;
    float _sideLength = m_static_settings.SideLength[0];
    int vert_index = out_data.out_counts.x;

    if (neighboor_filed(data, voxel_coord, glm::ivec3(0, 0, -1))) {
        return;
    }

    int img_id = get_block_image_id(block_info, 0);

    glm::vec4 normal = glm::vec4(0, 0, -1, 0);
    glm::vec2 uv = glm::vec2(img_id, 2);
    glm::vec4 v1 = glm::vec4(x * _sideLength, y * _sideLength, z * _sideLength, img_id);
    glm::vec4 v2 = glm::vec4(x * _sideLength + _sideLength, y * _sideLength, z * _sideLength, img_id);
    glm::vec4 v3 = glm::vec4(x * _sideLength + _sideLength, y * _sideLength + _sideLength, z * _sideLength, img_id);
    glm::vec4 v4 = glm::vec4(x * _sideLength, y * _sideLength + _sideLength, z * _sideLength, img_id);

#if USE_TRIANGLES
    PUSH_VERTS(out_data.out_vertex, out_data.out_normal, out_data.out_counts, v1, normal);
    PUSH_VERTS(out_data.out_vertex, out_data.out_normal, out_data.out_counts, v2, normal);

    PUSH_VERTS(out_data.out_vertex, out_data.out_normal, out_data.out_counts, v3, normal);
    PUSH_VERTS(out_data.out_vertex, out_data.out_normal, out_data.out_counts, v4, normal);


    PUSH_TRIS(out_data.out_trianges, out_data.out_counts, vert_index + 0);
    PUSH_TRIS(out_data.out_trianges, out_data.out_counts, vert_index + 2);
    PUSH_TRIS(out_data.out_trianges, out_data.out_counts, vert_index + 1);

    PUSH_TRIS(out_data.out_trianges, out_data.out_counts, vert_index + 3);
    PUSH_TRIS(out_data.out_trianges, out_data.out_counts, vert_index + 2);
    PUSH_TRIS(out_data.out_trianges, out_data.out_counts, vert_index + 0);
#else

    out_data.out_vertex.push_back(v1);
    out_data.out_vertex.push_back(v3);
    out_data.out_vertex.push_back(v2);

    out_data.out_vertex.push_back(v4);
    out_data.out_vertex.push_back(v3);
    out_data.out_vertex.push_back(v1);

    out_data.out_normal.push_back(normal);
    out_data.out_normal.push_back(normal);
    out_data.out_normal.push_back(normal);
    out_data.out_normal.push_back(normal);
    out_data.out_normal.push_back(normal);
    out_data.out_normal.push_back(normal);

    out_data.out_uv.push_back(uv);
    out_data.out_uv.push_back(uv);
    out_data.out_uv.push_back(uv);
    out_data.out_uv.push_back(uv);
    out_data.out_uv.push_back(uv);
    out_data.out_uv.push_back(uv);

    out_data.out_counts.x = out_data.out_vertex.size();

#endif

    //Logger::LogDebug(LOG_POS("process_tile"), "Render Tile: %i", out_data.out_counts.x);
}

bool CubeVoxelBuilder::neighboor_filed(const std::vector<uint32_t>& data, glm::ivec3 this_voxel, glm::ivec3 neightboor_offset)
{
    glm::ivec3 other_voxel = this_voxel + neightboor_offset;
    int v_idx = C_3D_to_1D(other_voxel.x + GRID_OFFSET, other_voxel.y + GRID_OFFSET, other_voxel.z + GRID_OFFSET, m_static_settings.ChunkSize.x + GRID_PADDING, m_static_settings.ChunkSize.y + GRID_PADDING);
    uint32_t raw_block_data = data[v_idx];
    glm::uvec2 block_info = glm::uvec2(Get_Block_Type(raw_block_data), Get_Block_Orientation(raw_block_data));
    return render_enabled(block_info);
}


uint32_t CubeVoxelBuilder::New_Block_Data(uint32_t type, uint8_t orientation)
{
    uint32_t data = 0;
    Set_Block_Type(data, type);
    Set_Block_Orientation(data, orientation);
    return data;
}

uint32_t CubeVoxelBuilder::Set_Block_Orientation(uint32_t& init_value, uint8_t val)
{
    uint32_t orien = 0x000000ff & val;
    init_value = init_value & 0xffffff00;

    init_value |= orien;

    return init_value;
}

uint32_t CubeVoxelBuilder::Set_Block_Type(uint32_t& init_value, uint32_t val)
{
    uint32_t orien = 0x000000ff & init_value;
    init_value = (val << 8) & 0xffffff00;

    init_value |= orien;

    return init_value;
}

uint32_t CubeVoxelBuilder::Get_Block_Type(uint32_t block_value)
{
    uint32_t ret = (block_value & 0xffffff00) >> 8;
    return ret;
}

uint8_t CubeVoxelBuilder::Get_Block_Orientation(uint32_t block_value)
{
    return (block_value & 0x000000ff);
}

int CubeVoxelBuilder::get_block_image_id(glm::ivec2 info, int tile_id)
{
    return m_request_tile_tex_delegate(info.x, tile_id, info.y);
}

bool CubeVoxelBuilder::render_enabled(glm::ivec2 info)
{
    if (info.x == 0)
    {
        return false;
    }

    return m_can_render_delegate(info.x);
}
