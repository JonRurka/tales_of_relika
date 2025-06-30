#include "Opaque_Structure_Chunk_Material.h"

Opaque_Structure_Chunk_Material::Opaque_Structure_Chunk_Material()
{
	Set_Shader(Shader::Get_Shader("opaque_structure_chunk_material"));
	RegisterTexture("diffuse");
	//RegisterTexture("material.specular");
}
