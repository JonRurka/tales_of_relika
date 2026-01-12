#include "Light.h"

#include <algorithm>
#include <cstring>
#include <sstream>

#include "WorldObject.h"
#include "Transform.h"
#include "Shader.h"
#include "Logger.h"

#include "tracy/Tracy.hpp"


#define FLUSH_DEBUG_PRINT false



#define NUM_LIGHTS() (m_light_data[0].int_options_1.w)

#define DEFAULT_LINEAR_COEF 0.027f
#define DEFAULT_QUADRATIC_COEF 0.0028f
#define DEFAULT_SPOTLIGHT_CUTOFF 20.0f
#define DEFAULT_SPOTLIGHT_OUTER_CUTOFF 25.0f

#define OPTIONS_ENABLED(data) ((data)->int_options_1.x)
#define OPTIONS_ALLOCATED(data) ((data)->int_options_1.y)
#define OPTIONS_TYPE(data) ((data)->int_options_1.z)

#define OPTIONS_STENGTH(data) ((data)->float_options_2.x)
#define OPTIONS_CONSTANT(data) ((data)->float_options_2.y)
#define OPTIONS_LINEAR(data) ((data)->float_options_2.z)

#define OPTIONS_QUADRATIC(data) ((data)->float_options_3.x)
#define OPTIONS_CUTOFF(data) ((data)->float_options_3.y)
#define OPTIONS_OUTER_CUTOFF(data) ((data)->float_options_3.z)

bool Light::m_initialized{ false };
GLuint Light::m_uboLights{ 0 };
bool Light::m_lights_inited{ false };

//std::span<Light::Light_Data, MAX_LIGHTS> Light::m_light_data;


int Light::m_num_lights{0};
int Light::curr_index{0};
std::vector<Shader*> Light::m_light_shaders;
std::unordered_map<int, std::weak_ptr<Light>> Light::m_linked_lights;
//std::vector<Light*> Light::m_lights;


void Light::Update_Lights(float dt)
{
	ZoneScopedN("Client Lights");

	debug_timer -= dt;

	Flush();
}

void Light::Init()
{
	if (!m_initialized) {
		glGenBuffers(1, &m_uboLights);
		Light_Data empty_data{};
		OPTIONS_ALLOCATED(&empty_data) = false;
		std::fill(m_light_data.begin(), m_light_data.end(), empty_data);
		//std::memset((void*)m_light_data, 0, MAX_LIGHTS * sizeof(Light_Data));
		Flush();
		m_initialized = true;
		Logger::LogInfo(LOG_POS("Init"), "Lights initialized.");
	}

	Light_Data data = Allocate_Light(std::dynamic_pointer_cast<Light>(shared_from_this()), m_id);
	//m_data = Allocate_Light(std::dynamic_pointer_cast<Light>(shared_from_this()), m_id);
	//printf("Added light: %i\n", m_id);

	m_otions_type = (Light_Type)OPTIONS_TYPE(&data);
	m_option_color = data.lightColor;
	m_option_strength = OPTIONS_STENGTH(&data);
	//m_option_constant = OPTIONS_CONSTANT(&data);
	m_option_linear_coefficient = OPTIONS_LINEAR(&data);
	m_option_quadratic_coefficient = OPTIONS_QUADRATIC(&data);
	m_option_cutOff = OPTIONS_CUTOFF(&data);
	m_option_outerCutOff = OPTIONS_OUTER_CUTOFF(&data);
	
	/*
	OPTIONS_ENABLED(&data)		= true;
	OPTIONS_TYPE(&data)			= (int)Light_Type::DIRECTIONAL;
	data.position				= glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	data.lightColor				= glm::vec4(1.0f);
	data.direction				= glm::vec4(0.0f, 0.0f, -1.0f, 0.0f);
	OPTIONS_STENGTH(&data)		= 1.0f;
	OPTIONS_CONSTANT(&data)		= 1.0f;
	OPTIONS_LINEAR(&data)		= DEFAULT_LINEAR_COEF;
	OPTIONS_QUADRATIC(&data)	= DEFAULT_QUADRATIC_COEF;
	OPTIONS_CUTOFF(&data)		= DEFAULT_SPOTLIGHT_CUTOFF;
	OPTIONS_OUTER_CUTOFF(&data) = DEFAULT_SPOTLIGHT_OUTER_CUTOFF;
	
	*/


}

void Light::Update(float dt)
{
	//return;

	//m_data->position = glm::vec4(Object().Get_Transform().Position(), 1.0f);
	//m_data->direction = glm::vec4(Object().Get_Transform().Forward(), 0.0f);

	/*if (has_shadows)
	{
		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);


		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}*/

}

void Light::OnDestroy()
{
}

void Light::Load(json jsn_data)
{
	Light_Data data = Default_Light_Data();

	jsn_data["enabled"].get_to(OPTIONS_ENABLED(&data));
	jsn_data["type"].get_to(OPTIONS_TYPE(&data));

	jsn_data["color"]["r"].get_to(data.lightColor.x);
	jsn_data["color"]["g"].get_to(data.lightColor.y);
	jsn_data["color"]["b"].get_to(data.lightColor.z);

	jsn_data["stength"].get_to(OPTIONS_STENGTH(&data));
	jsn_data["quadratic"].get_to(OPTIONS_QUADRATIC(&data));
	jsn_data["cutoff"].get_to(OPTIONS_CUTOFF(&data));
	jsn_data["outer_cutoff"].get_to(OPTIONS_OUTER_CUTOFF(&data));
	
	m_light_data[m_num_lights] = data;
}

void Light::Activate_Shadows(bool enabled)
{
	glGenFramebuffers(1, &depthMapFBO);
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Light::option_updated()
{
	Logger::LogDebug(LOG_POS("option_updated"), "An option has been updated.");
	m_has_change = true;
}

Light::Light_Data Light::sync()
{
	Light_Data data{};
	OPTIONS_ALLOCATED(&data) = 1;
	OPTIONS_ENABLED(&data) = m_option_enabled;
	OPTIONS_TYPE(&data) = (int)m_otions_type;
	data.lightColor = m_option_color;
	OPTIONS_STENGTH(&data) = m_option_strength;
	OPTIONS_LINEAR(&data) = m_option_linear_coefficient;
	OPTIONS_QUADRATIC(&data) = m_option_quadratic_coefficient;
	OPTIONS_CUTOFF(&data) = m_option_cutOff;
	OPTIONS_OUTER_CUTOFF(&data) = m_option_outerCutOff;

	m_has_change = false;

	return data;
}

void Light::Flush()
{
	//return; 
	if (!m_initialized) {
		return;
	}

	for (const auto& pair : m_linked_lights)
	{
		assert(!pair.second.expired());
		Light& light = *pair.second.lock().get();
		Light_Data& l_data = m_light_data[pair.first];

		if (light.m_has_change)
		{
			Logger::LogDebug(LOG_POS("Flush"), "Sync %s", light.Object().Name().c_str());
			l_data = light.sync();
		}

		l_data.position = glm::vec4(light.Object().Get_Transform().Position(), 1.0f);
		l_data.direction = glm::vec4(light.Object().Get_Transform().Forward(), 0.0f);
	}

	NUM_LIGHTS() = m_num_lights;
	Light_Data* data_ptr = m_light_data.data();


	if (FLUSH_DEBUG_PRINT && debug_timer <= 0)
	{
		Logger::LogDebug(LOG_POS("Flush"), "DUMP LIGHTS (%d): ", data_ptr[0].int_options_1.w);
		for (int i = 0; i < m_num_lights; i++)
		{
			Logger::LogDebug(LOG_POS("Flush"), "%s", data_ptr[i].print_data().c_str());
		}
		debug_timer = 2.0f;
	}

	glBindBuffer(GL_UNIFORM_BUFFER, m_uboLights);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(Light_Data) * m_num_lights, data_ptr, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_UNIFORM_BUFFER, 1, m_uboLights);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Light::Add_Shader(Shader* value)
{
	m_light_shaders.push_back(value);
	value->use();
	glBindBuffer(GL_UNIFORM_BUFFER, m_uboLights);
	glBindBufferBase(GL_UNIFORM_BUFFER, 1, m_uboLights);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	//glBindBuffer(GL_UNIFORM_BUFFER, m_uboLights);
	//glBindBufferBase(GL_UNIFORM_BUFFER, value->uniformBlockIndexLights, m_uboLights);
	//glBindBuffer(GL_UNIFORM_BUFFER, 0);
	//printf("Shader added to lights\n");
}

Light::Light_Data Light::Allocate_Light(std::weak_ptr<Light> light, int& id)
{
	if (m_num_lights == MAX_LIGHTS) {
		Logger::LogError(LOG_POS("Allocate_Light"), "Max Lights reached!");
		return Light::Light_Data();
	}

	m_light_data[m_num_lights] = Default_Light_Data();
	id = m_num_lights;
	m_num_lights++;
	m_linked_lights[id] = light;
	

	return m_light_data[id];

	/*
	Light_Data* res;

	m_light_data[m_num_lights] = Default_Light_Data();
	id = m_num_lights;
	res = &m_light_data[id];
	m_num_lights++;
	m_linked_lights[id] = light;

	NUM_LIGHTS() = m_num_lights;*/

	//return nullptr;
}

void Light::Deallocate_Light(int id)
{
	if (m_num_lights == 1 || id == m_num_lights - 1) {
		memset(&m_light_data[id], 0, sizeof(Light_Data));
		OPTIONS_ENABLED(&m_light_data[id]) = false;
		OPTIONS_ALLOCATED(&m_light_data[id]) = false;
	}
	else
	{
		m_light_data[id] = m_light_data[m_num_lights - 1];
		m_linked_lights[id] = m_linked_lights[m_num_lights - 1];
		m_linked_lights[id].lock()->m_id = id;

		m_linked_lights.erase(m_num_lights - 1);
		memset(&m_light_data[m_num_lights - 1], 0, sizeof(Light_Data));
	}

	m_num_lights = std::max(m_num_lights - 1, 0);
	NUM_LIGHTS() = m_num_lights;
}

Light::Light_Data Light::Default_Light_Data()
{
	Light_Data data;
	OPTIONS_ALLOCATED(&data)	= 1;
	OPTIONS_ENABLED(&data)		= 1;
	OPTIONS_TYPE(&data)			= (int)Light_Type::DIRECTIONAL;
	data.position				= glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	data.lightColor				= glm::vec4(1.0f);
	data.direction				= glm::vec4(0.0f, 0.0f, -1.0f, 0.0f);
	OPTIONS_STENGTH(&data)		= 1.0f;
	OPTIONS_CONSTANT(&data)		= 1.0f;
	OPTIONS_LINEAR(&data)		= DEFAULT_LINEAR_COEF;
	OPTIONS_QUADRATIC(&data)	= DEFAULT_QUADRATIC_COEF;
	OPTIONS_CUTOFF(&data)		= DEFAULT_SPOTLIGHT_CUTOFF;
	OPTIONS_OUTER_CUTOFF(&data) = DEFAULT_SPOTLIGHT_OUTER_CUTOFF;
	return data;
}

std::string Light::Light_Data::print_data()
{
	std::stringstream ss;

	ss << "Light Data:" << std::endl;
	ss << "\t Allocated: " << OPTIONS_ALLOCATED(this) << std::endl;
	ss << "\t Enabled: " << OPTIONS_ENABLED(this) << std::endl;
	ss << "\t Type: " << OPTIONS_TYPE(this) << std::endl;

	glm::vec4 pos = this->position;
	ss << "\t Position: (" << pos.x << ", " << pos.y << ", " << pos.z << ")" << std::endl;
	glm::vec4 col = this->lightColor;
	ss << "\t Light Color: (" << col.x << ", " << col.y << ", " << col.z << ")" << std::endl;
	glm::vec4 dir = this->direction;
	ss << "\t Direction: (" << dir.x << ", " << dir.y << ", " << dir.z << ")" << std::endl;

	ss << "\t Strength: " << OPTIONS_STENGTH(this) << std::endl;
	ss << "\t Constant: " << OPTIONS_CONSTANT(this) << std::endl;
	ss << "\t Linear: " << OPTIONS_LINEAR(this) << std::endl;
	ss << "\t Quadratic: " << OPTIONS_QUADRATIC(this) << std::endl;
	ss << "\t Cutoff: " << OPTIONS_CUTOFF(this) << std::endl;
	ss << "\t Outer Cutoff: " << OPTIONS_OUTER_CUTOFF(this) << std::endl;

	return ss.str();
}
