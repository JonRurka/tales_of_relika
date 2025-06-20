#pragma once

#include <string>

#include "game_engine.h"

class Game_Resources {
public:

	class Data_Files {
	public:
		inline static const std::string BLOCK_TYPES = "block_types.ini";
	};

	class Models {
	public:

	};

	class Shaders {
	public:
		class Compute {
		public:
			inline static const std::string CREATE_VBO = "compute::create_vbo.cl";
			inline static const std::string TEST_OPENCL_SPIRV = "compute::test::test_opencl_spirv.cl";
		};

		class Graphics {
		public:
			class Engine {

			};

			class Voxel {
				public:
				inline static const std::string CHUNK_OPAQUE_VERT = "graphics::voxel::opaque_chunk.vert";
				inline static const std::string CHUNK_OPAQUE_FRAG = "graphics::voxel::opaque_chunk.frag";
			};

			inline static const std::string STANDARD_VERT = "graphics::standard::standard.vert";
			inline static const std::string STANDARD_FRAG = "graphics::standard::standard.frag";

			inline static const std::string SKYBOX_VERT = "graphics::skybox::skybox.vert";
			inline static const std::string SKYBOX_FRAG = "graphics::skybox::skybox.frag";
		};
	};

	class Textures {
	public:
		inline static const std::string CONTAINER_DIFFUSE = "container::container2_diffuse.png";
		inline static const std::string CONTAINER_SPECULAR = "container::container2_specular.png";

		inline static const std::string WHITE = "Engine::white.png";

		inline static const std::string SKYBOX_BACK = "skybox::back.jpg";
		inline static const std::string SKYBOX_BOTTOM = "skybox::bottom.jpg";
		inline static const std::string SKYBOX_FRONT = "skybox::front.jpg";
		inline static const std::string SKYBOX_LEFT = "skybox::left.jpg";
		inline static const std::string SKYBOX_RIGHT = "skybox::right.jpg";
		inline static const std::string SKYBOX_TOP = "skybox::top.jpg";

		class Natural {
		public:
			inline static const std::string DIRT_1_DIFFUSE = "Natural::dirt::dirt_1::dirt_1_diffuse.jpg";
			inline static const std::string DIRT_1_NORMAL = "Natural::dirt::dirt_1::dirt_1_normals.bmp";

			inline static const std::string GRASS_1_DIFFUSE = "Natural::grass::grass_1::grass_1_diffuse.jpg";
			inline static const std::string GRASS_1_NORMAL = "Natural::grass::grass_1::grass_1_normals.bmp";

			inline static const std::string STONE_1_DIFFUSE = "Natural::stone::stone_1::stone_1_diffuse.jpg";
			inline static const std::string STONE_1_NORMAL = "Natural::stone::stone_1::stone_1_normals.bmp";
		};
	};

private:

};