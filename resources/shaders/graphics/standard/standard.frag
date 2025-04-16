#version 450 core

#define LIGHT_TYPE_DIRECTIONAL 	1
#define LIGHT_TYPE_POINT 		2
#define LIGHT_TYPE_SPOT 		3

const int MAX_LIGHTS = 99; 

layout(location = 0)in vec3 Normal;
layout(location = 1)in vec3 Color;
layout(location = 2)in vec2 TexCoords;
layout(location = 3)in vec3 FragPos;

struct Material {
	sampler2D diffuse;
	sampler2D specular;
    vec3 ambientColor;
    vec3 diffuseColor;
	vec2 scale;
    //vec3 specular;
	float specular_intensity;
    float shininess;
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
	//int enabled;
	//int allocated;
	//int type;
	ivec4 int_options_1; // enabled, allocated, type
	vec4 position;
	vec4 lightColor;
	vec4 direction;

	//float strength;
	//float constant;
	//float linear;
	vec4 float_options_2; // strength, constant, linear

	//float quadratic;
	//float cutOff;
	//float outerCutOff;
	vec4 float_options_3; // quadratic, cutOff, outerCutOff
};


struct Test_Struct
{
	ivec4 int_options_1;
	vec4 position;
	vec4 lightColor;
	vec4 direction;
	vec4 float_options_2;
	vec4 float_options_3;
};

struct Light_Result{
	float intensity;
	vec4 lightColor;
	vec3 lightDir;
};

layout(location = 0) out vec4 FragColor;

uniform sampler2D texture1;
uniform sampler2D texture2;

layout(location = 0) uniform Material material;
//uniform Light lights;
layout (std140, binding = 1) uniform LightBlock {
    Light lights[MAX_LIGHTS];
};
//uniform int num_lights;

layout (std140, binding = 2) uniform Test {
    Test_Struct test_val[10];
};

layout(location = 1) uniform vec3 globalAmbientLightColor;
layout(location = 2) uniform float globalAmbientIntensity;

layout(location = 3) uniform vec3 ambientLightColor;
layout(location = 4) uniform vec3 diffuseLightColor;

//uniform vec3 lightColor;
//uniform vec3 lightPos;

layout(location = 5) uniform vec3 viewPos;


float near = 0.1; 
float far  = 100.0; 
  
float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));	
}


vec3 get_light(Light p_light){
	
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
    vec3 diffuse = intensity * lightColor * material.diffuseColor * diff * vec3(texture(material.diffuse, vec2(TexCoords.x * material.scale.x, TexCoords.y * material.scale.y)));
	
	// specular
	vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = intensity * material.specular_intensity * lightColor * spec /*material.specular*/ * vec3(texture(material.specular, vec2(TexCoords.x * material.scale.x, TexCoords.y * material.scale.y)));
	
	return vec3(diffuse + specular);
}

void main()
{
	vec3 resColor = vec3(0.0f);
	
	// ambient
	vec3 ambient = globalAmbientIntensity * globalAmbientLightColor * material.ambientColor * vec3(texture(material.diffuse, vec2(TexCoords.x * material.scale.x, TexCoords.y * material.scale.y)));
	//vec3 ambient = 1.0 * globalAmbientLightColor * vec3(1.0f) * vec3(texture(material.diffuse, TexCoords));
	resColor += ambient;
	
	int num_lights = NUM_LIGHTS(lights);
	
	for (int i = 0; i < num_lights; i++)
	{
		resColor += get_light(lights[i]) * OPTIONS_ENABLED(lights[i]);
	}

	//FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
	
	FragColor = vec4(resColor, 1.0f);
	
	//float depth = LinearizeDepth(gl_FragCoord.z) / far;
	//FragColor = vec4(vec3(depth), 1.0);

	//int ind = test_val[0].int_options_1.w;
    //FragColor = test_val[ind].float_options_3;
}


/*void main_old()
{
	//vec3 lightColor = light.lightColor;
	//vec3 lightPos = light.position;
	
	//float intensity = light.strength / 1.0;// pow(distance(lightPos, FragPos), 2);
	//float light_dist = distance(lightPos, FragPos);
	//float attenuation = 1.0 / (light.constant + (light.linear * light_dist) + (light.quadratic * (light_dist * light_dist)));
	//float intensity = light.strength * attenuation;
	//float intensity = light.strength /(1 + pow(light_dist, 2));
	
	//vec3 lightDir = normalize(-light.direction);
	//vec3 lightDir =  normalize(lightPos - FragPos);
	
	vec3 norm = normalize(Normal);
	vec3 viewDir = normalize(viewPos - FragPos);
	
	float intensity = 0;
	vec3 Color = vec3(0.0f);
	
	Light_Result l_res = get_light(lights[0]);
	intensity += l_res.intensity;
	vec3 lightColor = l_res.lightColor.xyz;
	vec3 lightDir = l_res.lightDir;
	
	// ambient
	vec3 ambient = globalAmbientIntensity * globalAmbientLightColor * material.ambientColor * vec3(texture(material.diffuse, TexCoords));
	
	// diffuse
    //float diff = max(dot(norm, lightDir), 0.0);
    //vec3 diffuse = intensity * lightColor * material.diffuseColor * diff * vec3(texture(material.diffuse, TexCoords));
	
	// specular
	vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = intensity * material.specular_intensity * lightColor * spec * vec3(texture(material.specular, TexCoords));
	
	vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
	//FragColor = test_val[2];
	
}*/


/*void main_old()
{
	float ambientStrength = 0.15;
	vec3 ambient = ambientStrength * ambientLightColor;
	
	
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(lightPos - FragPos);
	
	float strength = 10.0f;
	float intensity = strength / pow(distance(lightPos, FragPos), 2);
	
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = diff * intensity * diffuseLightColor;
	
	float specularStrength = 1;
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);

	float reflectivity = 32;
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), reflectivity);
	vec3 specular = specularStrength * intensity * spec * diffuseLightColor;  	
	
	
	vec3 result = (ambient + diffuse + specular) * Color;
	FragColor = vec4(result, 1);
    //FragColor = texture(ourTexture, TexCoord);
	//FragColor = texture(texture1, TexCoord) * vec4(ourColor, 1.0);
	//FragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), 0.2);
}*/