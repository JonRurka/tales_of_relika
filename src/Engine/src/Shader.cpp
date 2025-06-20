#include "Shader.h"

#include "Camera.h"
#include "MeshRenderer.h"
#include "Material.h"
#include "Texture.h"
#include "Light.h"
#include "Logger.h"
#include "Resources.h"

#include "spirv_cross/spirv_cross.hpp"
#include "spirv_cross/spirv_glsl.hpp"
#include "spirv_cross/spirv_reflect.hpp"
#include "spirv_cross/spirv_cross_util.hpp"


std::unordered_map<unsigned int, std::vector<Renderer*>> Shader::m_renderers;
std::unordered_map<unsigned int, Shader*> Shader::m_shaders;
std::unordered_map<std::string, Shader*> Shader::m_shaders_map;

Shader::Shader(std::string name, const std::string vertexPath, const std::string fragmentPath)
{
    // 1. retrieve the vertex/fragment source code from filePath
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;

    m_name = name;

    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try {
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);
        std::stringstream vShaderStream, fShaderStream;
        // read file's buffer contents into streams
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        // close file handlers
        vShaderFile.close();
        fShaderFile.close();
        // convert stream into string
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
    }
    catch (std::ifstream::failure e) {
        //std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
        Logger::LogError(LOG_POS("NEW"), "FILE_NOT_SUCCESFULLY_READ");
        return;
    }

    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

    //printf("vertex code:\n%s \n", vShaderCode);
    //printf("frag code:\n%s \n", fShaderCode);

    // 2. compile shaders
    GLuint vertex, fragment;
    int success;
    char infoLog[512];

    // vertex Shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    // print compile errors if any
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertex, 512, NULL, infoLog);
        //std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
        Logger::LogError(LOG_POS("NEW::VERTEX"), "COMPILATION_FAILED: %s", infoLog);
        return;
    };

    // fragment Shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    // print compile errors if any
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertex, 512, NULL, infoLog);
        //std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
        Logger::LogError(LOG_POS("NEW::FRAGMENT"), "COMPILATION_FAILED: %s", infoLog);
        return;
    };

    m_ID = glCreateProgram();
    glAttachShader(m_ID, vertex);
    glAttachShader(m_ID, fragment);
    glLinkProgram(m_ID);
    // print linking errors if any
    glGetProgramiv(m_ID, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(m_ID, 512, NULL, infoLog);
        //std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        Logger::LogError(LOG_POS("NEW::PROGRAM"), "LINKING_FAILED: %s", infoLog);
        return;
    }

    // delete the shaders as they're linked into our program now and no longer necessary
    glDeleteShader(vertex);
    glDeleteShader(fragment);

    m_shaders[m_ID] = this;
    m_shaders_map[name] = this;
    m_renderers[m_ID] = std::vector<Renderer*>();
    m_initialized = true;
}

Shader::Shader(std::string name, const char* vertex_source, const char* fragment_src)
{
    m_name = name;

    const char* vShaderCode = vertex_source;
    const char* fShaderCode = fragment_src;

    //Logger::LogDebug(LOG_POS("NEW"), "%s", vShaderCode);

    //printf("vertex code:\n%s \n", vShaderCode);
    //printf("frag code:\n%s \n", fShaderCode);

    // 2. compile shaders
    GLuint vertex, fragment;
    int success;
    char infoLog[512];

    Logger::LogDebug(LOG_POS("NEW"), "%s: Compiling vertex shader.", name.c_str());
    // vertex Shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    // print compile errors if any
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertex, 512, NULL, infoLog);
        //std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
        Logger::LogError(LOG_POS("NEW::VERTEX"), "COMPILATION_FAILED: %s", infoLog);
        return;
    };

    Logger::LogDebug(LOG_POS("NEW"), "%s: Compiling fragment shader.", name.c_str());
    // fragment Shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    // print compile errors if any
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertex, 512, NULL, infoLog);
        //std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
        Logger::LogError(LOG_POS("NEW::FRAGMENT"), "COMPILATION_FAILED: %s", infoLog);
        return;
    };

    m_ID = glCreateProgram();
    glAttachShader(m_ID, vertex);
    glAttachShader(m_ID, fragment);
    glLinkProgram(m_ID);
    // print linking errors if any
    glGetProgramiv(m_ID, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(m_ID, 512, NULL, infoLog);
        //std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        Logger::LogError(LOG_POS("NEW::PROGRAM"), "LINKING_FAILED: %s", infoLog);
        std::string str(fragment_src);
        Logger::LogDebug(LOG_POS("NEW::PROGRAM"), "%i", (int)str.size());
        return;
    }

    // delete the shaders as they're linked into our program now and no longer necessary
    glDeleteShader(vertex);
    glDeleteShader(fragment);

    m_shaders[m_ID] = this;
    m_shaders_map[name] = this;
    m_renderers[m_ID] = std::vector<Renderer*>();
    m_initialized = true;

    Logger::LogDebug(LOG_POS("NEW::PROGRAM"), "%s: CREATED GLSL SHADER SUCCESSFULLY", name.c_str());
}

Shader::Shader(std::string name, const std::vector<char> vertex_bin, const std::vector<char> fragment_bin)
{
    m_name = name;

    // 2. compile shaders
    GLuint vertex, fragment;
    int success = 0;
    char infoLog[512];

    // vertex Shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderBinary(1, &vertex, GL_SHADER_BINARY_FORMAT_SPIR_V_ARB, vertex_bin.data(), vertex_bin.size());
    glSpecializeShaderARB(vertex, "main", 0, 0, 0);
    // print compile errors if any
    //glCheckError();
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glCheckError();
        glGetShaderInfoLog(vertex, 512, NULL, infoLog);
        //std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
        Logger::LogError(LOG_POS("NEW::VERTEX"), "%s: COMPILATION FAILED: %s", name.c_str(), infoLog);
        return;
    }
    else {
        //Logger::LogDebug(LOG_POS("NEW::VERTEX"), "%s: COMPILATION SUCCESS", name.c_str());
    }

    // fragment Shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderBinary(1, &fragment, GL_SHADER_BINARY_FORMAT_SPIR_V_ARB, fragment_bin.data(), fragment_bin.size());
    glSpecializeShaderARB(fragment, "main", 0, 0, 0);
    // print compile errors if any
    //glCheckError();
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glCheckError();
        glGetShaderInfoLog(vertex, 512, NULL, infoLog);
        //std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
        Logger::LogError(LOG_POS("NEW::FRAGMENT"), "%s: COMPILATION FAILED: %s", name.c_str(), infoLog);
        return;
    }
    else {
        //Logger::LogDebug(LOG_POS("NEW::FRAGMENT"), "%s: COMPILATION SUCCESS", name.c_str());
    }

    m_ID = glCreateProgram();
    glAttachShader(m_ID, vertex);
    glAttachShader(m_ID, fragment);
    glLinkProgram(m_ID);
    //glCheckError();
    // print linking errors if any
    glGetProgramiv(m_ID, GL_LINK_STATUS, &success);
    if (!success)
    {
        std::string err_type = "";
        glCheckError();
        switch (success) {
            case GL_INVALID_VALUE:
                err_type = "GL_INVALID_VALUE";
                break;
            case GL_INVALID_ENUM:
                err_type = "GL_INVALID_ENUM";
                break;
            case GL_INVALID_OPERATION:
                err_type = "GL_INVALID_OPERATION";
                break;
            default:
                err_type = "OTHER(" + std::to_string(success) + ")";
                break;
        }
        glGetProgramInfoLog(m_ID, 512, NULL, infoLog);
        //std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        Logger::LogError(LOG_POS("NEW::PROGRAM"), "%s: LINKING_FAILED (%s): %s", name.c_str(), err_type.c_str(), infoLog);
        return;
    }
    else {
        //Logger::LogDebug(LOG_POS("NEW::PROGRAM"), "%s: PROGRAM LINK SUCCESS", name.c_str());
    }

    // delete the shaders as they're linked into our program now and no longer necessary
    glDeleteShader(vertex);
    glDeleteShader(fragment);

    load_uniforms(vertex_bin);
    load_uniforms(fragment_bin);

    m_shaders[m_ID] = this;
    m_shaders_map[name] = this;
    m_renderers[m_ID] = std::vector<Renderer*>();
    m_initialized = true;
    m_is_spirv = true;

    Logger::LogDebug(LOG_POS("NEW::PROGRAM"), "%s: CREATED SPIRV SHADER SUCCESSFULLY", name.c_str());
}

void Shader::use(bool update_camera)
{
    glUseProgram(m_ID);
    if (update_camera)
    {
        if (Camera::Get_Active() != nullptr) {
            setMat4x4("projection_mat", Camera::Get_Active()->Projection_Matrix());
            setMat4x4("view_mat", Camera::Get_Active()->View_Matrix());
        }
    }
}

void Shader::Init_Lights()
{
    //GLuint uniformBlockIndexLights = glGetUniformBlockIndex(shaderLightingPass.Program, "LightBlock");
    //glUniformBlockBinding(shaderLightingPass.Program, uniformBlockIndexLights, 0);

    if (m_lights_initialized)
        return;

    //use();
    //uniformBlockIndexLights = glGetUniformBlockIndex(m_ID, LIGHT_UNIFORM_BUFFER_NAME);
    //printf("uniformBlockIndexLights: %u\n", uniformBlockIndexLights);
    //glUniformBlockBinding(m_ID, uniformBlockIndexLights, 1);

    Light::Add_Shader(this);

    m_lights_initialized = true;
}

void Shader::setBool(const std::string& name, bool value)
{
    setBool(get_uniform_location(name), value);
}

void Shader::setInt(const std::string& name, int value)
{
    int id = get_uniform_location(name);
    setInt(id, value);
}

void Shader::setFloat(const std::string& name, float value)
{
    setFloat(get_uniform_location(name), value);
}

void Shader::SetVec2(const std::string& name, glm::vec2 value)
{
    SetVec2(get_uniform_location(name), value);
}

void Shader::SetVec3(const std::string& name, glm::vec3 value)
{
    SetVec3(get_uniform_location(name), value);
}

void Shader::SetVec4(const std::string& name, glm::vec4 value)
{
    SetVec4(get_uniform_location(name), value);
}

void Shader::setMat3x3(const std::string& name, glm::mat3 value)
{
    setMat3x3(get_uniform_location(name), value);
}

void Shader::setMat4x4(const std::string& name, glm::mat4 value)
{
    setMat4x4(get_uniform_location(name), value);
}



void Shader::setBool(const int name, bool value) const
{
    glUniform1i(name, (int)value);
}

void Shader::setInt(const int name, int value) const
{
    glUniform1i(name, (int)value);
}

void Shader::setFloat(const int name, float value) const
{
    glUniform1f(name, value);
}

void Shader::SetVec2(const int name, glm::vec2 value) const
{
    glUniform2fv(name, 1, glm::value_ptr(value));
}

void Shader::SetVec3(const int name, glm::vec3 value) const
{
    glUniform3fv(name, 1, glm::value_ptr(value));
}

void Shader::SetVec4(const int name, glm::vec4 value) const
{
    glUniform4fv(name, 1, glm::value_ptr(value));
}

void Shader::setMat3x3(const int name, glm::mat3 value) const
{
    glUniformMatrix3fv(name, 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::setMat4x4(const int name, glm::mat4 value) const
{
    glUniformMatrix4fv(name, 1, GL_FALSE, glm::value_ptr(value));
}

/*void Shader::Set_Samplers(std::vector<std::string> sampler_names)
{
    use();
    for (int i = 0; i < sampler_names.size(); i++) {
        setInt(sampler_names[i], i);
    }
}

void Shader::Set_Textures(std::vector<Texture*> textures)
{
    //m_textures = textures;
}*/

void Shader::Set_Textures(std::vector<Bound_Texture> textures)
{
    use();
    m_textures = textures;
    if (m_is_spirv)
        return;
    for (int i = 0; i < m_textures.size(); i++) {
        setInt(m_textures[i].name, i);
    }
    
}

void Shader::Bind_Textures()
{
    for (int i = 0; i < m_textures.size(); i++) {
        m_textures[i].texture->Bind(GL_TEXTURE0 + i);
    }
}

void Shader::Register_Renderer(MeshRenderer* rend)
{
    m_renderers[m_ID].push_back(rend);
}

void Shader::Register_Material(Material* mat)
{
    m_source_materials.push_back(mat);
}

void Shader::Update_Source_Materials(float dt)
{
    for (const auto& mat : m_source_materials)
    {
        mat->Internal_Update(dt);
    }
}

Shader* Shader::Create(std::string name, const std::string vertex_name, const std::string fragment_name)
{
    if (!Resources::Has_Shader(vertex_name)) {
        Logger::LogError("SHADER::CREATE", "Failed to create shader '%s' - Could not load vertex shader: %s", name.c_str(), vertex_name.c_str());
        return nullptr;
    }

    if (!Resources::Has_Shader(fragment_name)) {
        Logger::LogError("SHADER::CREATE", "Failed to create shader '%s' - Could not load fragment shader: %s", name.c_str(), fragment_name.c_str());
        return nullptr;
    }

    Resources::Asset vertex_asset = Resources::Get_Shader_Asset(vertex_name);
    Resources::Asset fragment_asset = Resources::Get_Shader_Asset(fragment_name);

    if (vertex_asset.use_spirv != fragment_asset.use_spirv) {
        Logger::LogError(LOG_POS("Create"), "Shader must not combine spirv and glsl stages.");
        return nullptr;
    }

    bool use_spirv = vertex_asset.use_spirv;
    Shader* shader = nullptr;
    std::vector<char> vertex_bin = Resources::Get_Shader_bin(vertex_name);
    std::vector<char> fragment_bin = Resources::Get_Shader_bin(fragment_name);
    if (use_spirv) {
        shader = new Shader(name, vertex_bin, fragment_bin);
    }
    else{
        //std::string vertex_path = Resources::Get_Shader_File(vertex_name);
        //std::string fragment_path = Resources::Get_Shader_File(fragment_name);
        //Shader* shader = new Shader(name, vertex_path, fragment_path);

        std::string v_str = std::string(vertex_bin.begin(), vertex_bin.end());
        std::string f_str = std::string(fragment_bin.begin(), fragment_bin.end());
        if (fragment_name == "graphics::standard::standard.frag") {
            //Logger::LogDebug(LOG_POS("Create"), f_str.c_str());
        }
        shader = new Shader(name, v_str.c_str(), f_str.c_str());
    }
      
    if (!shader->Initialized()) {
        Logger::LogError(LOG_POS("CREATE"), "Failed to create shader '%s'", name.c_str());
    }

    return shader;
}

Shader* Shader::Get_Shader(unsigned int id)
{
    return m_shaders[id];
}

Shader* Shader::Get_Shader(std::string name)
{
    return m_shaders_map[name];
}

std::vector<unsigned int> Shader::Get_Shader_ID_List()
{
    std::vector<unsigned int> keys;
    keys.reserve(m_renderers.size());
    for (const auto& pair : m_renderers) {
        keys.push_back(pair.first);
    }
    return keys;
}

std::vector<Renderer*> Shader::Get_Shader_Renderer_List(unsigned int id)
{
    return m_renderers[id];
}

std::vector<std::vector<Renderer*>> Shader::Get_Shader_Renderer_List()
{
    std::vector<std::vector<Renderer*>> all_renderers;
    all_renderers.reserve(m_renderers.size());
    for (const auto& pair : m_renderers) {
        all_renderers.push_back(pair.second);
    }
    return all_renderers;
}

void Shader::load_uniforms(const std::vector<char> spirv_bin)
{
    uint32_t* spv_data = (uint32_t*)spirv_bin.data();
    std::vector<uint32_t> spirv_data(spv_data, spv_data + (spirv_bin.size() / 4));
    spirv_cross::Compiler compiler(spirv_data);

    // Get the reflected resources
    spirv_cross::ShaderResources resources = compiler.get_shader_resources();

    for (const auto& uniform : resources.gl_plain_uniforms)
    {
        auto type = compiler.get_type(uniform.type_id);
        std::string uniform_name = compiler.get_name(uniform.id);
        int uniform_index = compiler.get_decoration(uniform.id, spv::DecorationLocation);

        m_uniform_map[uniform_name] = uniform_index;

        Logger::LogDebug(LOG_POS("NEW::SPIRV"), "%s: Uniform: %s (%i)", m_name.c_str(), uniform_name.c_str(), uniform_index);
    }

    for (const auto& uniform : resources.sampled_images)
    {
        auto type = compiler.get_type(uniform.type_id);
        std::string uniform_name = compiler.get_name(uniform.id);
        int uniform_index = compiler.get_decoration(uniform.id, spv::DecorationLocation);

        m_uniform_map[uniform_name] = uniform_index;

        Logger::LogDebug(LOG_POS("NEW::SPIRV"), "%s: Sampler: %s (%i)", m_name.c_str(), uniform_name.c_str(), uniform_index);
    }
}

int Shader::get_uniform_location(std::string name)
{
    if (m_is_spirv) {
        if (!m_uniform_map.contains(name)) {
            Logger::LogError(LOG_POS("get_uniform_location"), "%s: spirv Uniform variable not found: %s", m_name.c_str(), name.c_str());
            return -1;
        }
        return m_uniform_map[name];
    }
    else {
        return glGetUniformLocation(m_ID, name.c_str());
    }

}
