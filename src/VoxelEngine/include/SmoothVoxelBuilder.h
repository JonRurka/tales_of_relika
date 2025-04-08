#pragma once

#include "stdafx.h"
#include "ForwardDeclarations.h"
#include "shared_structures.h"
#include "IVoxelBuilder_private.h"
#include "dynamic_compute.h"

#include "VoxelComputeProgram.h"

#include <queue>

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

		void Extract(glm::vec4* out_vertex, glm::vec4* out_normal, int* out_trianges, glm::ivec4 counts);

		void ReleaseHeightmap(int x, int z);

		void SetHeightmapsEnabled(bool enabled);

		//void SetBlock(int x, int y, int z, Block block);

		//Block GetBlock(int x, int y, int z);

		//void SetSurroundingChunks();

		void Dispose();

		struct Run_Settings {
			glm::ivec4 Location;
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

		

		std::string m_shaderDir;
		int m_WorkGroups = 0;

		Vulkan_Device_Info m_device;
		ComputeInterface::ControllerInfo m_controllerInfo{};
		IComputeController* m_controller;

		// Programs

		const std::string PROGRAM = "compute";
		VoxelComputeProgram* m_program_compute;

		const std::string PROGRAM_HEIGHTMAP = "1-heightmap_field_gen";
		VoxelComputeProgram* m_program_heightmap;

		const std::string PROGRAM_ISO_FIELD = "2-iso_field_gen";
		VoxelComputeProgram* m_program_iso_field;

		const std::string PROGRAM_MATERIAL_FIELD = "material_field"; // DEPRICATED
		VoxelComputeProgram* m_program_material_field;

		const std::string PROGRAM_UNIFY_FIELDS = "3-unify_fields";
		VoxelComputeProgram* m_program_unify_fields;

		const std::string PROGRAM_SMOOTH_RENDER_CREATE_VERTLIST = "smoothrender_createvertlist"; // DEPRICATED
		VoxelComputeProgram* m_program_smoothrender_createvertlist;

		const std::string PROGRAM_SMOOTH_RENDER_CREATE_MESH = "smoothrender_createmesh"; // DEPRICATED
		VoxelComputeProgram* m_program_smoothrender_createmesh;

		const std::string PROGRAM_SMOOTH_RENDER_CONSTRUCT = "4-smoothrender_construct";
		VoxelComputeProgram* m_program_smoothrender_construct;

		const std::string PROGRAM_SMOOTH_RENDER_MARK = "5-smoothrender_mark"; // DEPRICATED
		VoxelComputeProgram* m_program_smoothrender_mark;

		const std::string PROGRAM_SMOOTH_RENDER_STITCH = "smoothrender_stitch"; // DEPRICATED
		VoxelComputeProgram* m_program_smoothrender_stitch;

		const std::string PROGRAM_SMOOTH_RENDER_STITCH_ASYNC = "6-smoothrender_async_stitch";
		VoxelComputeProgram* m_program_smoothrender_stitch_async;


		// Buffers

		IComputeBuffer* m_in_static_settings_buffer;
		IComputeBuffer* m_in_run_settings_buffer;

		IComputeBuffer* in_locOffset_buffer;
		IComputeBuffer* in_globalOffsets_buffer;

		IComputeBuffer* in_directionOffsets_buffer;

		IComputeBuffer* in_edgeTable_Buffer;
		IComputeBuffer* in_TriTable_Buffer;

		IComputeBuffer* m_in_Buffer;
		IComputeBuffer* m_out_Buffer;

		IComputeBuffer* m_out_debug_buffer_Heightmap;
		IComputeBuffer* m_out_debug_buffer_ISO_Field;
		IComputeBuffer* m_out_debug_buffer_Unify_Fields;
		IComputeBuffer* m_out_debug_buffer_Construct;
		IComputeBuffer* m_out_debug_buffer_Mark;
		IComputeBuffer* m_out_debug_buffer_Stitch_async;

		IComputeBuffer* m_heightmap_run_settings_buffer;
		IComputeBuffer* m_heightmap_data_buffer;
		IComputeBuffer* m_iso_field_buffer;
		IComputeBuffer* m_material_buffer;
		IComputeBuffer* m_iso_mat_buffer;

		IComputeBuffer* m_vertList_buffer;
		IComputeBuffer* m_cubeIndex_buffer;

		IComputeBuffer* m_trans_vertex_buffer;
		IComputeBuffer* m_trans_normal_buffer;
		IComputeBuffer* m_trans_triangles_buffer;
		IComputeBuffer* m_trans_counts_buffer;

		IComputeBuffer* m_stitch_map_buffer;

		IComputeBuffer* out_Debug_Grid_buffer;

		IComputeBuffer* m_out_vertex_buffer;
		IComputeBuffer* m_out_normal_buffer;
		IComputeBuffer* m_out_triangles_buffer;
		IComputeBuffer* m_out_counts_buffer;

		
		Static_Settings m_static_settings{};
		Run_Settings* m_run_settings;

		int m_numBatchGroups;
		int m_numBatchesPerGroup;
		int m_totalBatches;
		
		bool m_invert_tris = false;

		// output cache
		bool extract_cached = false;
		glm::vec4* vertex_cache;
		glm::vec4* normal_cache;
		int* tris_cache;
		std::vector<glm::ivec4> counts_cache;

		// heightmap cache
		std::map<int, IComputeBuffer*> m_heightmap_cache;
		std::queue<IComputeBuffer*> m_unused_heightmaps;
		int m_numColumns;
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