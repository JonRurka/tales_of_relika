#include "Standard_Material.h"

#include "Game_Resources.h"

Standard_Material::Standard_Material()
{
	Set_Shader(Shader::Get_Shader("standard"));
	RegisterTexture("material.diffuse");
	RegisterTexture("material.specular");
}
