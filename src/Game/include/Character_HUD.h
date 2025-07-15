#pragma once

#include "game_engine.h"

class ISO_Sampler;

namespace Rml {
	class Context;
	class ElementDocument;
}

class Character_HUD : public Component {

public:

	void Init(Camera* camera);

	void HotBar_Visible(bool visible);

	void Set_HotBar_Tile_Material(int hotbar_id, int material_id);

	void Set_Active_HotBar_Tile(int hotbar_id);

protected:
	void Init() override;

	void Update(float dt) override;

private:

	Camera* m_camera{ nullptr };
	ISO_Sampler* m_iso_sampler{ nullptr };
	double m_edit_timer{0};

	bool show_demo_window = true;
	bool show_another_window = false;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	Rml::ElementDocument* m_hotbar_doc{ nullptr };
	int m_last_active_hotbar_tile{ -1 };

	int m_iter_hotbar_tile{ 0 };

	void draw_ui();

	void left_click_block(glm::vec3 hit_point, glm::vec3 normal);
	void right_click_block(glm::vec3 hit_point, glm::vec3 normal);

	void left_click_terrain(glm::vec3 hit_point, glm::vec3 normal, glm::ivec3 voxel_coord);
	void right_click_terrain(glm::vec3 hit_point, glm::vec3 normal, glm::ivec3 voxel_coord);

	void left_click_structure(glm::vec3 hit_point, glm::vec3 normal, glm::ivec3 voxel_coord);
	void right_click_structure(glm::vec3 hit_point, glm::vec3 normal, glm::ivec3 voxel_coord);

	glm::ivec3 get_closest_voxel(glm::ivec3 src_voxel, glm::fvec3 world_pos, bool inside);

	std::vector<glm::ivec4> get_surrounding_voxels(glm::ivec3 src_voxel, bool inside);

	void draw_voxel_box(glm::ivec3 voxel_coord);

	inline static const std::string LOG_LOC{ "CHARACTER_HUD" };

};