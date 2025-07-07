#pragma once

#include "game_engine.h"

class Block_Type;

class Block {
public:

	static Block Get_Block(glm::ivec3 coord);

	void Set_Type(uint32_t type_id);
	void Set_Type(std::string type_name);
	void Set_Type(Block_Type* type_object);

	Block_Type* Get_Type() { return m_block_type; }

private:

	Block_Type* m_block_type{nullptr};
	glm::ivec3 m_global_coord;

	Block(glm::ivec3 coord, Block_Type* type) : m_block_type{type}, m_global_coord{coord}
	{ }

	inline static const std::string LOG_LOC{ "BLOCK" };
};



