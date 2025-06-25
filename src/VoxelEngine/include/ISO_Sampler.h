#pragma once

#include "dynamic_compute.h"

#include "VoxelComputeProgram.h"

#include <vector>
#include <unordered_map>

using namespace VoxelEngine;

class HeightmapGenerator;
class TerrainModifications;

class ISO_Sampler {
public:

	struct Voxel_Location {
		glm::ivec4 chunk;
		glm::ivec4 voxel;
		glm::ivec4 int_data_1;
	};

	ISO_Sampler(IComputeController* controller,
		int size_x, int size_y, int size_z,
		std::string extension, IComputeProgram::FileType type,
		HeightmapGenerator* heightmap_gen, TerrainModifications* terrain_mods);

	void Finalize(IComputeBuffer* static_settings);

	std::vector<float> Get_ISO(glm::ivec3 chunk, std::vector<glm::ivec3> voxels);

	std::vector<float> Get_ISO(glm::ivec3 chunk, std::vector<Voxel_Location> voxels);

private:

	IComputeController* m_controller{ nullptr };

	HeightmapGenerator* m_heightmap_gen{ nullptr };
	TerrainModifications* m_terrain_mods{ nullptr };


	int m_size_x, m_size_z;
	int m_WorkGroups{ 16 };

	const std::string PROGRAM_ISO_SAMPLE = "iso_sampler";
	VoxelComputeProgram* m_program_iso_sample{ nullptr };

	IComputeBuffer* m_in_static_settings_buffer{ nullptr };

	IComputeBuffer* in_voxel_locations_data{ nullptr };
	IComputeBuffer* out_iso_data{ nullptr };

	void set_locations(std::vector<Voxel_Location> voxels);
	void execute(int elements);
	std::vector<glm::fvec4> get_iso_data(int elements);
};