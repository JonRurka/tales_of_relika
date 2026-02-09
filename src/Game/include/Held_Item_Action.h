#pragma once

#include "game_engine.h"
#include "Inventory_Item.h"
#include "ISO_Sampler.h"

#include <unordered_map>

class Held_Item_Action {
public:

	struct UseInfo {
		Physics::RayHit Ray_Hit;
	};

	enum class Mesh_Presets : int {
		None,
		Cube,
		Sphere,
		Plane,
		Other,
		
	};

	struct ItemAttributes {
	public:

		bool Has_String(std::string name) {
			return m_string_attr.contains(name);
		}
		void Set_String(std::string key, std::string val) {
			m_string_attr[key] = val;
		}
		std::string Get_String(std::string key) {
			return m_string_attr[key];
		}

		bool Has_Integer(std::string name) {
			return m_int_attr.contains(name);
		}
		void Set_Integer(std::string key, int val) {
			m_int_attr[key] = val;
		}
		int Get_Integer(std::string key) {
			return m_int_attr[key];
		}

		bool Has_Decimal(std::string name) {
			return m_float_attr.contains(name);
		}
		void Set_Decimal(std::string key, float val) {
			m_float_attr[key] = val;
		}
		float Get_Decimal(std::string key) {
			return m_float_attr[key];
		}

	private:
		std::unordered_map<std::string, std::string> m_string_attr;
		std::unordered_map<std::string, float> m_float_attr;
		std::unordered_map<std::string, int> m_int_attr;
	};

	struct ItemWorldObject {
		Mesh_Presets Preset;
		Mesh::Shared Shared_Mesh;
		Texture::Shared Diffuse_Texture;
		Texture::Shared Normal_Texture;
	};

	struct ItemDescription {
		ItemWorldObject PhysicalObject;
		Item_Loader::Item_Data LoadData;
		ItemAttributes Attributes;
	};


	

	static void Load();

	virtual const ItemWorldObject& Hold_Object(Inventory_Item& item);

	virtual void Use_Left(Inventory_Item& item, const UseInfo& hit) = 0;

	virtual void Use_Right(Inventory_Item& item, const UseInfo& hit) = 0;

	static const ItemDescription& Get_Item_Description(Item_Type::Shared item_type);
	static const ItemDescription& Get_Item_Description(Inventory_Item& item);
	static const ItemWorldObject& Get_Item_Object(Inventory_Item& item);
	static const ItemAttributes& Get_Item_Attributes(Inventory_Item& item);

protected:
	
	// TODO: Unify the collider user data type IDs somewhere
	enum class VoxelType : int {
		Structural = 1,
		Terrain = 2,
		Other
	};

	enum class InOutHit {
		In,
		Out,
		Either
	};
	glm::ivec3 Hit_Voxel(const UseInfo& info, InOutHit in_out = InOutHit::Either);
	glm::vec3 Hit_Position(const UseInfo& info);
	glm::vec3 Hit_Normal(const UseInfo& info);
	glm::ivec3 Hit_Closest_voxel(const UseInfo& info, InOutHit in_out = InOutHit::In);

	VoxelType Get_Voxel_Type(const UseInfo& info);


	void Init_ISO_Sampler();

	ISO_Sampler& ISO_Sample() {
		assert(m_iso_sampler != nullptr);
		return *m_iso_sampler.get();
	}

private:

	static std::unordered_map<int, ItemDescription> m_world_objects;
	static std::unordered_map<Mesh_Presets, Mesh::Shared> m_mesh_presets;

	ISO_Sampler::Shared m_iso_sampler;

	static void load_preset_models();

	inline static const std::string LOG_LOC{ "HELD_ITEM_ACTION" };
};