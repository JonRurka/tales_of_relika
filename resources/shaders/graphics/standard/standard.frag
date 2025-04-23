#version 450 core


#define LIGHT_TYPE_DIRECTIONAL 	1
#define LIGHT_TYPE_POINT 		2
#define LIGHT_TYPE_SPOT 		3

const int MAX_LIGHTS = 1; 

layout (location = 0) in vec3 Normal;
layout (location = 1) in vec3 Color;
layout (location = 2) in vec2 TexCoords;
layout (location = 3) in vec3 FragPos;

layout (location = 0) out vec4 FragColor;

struct Material {
	//sampler2D 	diffuse;
	//sampler2D 	specular;
    vec3 		ambientColor;
    vec3 		diffuseColor;
	vec2 		scale;
    //vec3 		specular;
	float 		specular_intensity;
    float 		shininess;
};


#define NUM_LIGHTS(data) (data[0].int_options_1.w)
#define OPTIONS_ENABLED(data) ((data).int_options_1.x)
#define OPTIONS_ALLOCATED(data) ((data).int_options_1.y)
#define OPTIONS_TYPE(data) ((data).int_options_1.z)

#define OPTIONS_STENGTH(data) ((data).float_options_2.x)
#define OPTIONS_CONSTANT(data) ((data).float_options_2.y)
#define OPTIONS_LINEAR(data) ((data).float_options_2.z)

#define OPTIONS_QUADRATIC(data) ((data).float_options_3.x)
#define OPTIONS_CUTOFF(data) ((data).float_options_3.y)
#define OPTIONS_OUTER_CUTOFF(data) ((data).float_options_3.z)

struct Light {

	ivec4 int_options_1; // enabled, allocated, type
	vec4 position;
	vec4 lightColor;
	vec4 direction;
	vec4 float_options_2; // strength, constant, linear
	vec4 float_options_3; // quadratic, cutOff, outerCutOff
};

struct Light_Result{
	float intensity;
	vec4 lightColor;
	vec3 lightDir;
};



//uniform Light lights;
layout (std140, binding = 0) uniform LightBlock {
    Light lights[MAX_LIGHTS];
};


layout (location = 10) uniform vec3 globalAmbientLightColor;
layout (location = 11) uniform float globalAmbientIntensity;
layout (location = 12) uniform vec3 viewPos;

//layout (location = 13) uniform Material material;
layout (binding = 1) uniform sampler2D material_diffuse;
layout (binding = 2) uniform sampler2D material_specular;

layout (location = 20) uniform vec3 material_ambientColor;
layout (location = 21) uniform vec3 material_diffuseColor;
layout (location = 22) uniform vec2 material_scale;
//layout (location = 23) uniform //vec3 specular;
layout (location = 24) uniform float material_specular_intensity;
layout (location = 25) uniform float material_shininess;


Material get_material()
{
	Material res;
	//res.diffuse = material_diffuse;
	//res.specular = material_specular;
	res.ambientColor = material_ambientColor;
	res.diffuseColor = material_diffuseColor;
	res.scale = material_scale;
	//res.specular = ;
	res.specular_intensity = material_specular_intensity;
	res.shininess = material_shininess;
	return res;
}

vec3 get_light(Light p_light){
	
	Material material = get_material();
	
	vec3 lightPos = p_light.position.xyz;
	vec3 norm = normalize(Normal);
	vec3 viewDir = normalize(viewPos - FragPos);
	
	//Light_Result res;
	vec3 lightDir = vec3(0.0f);
	vec3 lightColor = p_light.lightColor.xyz;
	float intensity = 0;
	
	if (OPTIONS_TYPE(p_light) == LIGHT_TYPE_DIRECTIONAL){
		lightDir = normalize(-p_light.direction.xyz);
		intensity = OPTIONS_STENGTH(p_light);
	}
	else if (OPTIONS_TYPE(p_light) == LIGHT_TYPE_POINT){
		lightDir = normalize(lightPos - FragPos);
		float light_dist = distance(lightPos, FragPos);
		float attenuation = 1.0 / (OPTIONS_CONSTANT(p_light) + (OPTIONS_LINEAR(p_light) * light_dist) + (OPTIONS_QUADRATIC(p_light) * (light_dist * light_dist)));
		intensity = OPTIONS_STENGTH(p_light) * attenuation;
	}
	else if (OPTIONS_TYPE(p_light) == LIGHT_TYPE_SPOT){
		lightDir =  normalize(lightPos - FragPos);
		
		float light_dist = distance(lightPos, FragPos);
		float attenuation = OPTIONS_STENGTH(p_light) / (OPTIONS_CONSTANT(p_light) + (OPTIONS_LINEAR(p_light) * light_dist) + (OPTIONS_QUADRATIC(p_light) * (light_dist * light_dist)));
		
		float theta = dot(lightDir, normalize(-p_light.direction.xyz));
		float epsilon = OPTIONS_CUTOFF(p_light) - OPTIONS_OUTER_CUTOFF(p_light);
		intensity = attenuation * clamp((theta - OPTIONS_OUTER_CUTOFF(p_light)) / epsilon, 0.0, 1.0);  
		/*if (theta > p_light.cutOff){
			float light_dist = distance(lightPos, FragPos);
			float attenuation = 1.0 / (p_light.constant + (p_light.linear * light_dist) + (p_light.quadratic * (light_dist * light_dist)));
			res.intensity = p_light.strength * attenuation;
		}*/
	}
	
	// diffuse
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = intensity * lightColor * material.diffuseColor * diff * vec3(texture(material_diffuse, vec2(TexCoords.x * material.scale.x, TexCoords.y * material.scale.y)));
	
	// specular
	vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = intensity * material.specular_intensity * lightColor * spec /*material.specular*/ * vec3(texture(material_specular, vec2(TexCoords.x * material.scale.x, TexCoords.y * material.scale.y)));
	
	return vec3(diffuse + specular);
}

void main()
{
	Material material = get_material();
	
	vec3 resColor = vec3(0.0f);
	
	// ambient
	vec3 ambient = globalAmbientIntensity * globalAmbientLightColor * material.ambientColor * vec3(texture(material_diffuse, vec2(TexCoords.x * material.scale.x, TexCoords.y * material.scale.y)));
	//vec3 ambient = 1.0 * globalAmbientLightColor * vec3(1.0f) * vec3(texture(material.diffuse, TexCoords));
	resColor += ambient;
	
	int num_lights = NUM_LIGHTS(lights);
	
	for (int i = 0; i < num_lights; i++)
	{
		resColor += get_light(lights[i]) * OPTIONS_ENABLED(lights[i]);
	}

	FragColor = vec4(resColor, 1.0f);
}


