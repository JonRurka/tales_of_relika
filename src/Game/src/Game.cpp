#include "Game.h"

#include "Test_Scene.h"
#include "Voxel_Test_Scene.h"
#include "Test_OpenCL_Scene.h"
#include "VoxelWorld_Scene.h"
#include "Game_Resources.h"
#include "Material_Types.h"

void Game::Init()
{
	//Init_Resources<Game_Resources>();

	Logger::LogInfo(LOG_POS("Init"), "Load container diffuse");
	Resources::Load_Texture(Game_Resources::Textures::CONTAINER_DIFFUSE);
	
	Logger::LogInfo(LOG_POS("Init"), "Load container specular");
	Resources::Load_Texture(Game_Resources::Textures::CONTAINER_SPECULAR);

	init_block_types();
	init_shaders();
	
	

	Logger::LogInfo(LOG_POS("Init"), "Game Initialized.");

	Scene* s = Load_Scene<VoxelWorld_Scene>("VoxelWorld");
	//Scene* s = Load_Scene<Voxel_Test_Scene>("test");
	//Scene* s = Load_Scene<Test_OpenCL_Scene>("test");
	s->Activate(true);
}

void Game::init_shaders()
{
	Shader* std_shader = Shader::Create(
		"standard", 
		Game_Resources::Shaders::Graphics::STANDARD_VERT, 
		Game_Resources::Shaders::Graphics::STANDARD_FRAG
	);
	if (std_shader == nullptr || !std_shader->Initialized())
	{
		Logger::LogFatal(LOG_POS("init_shaders"), "Failed to load standard shader.");
		return;
	}

	std_shader->Init_Lights();

	Shader* opaque_chunk_shader = Shader::Create(
		"opaque_chunk_material", 
		Game_Resources::Shaders::Graphics::Voxel::CHUNK_OPAQUE_VERT,
		Game_Resources::Shaders::Graphics::Voxel::CHUNK_OPAQUE_FRAG
	);
	if (opaque_chunk_shader == nullptr || !opaque_chunk_shader->Initialized())
	{
		Logger::LogFatal(LOG_POS("init_shaders"), "Failed to load opaque chunk shader.");
		return;
	}
	opaque_chunk_shader->Init_Lights();

	/*std_shader->Set_Textures({
		{"material.diffuse",Resources::Get_Texture(Game_Resources::Textures::CONTAINER_DIFFUSE)},
		{"material.specular",Resources::Get_Texture(Game_Resources::Textures::CONTAINER_SPECULAR)},
	});*/
}

void Game::init_block_types()
{
	m_material_types = new Material_Types();
	m_material_types->Load_Materials(Game_Resources::Data_Files::BLOCK_TYPES);
	m_material_types->Initialize_Materials();
}

void Game::Update(float dt)
{
}
