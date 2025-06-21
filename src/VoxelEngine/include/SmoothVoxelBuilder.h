#pragma once

#include "stdafx.h"
#include "ForwardDeclarations.h"
#include "shared_structures.h"
#include "IVoxelBuilder_private.h"
#include "dynamic_compute.h"

#include "Utilities.h"

#include "VoxelComputeProgram.h"
#include "HeightmapGenerator.h"
#include "TerrainModifications.h"

#include <queue>

#define VOXEL_RUNTIME_VULKAN 1
#define VOXEL_RUNTIME_OPENCL 2


#define VOXEL_RUNTIME VOXEL_RUNTIME_OPENCL

using namespace DynamicCompute::Compute;

#define DATA_SIZE 32

namespace VoxelEngine {

	class SmoothVoxelBuilder : public IVoxelBuilder_private {
	public:

		void Init(ChunkSettings* settings);

		//void SetBlockTypes(BlockType* blockTypeList, Rect* AtlasUvs);

		glm::dvec4 Render(ChunkRenderOptions* options);

		glm::dvec4 Generate(ChunkGenerationOptions* options);

		std::vector<glm::ivec4> GetSize();

		void Extract(glm::vec4* out_vertex, glm::vec4* out_normal, glm::vec4* out_mat, unsigned int* out_trianges, glm::ivec4 counts) override;

		void Extract(IComputeBuffer* out_vertex, IComputeBuffer* out_normal, IComputeBuffer* out_mat, IComputeBuffer* out_trianges, glm::ivec4 counts) override;

		void ReleaseHeightmap(int x, int z);

		void SetHeightmapsEnabled(bool enabled);

		IComputeBuffer* Get_Tranfer_Buffer(int size, int stride, bool external);

		IComputeController* Get_Compute_Controller();

		HeightmapGenerator* Get_Heightmap_Generator() { return m_HeightmapGenerator; }

		TerrainModifications* Get_Terrain_Modifications() { return m_Terrain_Modifications; }

		//void SetBlock(int x, int y, int z, Block block);

		//Block GetBlock(int x, int y, int z);

		//void SetSurroundingChunks();

		void Dispose();

		struct Run_Settings {
			glm::ivec4 Location;
			glm::ivec4 int_data_1;
		};

	private:

		/*struct Static_Settings {
			alignas(16) float VoxelsPerMeter;
			alignas(16) int ChunkMeterSizeX;
			alignas(16) int ChunkMeterSizeY;
			alignas(16) int ChunkMeterSizeZ;
			alignas(16) int ChunkSizeX;
			alignas(16) int ChunkSizeY;
			alignas(16) int ChunkSizeZ;
			alignas(16) int FullChunkSize;
			alignas(16) float skipDist;
			alignas(16) float half;
			alignas(16) float xSideLength;
			alignas(16) float ySideLength;
			alignas(16) float zSideLength;
		};*/

		struct Static_Settings {
			glm::vec4 VoxelsPerMeter;
			glm::ivec4 ChunkMeterSize;
			glm::ivec4 ChunkSize;
			glm::ivec4 FullChunkSize;
			glm::vec4 skipDist;
			glm::vec4 half_;
			glm::vec4 SideLength;
			glm::ivec4 batches;
		};

		/*struct Run_Settings {
			alignas(16) int X;
			alignas(16) int Y;
			alignas(16) int Z;
		};*/

		

		struct ISO_Material {
			alignas(16) glm::vec4 final_iso;
			alignas(16) glm::ivec4 material_types;
			alignas(16) glm::vec4 material_ratios;
		};

		// Initialization

		void CalculateVariables();

		void InitializeComputePrograms();

		void CreateComputeBuffers();

		void FinalizePrograms();

		// Noise Generation

		double GenerateHeightmap(int group, Run_Settings* group_start);

		double GenerateHeightmap_do_cache(int group, Run_Settings* group_start);
		double GenerateHeightmap_dont_cache(int group, Run_Settings* group_start);


		double GenerateISOField(int group, Run_Settings* group_set);

		double GenerateMaterialField(int group, Run_Settings* group_set);

		double AssembleUnifiedField(int group, Run_Settings* group_set);


		// Chunk Rendering

		double Construct(int group, Run_Settings* group_set);

		glm::dvec4 DoRender();

		glm::ivec4 ProcessVoxel(glm::vec4* vertlist, int* cubeIndex, int index, glm::ivec4 counts);

		glm::ivec4 BuildMesh();


		void SetRunSettings(std::vector<glm::ivec3> locations);

		void Generate_Heightmaps(std::vector<glm::ivec3> chunk_locations);

		std::string m_shaderDir;
		int m_WorkGroups = 0;

		int m_active_batches{ 0 };

		const int Max_Verts = (int)Utilities::Vertex_Limit_Mode::Chunk_Max;
		const int m_voxel_runtime{ VOXEL_RUNTIME };

		Vulkan_Device_Info m_device_vk;
		OpenCL_Device_Info m_device_cl;
		ComputeInterface::ControllerInfo m_controllerInfo{};
		IComputeController* m_controller;

		HeightmapGenerator* m_HeightmapGenerator{ nullptr };
		TerrainModifications* m_Terrain_Modifications{ nullptr };

		IComputeProgram::FileType m_type;

		// Programs

		const std::string PROGRAM = "compute";
		VoxelComputeProgram* m_program_compute{ nullptr };

		const std::string PROGRAM_HEIGHTMAP = "1-heightmap_field_gen";
		VoxelComputeProgram* m_program_heightmap{ nullptr };

		const std::string PROGRAM_ISO_FIELD = "2-iso_field_gen";
		VoxelComputeProgram* m_program_iso_field{ nullptr };

		const std::string PROGRAM_MATERIAL_FIELD = "material_field"; // DEPRICATED
		VoxelComputeProgram* m_program_material_field{ nullptr };

		const std::string PROGRAM_UNIFY_FIELDS = "3-unify_fields";
		VoxelComputeProgram* m_program_unify_fields{ nullptr };

		const std::string PROGRAM_SMOOTH_RENDER_GEN_ISO_NORMAL = "3-smoothrender_gen_iso_normals";
		VoxelComputeProgram* m_program_smoothrender_normal_iso_norm{ nullptr };


		const std::string PROGRAM_SMOOTH_RENDER_CREATE_VERTLIST = "smoothrender_createvertlist"; // DEPRICATED
		VoxelComputeProgram* m_program_smoothrender_createvertlist{ nullptr };

		const std::string PROGRAM_SMOOTH_RENDER_CREATE_MESH = "smoothrender_createmesh"; // DEPRICATED
		VoxelComputeProgram* m_program_smoothrender_createmesh{ nullptr };

		const std::string PROGRAM_SMOOTH_RENDER_CONSTRUCT = "4-smoothrender_construct";
		VoxelComputeProgram* m_program_smoothrender_construct{ nullptr };

		const std::string PROGRAM_SMOOTH_RENDER_MARK = "5-smoothrender_mark";
		VoxelComputeProgram* m_program_smoothrender_mark{ nullptr };

		const std::string PROGRAM_SMOOTH_RENDER_MARK_OFFSETS = "6-smoothrender_mark_offsets";
		VoxelComputeProgram* m_program_smoothrender_mark_offsets{ nullptr };

		const std::string PROGRAM_SMOOTH_RENDER_STITCH = "smoothrender_stitch"; // DEPRICATED
		VoxelComputeProgram* m_program_smoothrender_stitch{ nullptr };

		const std::string PROGRAM_SMOOTH_RENDER_STITCH_ASYNC = "7-smoothrender_async_stitch";
		VoxelComputeProgram* m_program_smoothrender_stitch_async{ nullptr };


		// Buffers

		IComputeBuffer* m_in_static_settings_buffer{ nullptr };
		IComputeBuffer* m_in_run_settings_buffer{ nullptr };

		IComputeBuffer* in_locOffset_buffer{ nullptr };
		IComputeBuffer* in_globalOffsets_buffer{ nullptr };

		IComputeBuffer* in_directionOffsets_buffer{ nullptr };

		IComputeBuffer* in_edgeTable_Buffer{ nullptr };
		IComputeBuffer* in_TriTable_Buffer{ nullptr };

		IComputeBuffer* m_in_Buffer{ nullptr };
		IComputeBuffer* m_out_Buffer{ nullptr };

		IComputeBuffer* m_out_debug_buffer_Heightmap{ nullptr };
		IComputeBuffer* m_out_debug_buffer_ISO_Field{ nullptr };
		IComputeBuffer* m_out_debug_buffer_Unify_Fields{ nullptr };
		IComputeBuffer* m_out_debug_buffer_Construct{ nullptr };
		IComputeBuffer* m_out_debug_buffer_Mark{ nullptr };
		IComputeBuffer* m_out_debug_buffer_Stitch_async{ nullptr };

		IComputeBuffer* m_heightmap_run_settings_buffer{ nullptr };
		IComputeBuffer* m_heightmap_data_buffer{ nullptr };
		IComputeBuffer* m_iso_field_buffer{ nullptr };
		IComputeBuffer* m_material_buffer{ nullptr };
		IComputeBuffer* m_iso_mat_buffer{ nullptr };

		IComputeBuffer* m_gen_iso_type_buffer{ nullptr };
		IComputeBuffer* m_gen_normal_buffer{ nullptr };

		IComputeBuffer* m_vertList_buffer{ nullptr };
		IComputeBuffer* m_cubeIndex_buffer{ nullptr };

		IComputeBuffer* m_trans_vertex_buffer{ nullptr };
		IComputeBuffer* m_trans_normal_buffer{ nullptr };
		IComputeBuffer* m_trans_triangles_buffer{ nullptr };
		IComputeBuffer* m_trans_counts_buffer{ nullptr };

		IComputeBuffer* m_stitch_map_buffer{ nullptr };
		IComputeBuffer* m_stitch_map_offset_buffer{ nullptr };

		IComputeBuffer* out_Debug_Grid_buffer{ nullptr };

		IComputeBuffer* m_out_vertex_buffer{ nullptr };
		IComputeBuffer* m_out_normal_buffer{ nullptr };
		IComputeBuffer* m_out_mat_buffer{ nullptr };
		IComputeBuffer* m_out_triangles_buffer{ nullptr };
		IComputeBuffer* m_out_counts_buffer{ nullptr };

		
		Static_Settings m_static_settings{};
		Run_Settings* m_run_settings{ nullptr };

		int m_numBatchGroups{ 0 };
		int m_numBatchesPerGroup{ 0 };
		int m_totalBatches{ 0 };
		
		bool m_invert_tris = false;

		// output cache
		bool extract_cached = false;
		glm::vec4* vertex_cache{ nullptr };
		glm::vec4* normal_cache{ nullptr };
		int* tris_cache{ nullptr };
		std::vector<glm::ivec4> counts_cache;

		// heightmap cache
		std::map<int, IComputeBuffer*> m_heightmap_cache;
		std::queue<IComputeBuffer*> m_unused_heightmaps;
		int m_numColumns{ 0 };
		std::vector<int> m_activeColumIndex;


		/*double VoxelsPerMeter;
		int ChunkMeterSizeX;
		int ChunkMeterSizeY;
		int ChunkMeterSizeZ;
		int ChunkSizeX;
		int ChunkSizeY;
		int ChunkSizeZ;
		int FullChunkSize;
		float skipDist;
		float half;
		float xSideLength;
		float ySideLength;
		float zSideLength;*/

		glm::fvec4 locOffset[8];
		glm::fvec4 globalOffsets[8];

		inline static const std::string LOG_LOC{ "SMOOTH_VOXEL_BUILDER" };
	};
}