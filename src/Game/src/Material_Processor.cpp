#include "Material_Processor.h"

#include "Logger.h"

std::unordered_map<std::string, Material_Processor*> Material_Processor::m_processors;

void Material_Processor::initialize_material_processor(Material_Processor* mat_processor)
{
	mat_processor->Init();
	m_processors[mat_processor->Name()] = mat_processor;
}

bool Material_Processor::Material_Processor_Exists(std::string name)
{
	return m_processors.contains(name);
}

Material_Processor* Material_Processor::Get_Material_Processor(std::string name)
{
	if (!Material_Processor_Exists(name)) {
		return nullptr;
	}
	return m_processors[name];
}

