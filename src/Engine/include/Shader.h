#pragma once

#include "opengl.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <unordered_map>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define LIGHT_UNIFORM_BUFFER_NAME "LightBlock"


class MeshRenderer;
class Renderer;
class Texture;
class Light;
class Material;

class Shader
{
    friend class Light;
public:

    struct Bound_Texture {
        std::string name;
        Texture* texture;
    };

    // the program ID
    unsigned int ID() { return m_ID; }
    bool Initialized() { return m_initialized; }

    // constructor reads and builds the shader
    Shader(std::string name, const std::string vertexPath, const std::string fragmentPath);
    // use/activate the shader
    void use(bool update_camera = false);

    void Bind_Textures();

    void Init_Lights();


    // utility uniform functions
    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;
    void SetVec2(const std::string& name, glm::vec2 value) const;
    void SetVec3(const std::string& name, glm::vec3 value) const;
    void SetVec4(const std::string& name, glm::vec4 value) const;
    void setMat3x3(const std::string& name, glm::mat3 value) const;
    void setMat4x4(const std::string& name, glm::mat4 value) const;

    //void Set_Samplers(std::vector<std::string> sampler_names);
    //void Set_Textures(std::vector<Texture*> textures);
    void Set_Textures(std::vector<Bound_Texture> textures);

    void Register_Renderer(MeshRenderer* rend);
    void Register_Material(Material* mat);
    void Update_Source_Materials(float dt);


    static Shader* Create(std::string name, const std::string vertex_name, const std::string fragment_name);
    static Shader* Get_Shader(unsigned int id);
    static Shader* Get_Shader(std::string name);
    static std::vector<unsigned int> Get_Shader_ID_List();
    static std::vector<Renderer*> Get_Shader_Renderer_List(unsigned int id);
    static std::vector<std::vector<Renderer*>> Get_Shader_Renderer_List();


private:
    unsigned int m_ID{ 0 };
    std::string m_name;
    bool m_initialized{false};

    bool m_lights_initialized{false};
    GLuint uniformBlockIndexLights{ 0 };

    //std::vector<Texture*> m_textures;
    std::vector<Bound_Texture> m_textures;

    std::vector<Material*> m_source_materials;

    static std::unordered_map<unsigned int, std::vector<Renderer*>> m_renderers;
    static std::unordered_map<unsigned int, Shader*> m_shaders;
    static std::unordered_map<std::string, Shader*> m_shaders_map;

    const std::string LOG_LOC{ "SHADER" };
};



