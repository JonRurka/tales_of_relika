#pragma once

#include "Component.h"

#include "opengl.h"

#include <unordered_map>
#include <span>
#include <array>

#define MAX_LIGHTS 99

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader;

/*#define OPTIONS_ENABLED(data) ((data)->int_options_1.x)
#define OPTIONS_ALLOCATED(data) ((data)->int_options_1.y)
#define OPTIONS_TYPE(data) ((data)->int_options_1.z)

#define OPTIONS_STENGTH(data) ((data)->float_options_2.x)
#define OPTIONS_CONSTANT(data) ((data)->float_options_2.y)
#define OPTIONS_LINEAR(data) ((data)->float_options_2.z)

#define OPTIONS_QUADRATIC(data) ((data)->float_options_3.x)
#define OPTIONS_CUTOFF(data) ((data)->float_options_3.y)
#define OPTIONS_OUTER_CUTOFF(data) ((data)->float_options_3.z)*/


class Light : public Component
{
	friend class Shader;
public:
	typedef std::shared_ptr<Light> Shared;
	typedef std::weak_ptr<Light> Weak;

	enum class Light_Type {
		DIRECTIONAL = 1,
		POINT = 2,
		SPOT = 3,
	};

	/*void Enabled(bool val) { OPTIONS_ENABLED(m_data) = val ? 1 : 0; }
	bool Enabled() { return OPTIONS_ENABLED(m_data) > 0; }

	void Type(Light_Type val) { OPTIONS_TYPE(m_data) = (int)val;}
	Light_Type Type() { return (Light_Type)OPTIONS_TYPE(m_data); }

	void Color(glm::vec4 value) { m_data->lightColor = value;}
	glm::vec4 Color() { return m_data->lightColor; }

	void Strength(float value) { OPTIONS_STENGTH(m_data) = value;}
	float Strength() { return OPTIONS_STENGTH(m_data); }

	void Linear_Coefficient(float value) { OPTIONS_LINEAR(m_data) = value;}
	float Linear_Coefficient() { return OPTIONS_LINEAR(m_data); }

	void Quadratic_Coefficient(float value) { OPTIONS_QUADRATIC(m_data) = value;}
	float Quadratic_Coefficient() { return OPTIONS_QUADRATIC(m_data); }

	void CutOff(float value) { OPTIONS_CUTOFF(m_data) = value;}
	float CutOff() { return OPTIONS_CUTOFF(m_data); }

	void OuterCutOff(float value) { OPTIONS_OUTER_CUTOFF(m_data) = value;}
	float OuterCutOff() { return OPTIONS_OUTER_CUTOFF(m_data); }*/

	void Enabled(bool val) { m_option_enabled = val ? 1 : 0; option_updated();}
	bool Enabled() { return m_option_enabled > 0; }

	void Type(Light_Type val) { m_otions_type = val; option_updated();}
	Light_Type Type() { return m_otions_type; }

	void Color(glm::vec4 value) { m_option_color = value; option_updated();}
	glm::vec4 Color() { return m_option_color; }

	void Strength(float value) { m_option_strength = value; option_updated();}
	float Strength() { return m_option_strength; }

	void Linear_Coefficient(float value) { m_option_linear_coefficient = value; option_updated();}
	float Linear_Coefficient() { return m_option_linear_coefficient; }

	void Quadratic_Coefficient(float value) { m_option_quadratic_coefficient = value; option_updated();}
	float Quadratic_Coefficient() { return m_option_quadratic_coefficient; }

	void CutOff(float value) { m_option_cutOff = value; option_updated();}
	float CutOff() { return m_option_cutOff; }

	void OuterCutOff(float value) { m_option_outerCutOff = value; option_updated();}
	float OuterCutOff() { return m_option_outerCutOff; }

	void Activate_Shadows(bool enabled);

	static void Update_Lights(float dt);

	void Load(json data);

protected:
	void Init() override;

	void Update(float dt) override;

	void OnDestroy() override;

private:

	struct Light_Data {
		//int enabled;
		//int allocated;
		//int type;
		glm::ivec4 int_options_1; // enabled, allocated, type
		glm::vec4 position;
		glm::vec4 lightColor;
		glm::vec4 direction;

		//float strength;
		//float constant;
		//float linear;
		glm::vec4 float_options_2; // strength, constant, linear

		//float quadratic;
		//float cutOff;
		//float outerCutOff;
		glm::vec4 float_options_3; // quadratic, cutOff, outerCutOff

		std::string print_data();
	};

	//Light_Data* m_data{ nullptr };
	//Light_Data m_data;
	int m_id{ 0 };

	// Options
	int m_option_enabled{ true };
	Light_Type m_otions_type{ Light_Type::DIRECTIONAL };
	glm::vec4 m_option_color{ glm::vec4() };
	float m_option_strength{ 0 };
	float m_option_linear_coefficient{ 0 };
	float m_option_quadratic_coefficient{ 0 };
	float m_option_cutOff{ 0 };
	float m_option_outerCutOff{ 0 };

	bool m_has_change{ false };



	static bool m_initialized;
	static GLuint m_uboLights;
	static bool m_lights_inited;
	//static Light_Data m_light_data[MAX_LIGHTS];
	inline static std::array<Light_Data, MAX_LIGHTS> m_light_data;
	static int m_num_lights;
	static int curr_index;
	static std::vector<Shader*> m_light_shaders;
	static std::unordered_map<int, std::weak_ptr<Light>> m_linked_lights;
	//static std::vector<Light*> m_lights;

	const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
	GLuint depthMapFBO{ 0 };
	GLuint depthMap{ 0 };
	bool has_shadows{ false };
	
	inline static double debug_timer = 1.0f;

	void option_updated();

	Light_Data sync();

	static void Flush();

	static void Add_Shader(Shader* value);

	//static void Bind();

	static Light_Data Allocate_Light(std::weak_ptr<Light> light, int& id);

	static void Deallocate_Light(int id);

	static Light_Data Default_Light_Data();

	inline static const std::string LOG_LOC{ "LIGHT" };
};

/*#undef OPTIONS_ENABLED
#undef OPTIONS_ALLOCATED
#undef OPTIONS_TYPE

#undef OPTIONS_STENGTH
#undef OPTIONS_CONSTANT
#undef OPTIONS_LINEAR

#undef OPTIONS_QUADRATIC
#undef OPTIONS_CUTOFF
#undef OPTIONS_OUTER_CUTOFF*/


