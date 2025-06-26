#pragma once

#include "game_engine.h"

class ISO_Sampler;

class Character_HUD : public Component {

public:

	void Init(Camera* camera);

protected:
	void Init() override;

	void Update(float dt) override;

private:

	Camera* m_camera{ nullptr };
	ISO_Sampler* m_iso_sampler{ nullptr };
	double m_edit_timer{0};

	glm::ivec3 get_closest_voxel(glm::ivec3 src_voxel, glm::fvec3 world_pos, bool inside);

	std::vector<glm::ivec4> get_surrounding_voxels(glm::ivec3 src_voxel, bool inside);

	void draw_voxel_box(glm::ivec3 voxel_coord);

	inline static const std::string LOG_LOC{ "CHARACTER_HUD" };

};