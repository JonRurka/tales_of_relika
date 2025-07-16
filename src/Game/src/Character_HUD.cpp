#include "Character_HUD.h"

#include "WorldGenController.h"
#include "StructureController.h"
#include "CubeVoxelBuilder.h"
#include "Block_Type.h"
#include "Block.h"

#include <RmlUi/Core.h>
#include "imgui.h"

#include "Game_Resources.h"
#include "Item_Type.h"

using namespace VoxelEngine;

#define IMAGE_DIR "material_previews"
#define IMAGE_EXT ".tga"

#define GRID_SIZE 8
namespace {
	glm::ivec4 g_directionOffsets[GRID_SIZE] =
	{
		glm::ivec4(0, 0, 1, 0),
		glm::ivec4(1, 0, 1, 0),
		glm::ivec4(1, 0, 0, 0),
		glm::ivec4(0, 0, 0, 0),
		glm::ivec4(0, 1, 1, 0),
		glm::ivec4(1, 1, 1, 0),
		glm::ivec4(1, 1, 0, 0),
		glm::ivec4(0, 1, 0, 0),
	};
}

void Character_HUD::Init(Camera* camera)
{
	m_camera = camera;

	m_hotbar_doc = UI_Engine::Instance()->Load_Document_Resource("hot_bar", Game_Resources::UI::Documents::HUD::HOT_BAR);
	m_hotbar_doc->Show();

	Set_HotBar_Tile_ID(0, 4);
	Set_Active_HotBar_Tile(0);

	m_iter_hotbar_tile = 0;

	m_hot_bar_items = std::vector<Inventory_Item>(10, Inventory_Item());
}

void Character_HUD::HotBar_Visible(bool visible)
{
	if (visible) {
		m_hotbar_doc->Show();
	}
	else {
		m_hotbar_doc->Hide();
	}
}

void Character_HUD::Set_Hotbar_Item(int hotbar_id, Inventory_Item item)
{
	m_hot_bar_items[hotbar_id] = item;
	Set_HotBar_Tile_ID(hotbar_id, item.Get_Type()->Get_ID());
}

Inventory_Item Character_HUD::Get_Hotbar_Item(int hotbar_id)
{
	return m_hot_bar_items[hotbar_id];
}

void Character_HUD::Set_HotBar_Tile_ID(int hotbar_id, int material_id)
{
	std::string img_id = "item_img_" + std::to_string(hotbar_id);

	Rml::Element* img_elem = m_hotbar_doc->GetElementById(img_id);

	std::string tile_img_src = IMAGE_DIR + std::string("/") + std::to_string(material_id) + IMAGE_EXT;
	img_elem->SetAttribute("src", tile_img_src);
	img_elem->SetProperty("visibility", "visible");
}

void Character_HUD::Set_Active_HotBar_Tile(int hotbar_id)
{
	if (hotbar_id == m_last_active_hotbar_tile)
		return;

	std::string cell_id = "item_cell_" + std::to_string(hotbar_id);

	Rml::Element* cell_elem = m_hotbar_doc->GetElementById(cell_id);

	cell_elem->SetProperty("border-color", "gray");

	if (m_last_active_hotbar_tile >= 0)
	{
		cell_id = "item_cell_" + std::to_string(m_last_active_hotbar_tile);
		cell_elem = m_hotbar_doc->GetElementById(cell_id);
		cell_elem->SetProperty("border-color", "black");
	}

	m_last_active_hotbar_tile = hotbar_id;
}

void Character_HUD::Init()
{
	m_edit_timer = Utilities::Get_Time();
}

void Character_HUD::Update(float dt)
{
	/*bool can_place = false;
	double cur_time = Utilities::Get_Time();
	if (cur_time - m_edit_timer > 1.0f) {
		m_edit_timer = cur_time;
		can_place = true;
		Set_Active_HotBar_Tile(m_iter_hotbar_tile);

		m_iter_hotbar_tile++;
		if (m_iter_hotbar_tile >= 10)
			m_iter_hotbar_tile = 0;
	}*/

	//Logger::LogDebug(LOG_POS("Update"), "%f", Input::Get_Input_Y("scroll"));

	 int scroll_delta = std::round(Input::Get_Input_Y("scroll"));
	 m_iter_hotbar_tile += scroll_delta;

	 if (m_iter_hotbar_tile < 0)
		 m_iter_hotbar_tile = 9;
	 if (m_iter_hotbar_tile >= 10)
		 m_iter_hotbar_tile = 0;
	 Set_Active_HotBar_Tile(m_iter_hotbar_tile);

	draw_ui();

	glm::vec3 ray_start;
	glm::vec3 ray_dir;
	m_camera->ScreenPointToRay(Input::Get_Mouse_Position(), ray_start, ray_dir);
	Physics::RayHit hit = Physics::Raycast(ray_start, ray_dir * 100.0f);
	if (hit.did_hit) {
		Graphics::DrawDebugRay(hit.hit_point, hit.normal, glm::vec3(0.0f, 1.0f, 0.0f));
		glm::ivec3 voxel_coord_box = WorldGenController::WorldToVoxel(hit.hit_point - (hit.normal * 0.00f));

		//glm::ivec3 chunk = WorldGenController::
		//glm::ivec3 chunk = WorldGenController::VoxelToChunk(voxel_coord);
		//glm::ivec3 voxel_local = WorldGenController::GlobalToLocalChunkCoord(chunk, voxel_coord);

		draw_voxel_box(voxel_coord_box);

		if (Input::GetMouseKeyDown(input::MouseButton::Left)) {
			Logger::LogDebug(LOG_POS("Update"), "Left Click.");
			left_click_block(hit.hit_point, hit.normal);
			//WorldGenController::Instance()->Modify_Voxel_ISO(selected_voxel, 1.0);
		}
		if (Input::GetMouseKeyDown(input::MouseButton::Right)) {
			right_click_block(hit.hit_point, hit.normal);
			//WorldGenController::Instance()->Modify_Voxel_ISO(selected_voxel, -1.0);
		}

	}


}



void Character_HUD::draw_ui()
{
	//ImGuiWindowFlags flags;
	//flags
	//ImGui::Begin(".");
	//ImGui::Text("This is a text box.");
	//ImGui::End();


	/*{
		static float f = 0.0f;
		static int counter = 0;

		ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

		ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
		ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
		ImGui::Checkbox("Another Window", &show_another_window);

		ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
		ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

		if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
			counter++;
		ImGui::SameLine();
		ImGui::Text("counter = %d", counter);

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / Graphics::GUI_IO().Framerate, Graphics::GUI_IO().Framerate);
		ImGui::End();
	}

	if (show_another_window)
	{
		ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
		ImGui::Text("Hello from another window!");
		if (ImGui::Button("Close Me"))
			show_another_window = false;
		ImGui::End();
	}*/
}

void Character_HUD::left_click_block(glm::vec3 hit_point, glm::vec3 normal)
{
	Logger::LogDebug(LOG_POS("left_click_block"), "Block Click.");
	glm::ivec3 voxel_coord = WorldGenController::WorldToVoxel(hit_point + (normal * 0.01f));
	//left_click_terrain(hit_point, normal, voxel_coord);
	left_click_structure(hit_point, normal, voxel_coord);
}

void Character_HUD::right_click_block(glm::vec3 hit_point, glm::vec3 normal)
{
	glm::ivec3 voxel_coord = WorldGenController::WorldToVoxel(hit_point + (normal * 0.01f));
	//right_click_terrain(hit_point, normal, voxel_coord);
	right_click_structure(hit_point, normal, voxel_coord);
}

void Character_HUD::left_click_terrain(glm::vec3 hit_point, glm::vec3 normal, glm::ivec3 voxel_coord)
{
	Logger::LogDebug(LOG_POS("Update"), "Voxel Clicked....");
	glm::ivec3 selected_src_voxel = get_closest_voxel(voxel_coord, hit_point, true);
	std::vector<glm::ivec4> near_voxels = get_surrounding_voxels(selected_src_voxel, true);

	std::vector<WorldGenController::TerrainMod> changes;
	changes.reserve(near_voxels.size());
	for (const auto& nv : near_voxels) {
		WorldGenController::TerrainMod mod(glm::ivec3(nv.x, nv.y, nv.z), (0.5f / (float)nv.w));
		changes.push_back(mod);
	}
	WorldGenController::Instance()->Modify_Voxel(changes);
}

void Character_HUD::right_click_terrain(glm::vec3 hit_point, glm::vec3 normal, glm::ivec3 voxel_coord)
{
	
	Logger::LogDebug(LOG_POS("Update"), "Voxel Clicked....");
	glm::ivec3 selected_src_voxel = get_closest_voxel(voxel_coord, hit_point, false);
}

void Character_HUD::left_click_structure(glm::vec3 hit_point, glm::vec3 normal, glm::ivec3 voxel_coord)
{
	Logger::LogDebug(LOG_POS("left_click_structure"), "Structure Click.");

	Block target_block = Block::Get_Block(voxel_coord);
	target_block.Set_Type(Block_Type::Brick);

	//glm::ivec3 chunk = StructureController::VoxelToChunk(voxel_coord);
	//glm::ivec3 voxel_local = WorldGenController::GlobalToLocalChunkCoord(chunk, voxel_coord);

	/*uint32_t block_type_raw = 0;
	CubeVoxelBuilder::Set_Block_Type(block_type_raw, 4);
	CubeVoxelBuilder::Set_Block_Orientation(block_type_raw, 0);

	if (!StructureController::Instance()->Chunk_Exists(chunk)) {
		StructureController::StructureMod new_block(voxel_local, block_type_raw);
		std::vector<StructureController::StructureMod> mod;
		mod.push_back(new_block);
		StructureController::Instance()->Spawn_Chunk(chunk, mod);
		Logger::LogDebug(LOG_POS("left_click_structure"), "Spawning new chunk...");
	}
	else {
		//StructureController::StructureMod new_block(voxel_coord, block_type_raw);
		StructureController::Instance()->Modify_Voxel_Type(voxel_coord, block_type_raw);
			Logger::LogDebug(LOG_POS("left_click_structure"), "Placing Block");
		
	}*/

	
}

void Character_HUD::right_click_structure(glm::vec3 hit_point, glm::vec3 normal, glm::ivec3 voxel_coord)
{



}

glm::ivec3 Character_HUD::get_closest_voxel(glm::ivec3 src_voxel, glm::fvec3 world_pos, bool inside)
{
	ISO_Sampler* iso_sampler = WorldGenController::Instance()->Get_ISO_Sampler();

	glm::ivec3 chunk = WorldGenController::VoxelToChunk(src_voxel);
	glm::ivec3 src_local = WorldGenController::GlobalToLocalChunkCoord(chunk, src_voxel);

	std::vector<glm::ivec3> grid_global;
	std::vector<glm::ivec3> grid_local;
	std::vector<glm::fvec3> grid_world;
	grid_global.reserve(GRID_SIZE);
	grid_local.reserve(GRID_SIZE);
	grid_world.reserve(GRID_SIZE);
	for (int i = 0; i < GRID_SIZE; ++i) {
		glm::ivec3 v = src_voxel + glm::ivec3(g_directionOffsets[i].x, g_directionOffsets[i].y, g_directionOffsets[i].z);
		grid_global.push_back(v);
		grid_local.push_back(src_local + glm::ivec3(g_directionOffsets[i].x, g_directionOffsets[i].y, g_directionOffsets[i].z));
		grid_world.push_back(WorldGenController::VoxelToWorld(v));
	}

	std::vector<float> iso_vals = iso_sampler->Get_ISO(chunk, grid_local);

	int closest_voxel_idx = -1;
	float shortest_dist = 1000.0; 
	for (int i = 0; i < GRID_SIZE; ++i)
	{
		bool include = false;
		if (inside) { // inside terrain
			if (iso_vals[i] >= 0.0 && iso_vals[i] < 2.0) {
				include = true;
			}
		}
		else { // outside terrain
			if (iso_vals[i] < 0.0 && iso_vals[i] > -2.0) {
				include = true;
			}
		}

		if (include) {
			float dist = glm::distance(world_pos, grid_world[i]);
			if (dist < shortest_dist) {
				shortest_dist = dist;
				closest_voxel_idx = i;
			}

		}
	}

	if (closest_voxel_idx < 0) {
		// shouldn't happen. error
		Logger::LogError(LOG_POS("get_closest_voxel"), "Invalid selection.");
		return src_voxel;
	}

	return grid_global[closest_voxel_idx];
}

std::vector<glm::ivec4> Character_HUD::get_surrounding_voxels(glm::ivec3 src_voxel, bool inside)
{
	const int SIZE = 3 * 3 * 3;

	ISO_Sampler* iso_sampler = WorldGenController::Instance()->Get_ISO_Sampler();

	glm::ivec3 chunk = WorldGenController::VoxelToChunk(src_voxel);
	glm::ivec3 src_local = WorldGenController::GlobalToLocalChunkCoord(chunk, src_voxel);

	std::unordered_map<int, std::vector<glm::ivec3>> voxels_map;
	std::unordered_map<int, glm::ivec3> chunk_map;

	for (int x = -1; x <= 1; x++) {
		for (int y = -1; y <= 1; y++) {
			for (int z = -1; z <= 1; z++) {
				glm::ivec3 v = glm::ivec3(src_voxel.x + x, src_voxel.y + y, src_voxel.z + z);

				glm::ivec3 v_chunk = WorldGenController::VoxelToChunk(src_voxel);
				glm::ivec3 v_local = WorldGenController::GlobalToLocalChunkCoord(chunk, src_voxel);
				int ch_hash = WorldGenController::Hash_Chunk(v_chunk);
				
				if (!chunk_map.contains(ch_hash)) {
					chunk_map[ch_hash] = v_chunk;
					voxels_map[ch_hash] = std::vector<glm::ivec3>();
				}

				voxels_map[ch_hash].push_back(v_local);
			}
		}
	}

	std::vector<glm::ivec4> res;
	res.reserve(SIZE);

	for (const auto& pair : chunk_map) {
		int ch_hash = pair.first;
		glm::ivec3 ch_coord = pair.second;

		auto v_list = voxels_map[ch_hash];
		std::vector<float> iso_vals = iso_sampler->Get_ISO(chunk, v_list);
		for (int i = 0; i < iso_vals.size(); ++i) {

			bool include = false;
			if (inside) { // inside terrain
				if (iso_vals[i] >= 0.0 && iso_vals[i] < 2.0) {
					include = true;
				}
			}
			else { // outside terrain
				if (iso_vals[i] < 0.0 && iso_vals[i] > -2.0) {
					include = true;
				}
			}

			if (include) {
				float dist = glm::distance(
					glm::vec3(src_local.x, src_local.y, src_local.z),
					glm::vec3(v_list[i].x, v_list[i].y, v_list[i].z)
				);

				int div = 1;
				if (dist < 0.1) {
					div = 1;
				}
				else if (dist < 1.0) {
					div = 2;
				}
				else {
					div = 8;
				}

				glm::ivec3 wv = WorldGenController::LocalToGlobalCoord(ch_coord, v_list[i]);
				res.push_back(glm::ivec4(wv, div));
			}


		}
	}

	return res;
}

void Character_HUD::draw_voxel_box(glm::ivec3 voxel_coord)
{
	glm::vec3 edge[8];

	int size = 1;

	glm::fvec3 voxel_world_pos = WorldGenController::VoxelToWorld(voxel_coord);

	//Graphics::DrawDebugLine()
	for (int i = 0; i < 8; i++) {
		edge[i] = voxel_world_pos + glm::fvec3(g_directionOffsets[i].x * size, g_directionOffsets[i].y * size, g_directionOffsets[i].z * size);
	}


	Graphics::DrawDebugLine(edge[0], edge[1], glm::vec3(0, 0, 1));
	Graphics::DrawDebugLine(edge[1], edge[2], glm::vec3(0, 0, 1));
	Graphics::DrawDebugLine(edge[2], edge[3], glm::vec3(0, 0, 1));
	Graphics::DrawDebugLine(edge[3], edge[0], glm::vec3(0, 0, 1));

	Graphics::DrawDebugLine(edge[4], edge[5], glm::vec3(0, 0, 1));
	Graphics::DrawDebugLine(edge[5], edge[6], glm::vec3(0, 0, 1));
	Graphics::DrawDebugLine(edge[6], edge[7], glm::vec3(0, 0, 1));
	Graphics::DrawDebugLine(edge[7], edge[4], glm::vec3(0, 0, 1));

	Graphics::DrawDebugLine(edge[0], edge[4], glm::vec3(0, 0, 1));
	Graphics::DrawDebugLine(edge[1], edge[5], glm::vec3(0, 0, 1));
	Graphics::DrawDebugLine(edge[2], edge[6], glm::vec3(0, 0, 1));
	Graphics::DrawDebugLine(edge[3], edge[7], glm::vec3(0, 0, 1));



}
