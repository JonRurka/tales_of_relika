#include "Standard_Material.h"

#include "Game_Resources.h"

Standard_Material::Standard_Material()
{
	Name("standard_mat");
	Set_Shader(Shader::Get_Shader("standard"));
	RegisterTexture("material_diffuse");
	RegisterTexture("material_specular");
}
