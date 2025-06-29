#include "CubeVoxelBuilder.h"

#include "StructureDataStorage.h"

#define MAX_STORED_CHUNKS 100

#define GRID_PADDING 2
#define GRID_OFFSET 1

using namespace VoxelEngine;

#define PUSH_VERTS(v_list, counts, v)  \
do {                                   \
    v_list[counts.x] = v;              \
    counts.x++;                        \
}while(true)


#define PUSH_TRIS(t_list, counts, t)   \
do {                                   \
    t_list[counts.y] = t;              \
    counts.y++;                        \
}while(true)


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

void CubeVoxelBuilder::Init(ChunkSettings* settings)
{
    Settings p_settings = *settings->GetSettings();

    float vpm = p_settings.getFloat("voxelsPerMeter");

    m_static_settings.VoxelsPerMeter = glm::vec4(vpm, vpm, vpm, vpm);
    m_static_settings.ChunkMeterSize.x = p_settings.getInt("chunkMeterSizeX");
    m_static_settings.ChunkMeterSize.y = p_settings.getInt("chunkMeterSizeY");
    m_static_settings.ChunkMeterSize.z = p_settings.getInt("chunkMeterSizeZ");

    CalculateVariables();

    m_structure_data = new StructureDataStorage(
        m_static_settings.ChunkSize.x, m_static_settings.ChunkSize.y, m_static_settings.ChunkSize.z,
        MAX_STORED_CHUNKS
    );
}

glm::dvec4 CubeVoxelBuilder::Render(
	ChunkRenderOptions* options,
	glm::vec4* out_vertex,
	glm::vec4* out_normal,
	glm::vec2* out_uv,
	unsigned int* out_trianges,
	glm::ivec4 counts)
{
    Out_Data out_data{};
    out_data.out_vertex = out_vertex;
    out_data.out_normal = out_normal;
    out_data.out_uv = out_uv;
    out_data.out_trianges = out_trianges;
    out_data.out_counts = counts;

    glm::ivec3 chunk = options->locations[0];

    if (!m_structure_data->Has_Chunk(chunk)) {
        return glm::dvec4();
    }

    uint32_t* data = m_structure_data->Get_Data_ptr(chunk);

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

void CubeVoxelBuilder::process_block(uint32_t* data, glm::ivec3 chunk_coord, glm::ivec3 voxel_coord, Out_Data& out_data)
{
    int v_idx = C_3D_to_1D(voxel_coord.x, voxel_coord.y, voxel_coord.z, m_static_settings.ChunkSize.x + GRID_PADDING, m_static_settings.ChunkSize.y + GRID_PADDING);
    uint32_t raw_block_data = data[v_idx];
    glm::uvec2 block_info = glm::uvec2(get_block_type(raw_block_data), get_block_orientation(raw_block_data));
    
    if (block_info.x == 0) {
        return;
    }

    for (int i = 0; i < 6; i++) {
        process_tile(data, block_info, chunk_coord, voxel_coord, i, out_data);
    }
}

void CubeVoxelBuilder::process_tile(uint32_t* data, glm::ivec2 block_info, glm::ivec3 chunk_coord, glm::ivec3 voxel_coord, int tile_index, Out_Data& out_data)
{
    int x = voxel_coord.x;
    int y = voxel_coord.y;
    int z = voxel_coord.z;
    float _sideLength = m_static_settings.SideLength[0];
    int vert_index = out_data.out_counts.x;

    // 0, 1: x-dir
    if (tile_index == 0) { // +x
        PUSH_VERTS(out_data.out_vertex, out_data.out_counts, glm::vec4());
        PUSH_VERTS(out_data.out_vertex, out_data.out_counts, glm::vec4());

        PUSH_VERTS(out_data.out_vertex, out_data.out_counts, glm::vec4());
        PUSH_VERTS(out_data.out_vertex, out_data.out_counts, glm::vec4());


        PUSH_TRIS(out_data.out_trianges, out_data.out_counts, vert_index);
        PUSH_TRIS(out_data.out_trianges, out_data.out_counts, vert_index);
        PUSH_TRIS(out_data.out_trianges, out_data.out_counts, vert_index);

        PUSH_TRIS(out_data.out_trianges, out_data.out_counts, vert_index);
        PUSH_TRIS(out_data.out_trianges, out_data.out_counts, vert_index);
        PUSH_TRIS(out_data.out_trianges, out_data.out_counts, vert_index);
    }
    else if (tile_index == 1) { // -x
        PUSH_VERTS(out_data.out_vertex, out_data.out_counts, glm::vec4());
        PUSH_VERTS(out_data.out_vertex, out_data.out_counts, glm::vec4());

        PUSH_VERTS(out_data.out_vertex, out_data.out_counts, glm::vec4());
        PUSH_VERTS(out_data.out_vertex, out_data.out_counts, glm::vec4());


        PUSH_TRIS(out_data.out_trianges, out_data.out_counts, vert_index);
        PUSH_TRIS(out_data.out_trianges, out_data.out_counts, vert_index);
        PUSH_TRIS(out_data.out_trianges, out_data.out_counts, vert_index);

        PUSH_TRIS(out_data.out_trianges, out_data.out_counts, vert_index);
        PUSH_TRIS(out_data.out_trianges, out_data.out_counts, vert_index);
        PUSH_TRIS(out_data.out_trianges, out_data.out_counts, vert_index);
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
        PUSH_VERTS(out_data.out_vertex, out_data.out_counts, glm::vec4());
        PUSH_VERTS(out_data.out_vertex, out_data.out_counts, glm::vec4());

        PUSH_VERTS(out_data.out_vertex, out_data.out_counts, glm::vec4());
        PUSH_VERTS(out_data.out_vertex, out_data.out_counts, glm::vec4());


        PUSH_TRIS(out_data.out_trianges, out_data.out_counts, vert_index);
        PUSH_TRIS(out_data.out_trianges, out_data.out_counts, vert_index);
        PUSH_TRIS(out_data.out_trianges, out_data.out_counts, vert_index);

        PUSH_TRIS(out_data.out_trianges, out_data.out_counts, vert_index);
        PUSH_TRIS(out_data.out_trianges, out_data.out_counts, vert_index);
        PUSH_TRIS(out_data.out_trianges, out_data.out_counts, vert_index);
    }

    // 4, 5: z-dir
    else if (tile_index == 4) { // +z
        PUSH_VERTS(out_data.out_vertex, out_data.out_counts, glm::vec4());
        PUSH_VERTS(out_data.out_vertex, out_data.out_counts, glm::vec4());

        PUSH_VERTS(out_data.out_vertex, out_data.out_counts, glm::vec4());
        PUSH_VERTS(out_data.out_vertex, out_data.out_counts, glm::vec4());


        PUSH_TRIS(out_data.out_trianges, out_data.out_counts, vert_index);
        PUSH_TRIS(out_data.out_trianges, out_data.out_counts, vert_index);
        PUSH_TRIS(out_data.out_trianges, out_data.out_counts, vert_index);

        PUSH_TRIS(out_data.out_trianges, out_data.out_counts, vert_index);
        PUSH_TRIS(out_data.out_trianges, out_data.out_counts, vert_index);
        PUSH_TRIS(out_data.out_trianges, out_data.out_counts, vert_index);
    }
    else if (tile_index == 5) { // -z
        PUSH_VERTS(out_data.out_vertex, out_data.out_counts, glm::vec4());
        PUSH_VERTS(out_data.out_vertex, out_data.out_counts, glm::vec4());

        PUSH_VERTS(out_data.out_vertex, out_data.out_counts, glm::vec4());
        PUSH_VERTS(out_data.out_vertex, out_data.out_counts, glm::vec4());


        PUSH_TRIS(out_data.out_trianges, out_data.out_counts, vert_index);
        PUSH_TRIS(out_data.out_trianges, out_data.out_counts, vert_index);
        PUSH_TRIS(out_data.out_trianges, out_data.out_counts, vert_index);

        PUSH_TRIS(out_data.out_trianges, out_data.out_counts, vert_index);
        PUSH_TRIS(out_data.out_trianges, out_data.out_counts, vert_index);
        PUSH_TRIS(out_data.out_trianges, out_data.out_counts, vert_index);
    }


}

uint16_t CubeVoxelBuilder::get_block_type(uint32_t block_value)
{
    return block_value;
}

uint8_t VoxelEngine::CubeVoxelBuilder::get_block_orientation(uint32_t block_value)
{
    return 1;
}
