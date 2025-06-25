#include "Character_HUD.h"

#include "WorldGenController.h"

void Character_HUD::Init(Camera* camera)
{
	m_camera = camera;
}

void Character_HUD::Init()
{
}

void Character_HUD::Update(float dt)
{

	glm::vec3 ray_start;
	glm::vec3 ray_dir;
	m_camera->ScreenPointToRay(Input::Get_Mouse_Position(), ray_start, ray_dir);
	Physics::RayHit hit = Physics::Raycast(ray_start, ray_dir * 100.0f);
	if (hit.did_hit) {
		Graphics::DrawDebugRay(hit.hit_point, hit.normal, glm::vec3(0.0f, 1.0f, 0.0f));
		glm::ivec3 voxel_coord = WorldGenController::WorldToVoxel(hit.hit_point - (hit.normal * 0.01f));
		draw_voxel_box(voxel_coord);

		if (Input::GetMouseKeyDown(input::MouseButton::Left)) {
			//Logger::LogDebug(LOG_POS("Update"), "Mouse Clicked.");
			WorldGenController::Instance()->Modify_Voxel_ISO(voxel_coord, 1.34);
		}
		if (Input::GetMouseKeyDown(input::MouseButton::Right)) {
			//Logger::LogDebug(LOG_POS("Update"), "Mouse Clicked.");
			WorldGenController::Instance()->Modify_Voxel_ISO(voxel_coord, -1.34);
		}

	}


}

void Character_HUD::draw_voxel_box(glm::ivec3 voxel_coord)
{

	glm::ivec4 directionOffsets[8] =
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

	glm::vec3 edge[8];

	int size = 1;

	glm::fvec3 voxel_world_pos = WorldGenController::VoxelToWorld(voxel_coord);

	//Graphics::DrawDebugLine()
	for (int i = 0; i < 8; i++) {
		edge[i] = voxel_world_pos + glm::fvec3(directionOffsets[i].x * size, directionOffsets[i].y * size, directionOffsets[i].z * size);
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
