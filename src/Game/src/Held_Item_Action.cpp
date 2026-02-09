#include "Held_Item_Action.h"

#include "WorldGenController.h"

#include <regex>

std::unordered_map<int, Held_Item_Action::ItemDescription> Held_Item_Action::m_world_objects;
std::unordered_map<Held_Item_Action::Mesh_Presets, Mesh::Shared> Held_Item_Action::m_mesh_presets;

#define GRID_SIZE 8

namespace {
	const std::unordered_map<std::string, Held_Item_Action::Mesh_Presets> g_str_to_preset{
		{"Cube", Held_Item_Action::Mesh_Presets::Cube},
		{"Sphere", Held_Item_Action::Mesh_Presets::Sphere},
		{"Plane", Held_Item_Action::Mesh_Presets::Plane}
	};

	bool is_int(const std::string& str) {
		try {
			size_t pos;
			std::stoi(str, &pos);
			return pos == str.length();
		}
		catch (...) {
			return false;
		}
	}

	bool is_float(const std::string& str) {
		try {
			size_t pos;
			std::stof(str, &pos);
			return pos == str.length();
		}
		catch (...) {
			return false;
		}
	}

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


void Held_Item_Action::Load()
{
	load_preset_models();


	const auto& items = Item_Loader::Instance().Get_Item_Data();

	for (const auto& itd : items) {
		ItemAttributes attr;
		ItemWorldObject phys_obj;
		ItemDescription desc;

		if (g_str_to_preset.contains(itd.Held_Mesh)) {
			Mesh_Presets prst = g_str_to_preset.at(itd.Held_Mesh);
			phys_obj.Preset = prst;
			phys_obj.Shared_Mesh = m_mesh_presets[prst];
		}
		else if (Resources::Has_Model(itd.Held_Mesh)) {
			phys_obj.Preset = Mesh_Presets::Other;
			phys_obj.Shared_Mesh = Resources::Get_Model(itd.Held_Mesh)->mesh()[0];
		}
		else {
			phys_obj.Preset = Mesh_Presets::None;
		}
		

		if (Resources::Has_Texture(itd.held_Mesh_Tex)) {
			phys_obj.Diffuse_Texture = Resources::Get_Texture(itd.held_Mesh_Tex);
		}

		if (Resources::Has_Texture(itd.held_Mesh_Normal_Tex)) {
			phys_obj.Normal_Texture = Resources::Get_Texture(itd.held_Mesh_Tex);
		}

		for (const auto& att : itd.Other_Attributes) {
			if (is_float(att.second)) {
				attr.Set_Decimal(att.first, std::stof(att.second));
			}
			else if (is_int(att.second)) {
				attr.Set_Integer(att.first, std::stoi(att.second));
			}
			else {
				attr.Set_String(att.first, att.second);
			}
		}


		desc.Attributes = attr;
		desc.PhysicalObject = phys_obj;
		desc.LoadData = itd;

		m_world_objects[itd.ID] = desc;
	}


}

const Held_Item_Action::ItemDescription& Held_Item_Action::Get_Item_Description(Item_Type::Shared item_type)
{
	assert(m_world_objects.contains(item_type->Get_ID()));
	return m_world_objects[item_type->Get_ID()];
}

const Held_Item_Action::ItemDescription& Held_Item_Action::Get_Item_Description(Inventory_Item& item)
{
	return Get_Item_Description(item.Get_Type());
}

const Held_Item_Action::ItemWorldObject& Held_Item_Action::Get_Item_Object(Inventory_Item& item)
{
	return Get_Item_Description(item).PhysicalObject;
}

const Held_Item_Action::ItemAttributes& Held_Item_Action::Get_Item_Attributes(Inventory_Item& item)
{
	return Get_Item_Description(item).Attributes;
}

glm::ivec3 Held_Item_Action::Hit_Voxel(const UseInfo& info, InOutHit in_out)
{
	float offset = 0.0f;
	if (in_out == InOutHit::In) {
		offset = -0.01f;
	}
	else if (in_out == InOutHit::Out) {
		offset = 0.01f;
	}

	glm::ivec3 voxel_coord = WorldGenController::WorldToVoxel(info.Ray_Hit.hit_point + (info.Ray_Hit.normal * offset));
	return voxel_coord;
}

glm::vec3 Held_Item_Action::Hit_Position(const UseInfo& info)
{
	return info.Ray_Hit.hit_point;
}

glm::vec3 Held_Item_Action::Hit_Normal(const UseInfo& info)
{
	return info.Ray_Hit.normal;
}

glm::ivec3 Held_Item_Action::Hit_Closest_voxel(const UseInfo& info, InOutHit in_out)
{
	glm::ivec3 src_voxel = Hit_Voxel(info);
	glm::vec3 world_pos = Hit_Position(info);

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

	std::vector<float> iso_vals;
	if (in_out != InOutHit::Either) {
		assert(m_iso_sampler != nullptr);
		iso_vals = m_iso_sampler->Get_ISO(chunk, grid_local);
	}

	int closest_voxel_idx = -1;
	float shortest_dist = 1000.0;
	for (int i = 0; i < GRID_SIZE; ++i)
	{
		bool include = false;
		if (in_out != InOutHit::Either) 
		{
			if (in_out == InOutHit::In) { // inside terrain
				if (iso_vals[i] >= 0.0 && iso_vals[i] < 2.0) {
					include = true;
				}
			}
			else { // outside terrain
				if (iso_vals[i] < 0.0 && iso_vals[i] > -2.0) {
					include = true;
				}
			}
		}
		else 
		{
			include = true;
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
		Logger::LogWarning(LOG_POS("get_closest_voxel"), "Invalid selection.");
		return src_voxel;
	}

	return grid_global[closest_voxel_idx];


	return glm::ivec3();
}

Held_Item_Action::VoxelType Held_Item_Action::Get_Voxel_Type(const UseInfo& info)
{
	return (VoxelType)info.Ray_Hit.rigidbody->GetUserData();
}

void Held_Item_Action::Init_ISO_Sampler()
{
	if (WorldGenController::Instance()->Voxel_Engine_Enabled())
		m_iso_sampler = WorldGenController::Instance()->Get_ISO_Sampler();
}

void Held_Item_Action::load_preset_models()
{
	// TODO: Actually load meshes.
	m_mesh_presets[Mesh_Presets::Cube] = Mesh::CreateDummy();
	m_mesh_presets[Mesh_Presets::Sphere] = Mesh::CreateDummy();
	m_mesh_presets[Mesh_Presets::Plane] = Mesh::CreateDummy();
}

const Held_Item_Action::ItemWorldObject& Held_Item_Action::Hold_Object(Inventory_Item& item) {
	return Get_Item_Object(item);
}

