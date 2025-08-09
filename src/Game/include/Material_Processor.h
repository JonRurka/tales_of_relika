#pragma once

#include "game_engine.h"

#include <unordered_map>
#include <memory>

#define UNIFORM_PROCESSOR_NAME "uniform"

class Material_Processor {
public:
	typedef std::shared_ptr<Material_Processor> Shared;
	typedef std::weak_ptr<Material_Processor> Weak;

	template<typename T,
		typename = std::enable_if_t<std::is_base_of<Material_Processor, T>::value>>
	static void Add() {
		std::shared_ptr<T> comp = std::make_shared<T>();
		initialize_material_processor(std::static_pointer_cast<Material_Processor>(comp));
	}

	static bool Material_Processor_Exists(std::string name);

	static std::shared_ptr<Material_Processor> Get_Material_Processor(std::string name);


	virtual std::string Name() = 0;

	virtual int Get_Tile_Texture_Index(int tile_id, uint8_t orientation) = 0;


protected:

	virtual void Init() = 0;

private:

	static std::unordered_map<std::string, std::shared_ptr<Material_Processor>> m_processors;

	static void initialize_material_processor(std::shared_ptr<Material_Processor> mat_processor);

	inline static const std::string LOG_LOC{ "MATERIAL_PROCESSOR" };

};