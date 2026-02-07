#include "ISO_Sampler.h"

#include "Logger.h"

#include "HeightmapGenerator.h"
#include "TerrainModifications.h"

#define BASE_RESOURCE_DIR "compute::voxelEngine::"

#define GRID_PADDING 2
#define GRID_OFFSET 1

ISO_Sampler::ISO_Sampler(
	IComputeController* controller, 
	int size_x, int size_y, int size_z, 
	std::string extension, IComputeProgram::FileType type,
	std::shared_ptr<HeightmapGenerator> heightmap_gen, std::shared_ptr<TerrainModifications> terrain_mods
) :
	m_controller{ controller },
	m_size_x{ size_x }, m_size_z{ size_z },
	m_heightmap_gen {heightmap_gen}, m_terrain_mods{terrain_mods}

{

	int max_elements = (size_x + GRID_PADDING) * (size_y + GRID_PADDING) * (size_z + GRID_PADDING);

	m_program_iso_sample = new VoxelComputeProgram(m_controller, BASE_RESOURCE_DIR + PROGRAM_ISO_SAMPLE + extension, m_WorkGroups, type);

	in_voxel_locations_data = m_controller->NewReadBuffer(max_elements, sizeof(Voxel_Location));
	out_iso_data = m_controller->NewReadWriteBuffer(max_elements, sizeof(Voxel_Location));

	
	Logger::LogInfo(LOG_POS("ISO_Sampler"), "ISO Sampler initialized.");
}

void ISO_Sampler::Finalize(IComputeBuffer* static_settings)
{

	m_in_static_settings_buffer = static_settings;

	m_program_iso_sample->AddBuffer(0, m_in_static_settings_buffer);
	m_program_iso_sample->AddBuffer(1, in_voxel_locations_data);
	m_program_iso_sample->AddBuffer(2, m_heightmap_gen->Height_Data());
	m_program_iso_sample->AddBuffer(3, m_heightmap_gen->Height_Extended_Data());
	m_program_iso_sample->AddBuffer(4, m_terrain_mods->Modification_Data());
	m_program_iso_sample->AddBuffer(5, out_iso_data);

	m_program_iso_sample->Finalize();
}

float ISO_Sampler::Get_ISO(glm::ivec3 chunk, glm::ivec3 voxel)
{
	float res = 0.0;
	std::vector<glm::ivec3> voxels;
	voxels.push_back(voxel);
	std::vector<float> res_arr = Get_ISO(chunk, voxels);
	if (res_arr.size() > 0) {
		res = res_arr[0];
	}
	return res;
}

std::vector<float> ISO_Sampler::Get_ISO(const glm::ivec3& chunk, const std::vector<glm::ivec3>& voxels)
{
	std::vector<Voxel_Location> locations;
	locations.reserve(voxels.size());

	int col_data_offset = m_heightmap_gen->Get_Column_Data_Offset(glm::ivec2(chunk.x, chunk.z));
	int chunk_data_offset = m_terrain_mods->Get_Chunk_Data_Offset(chunk);

	for (const auto& v : voxels) 
	{
		Voxel_Location loc{};
		loc.chunk = glm::ivec4(chunk, 0.0);
		loc.voxel = glm::ivec4(v.x + GRID_OFFSET, v.y + GRID_OFFSET, v.z + GRID_OFFSET, 0);
		loc.int_data_1 = glm::ivec4(
			col_data_offset,
			chunk_data_offset,
			0, 0
		);
		locations.push_back(loc);
	}

	return Get_ISO(chunk, locations);
}

std::vector<float> ISO_Sampler::Get_ISO(const glm::ivec3& chunk, const std::vector<Voxel_Location>& voxels) 
{
	std::vector<float> res;
	res.reserve(voxels.size());

	set_locations(voxels);
	execute(voxels.size());
	std::vector<glm::fvec4> values = get_iso_data(voxels.size());

	for (const auto& v : values)
	{
		res.push_back(v.x);
	}
	return res;
}

void ISO_Sampler::set_locations(const std::vector<Voxel_Location>& voxels)
{
	in_voxel_locations_data->SetData((Voxel_Location*)voxels.data(), voxels.size() * sizeof(Voxel_Location));
}

void ISO_Sampler::execute(int elements)
{
	m_program_iso_sample->Execute(elements, 0, 0);
}

std::vector<glm::fvec4> ISO_Sampler::get_iso_data(int elements)
{
	glm::fvec4* data = new glm::fvec4[elements];
	char* data_ptr = (char*)data;
	size_t data_size = elements * sizeof(float) * 4;
	out_iso_data->GetData(data, data_size);
	std::vector<glm::fvec4> res((glm::fvec4*)data_ptr, (glm::fvec4*)(data_ptr + data_size));
	delete[] data;
	return res;
}
