#include "Opaque_Chunk_Material.h"

Opaque_Chunk_Material::Opaque_Chunk_Material()
{
	Set_Shader(Shader::Get_Shader("opaque_chunk_material"));
	RegisterTexture("diffuse");
	//RegisterTexture("material.specular");
}
