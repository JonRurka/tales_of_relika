#version 450 core

//#include "../include/lights.inc"
#ifndef _LIGHTS_INC_
#define _LIGHTS_INC_

const int MAX_LIGHTS = 99; 

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

#define LIGHT_TYPE_DIRECTIONAL 	1
#define LIGHT_TYPE_POINT 		2
#define LIGHT_TYPE_SPOT 		3

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

//uniform Light lights;
layout (std140, binding = 1) uniform LightBlock {
    Light lights[MAX_LIGHTS];
};

vec3 get_light(Light p_light, vec3 normal, vec3 view_pos, vec3 frag_pos, vec3 diffuseColor, float shininess, float specular_intensity, vec3 spec_val){
	
	vec3 lightPos = p_light.position.xyz;
	vec3 norm = normalize(normal);
	vec3 viewDir = normalize(view_pos - frag_pos);
	
	//Light_Result res;
	vec3 lightDir = vec3(0.0f);
	vec3 lightColor = p_light.lightColor.xyz;
	float intensity = 0;
	
	if (OPTIONS_TYPE(p_light) == LIGHT_TYPE_DIRECTIONAL){
		lightDir = normalize(-p_light.direction.xyz);
		intensity = OPTIONS_STENGTH(p_light);
	}
	else if (OPTIONS_TYPE(p_light) == LIGHT_TYPE_POINT){
		lightDir = normalize(lightPos - frag_pos);
		float light_dist = distance(lightPos, frag_pos);
		float attenuation = 1.0 / (OPTIONS_CONSTANT(p_light) + (OPTIONS_LINEAR(p_light) * light_dist) + (OPTIONS_QUADRATIC(p_light) * (light_dist * light_dist)));
		intensity = OPTIONS_STENGTH(p_light) * attenuation;
	}
	else if (OPTIONS_TYPE(p_light) == LIGHT_TYPE_SPOT){
		lightDir =  normalize(lightPos - frag_pos);
		
		float light_dist = distance(lightPos, frag_pos);
		float attenuation = OPTIONS_STENGTH(p_light) / (OPTIONS_CONSTANT(p_light) + (OPTIONS_LINEAR(p_light) * light_dist) + (OPTIONS_QUADRATIC(p_light) * (light_dist * light_dist)));
		
		float theta = dot(lightDir, normalize(-p_light.direction.xyz));
		float epsilon = OPTIONS_CUTOFF(p_light) - OPTIONS_OUTER_CUTOFF(p_light);
		intensity = attenuation * clamp((theta - OPTIONS_OUTER_CUTOFF(p_light)) / epsilon, 0.0, 1.0);  
		/*if (theta > p_light.cutOff){
			float light_dist = distance(lightPos, frag_pos);
			float attenuation = 1.0 / (p_light.constant + (p_light.linear * light_dist) + (p_light.quadratic * (light_dist * light_dist)));
			res.intensity = p_light.strength * attenuation;
		}*/
	}
	
	// diffuse
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = intensity * lightColor * diff * diffuseColor;
	
	// specular
	vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specular = intensity * specular_intensity * lightColor * spec * spec_val;
	
	return vec3(diffuse + specular);
}


vec3 Calculate_Lighting(vec3 normal, vec3 view_pos, vec3 frag_pos, vec3 diffuseColor, float shininess, float specular_intensity, vec3 spec_val)
{
	vec3 resColor = vec3(0.0f);
	
	int num_lights = NUM_LIGHTS(lights);
	for (int i = 0; i < num_lights; i++)
	{
		resColor += get_light(lights[i], normal, view_pos, frag_pos, diffuseColor, shininess, specular_intensity, spec_val) * OPTIONS_ENABLED(lights[i]);
	}
	
	return resColor;
}


#endif // _LIGHTS_INC_


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


layout (location = 0) in vec3 FragPos;
layout (location = 1) in vec3 Normal;
layout (location = 2) in vec3 Tangent;
layout (location = 3) in vec3 Bitangent;
layout (location = 4) in vec2 TexCoords;
layout (location = 5) in vec4 Mats;
//layout (location = 2) in vec3 Color;
//layout (location = 3) in vec2 TexCoords;


layout (location = 0) out vec4 FragColor;

layout (location = 10) uniform vec3 globalAmbientLightColor;
layout (location = 11) uniform float globalAmbientIntensity;
layout (location = 12) uniform vec3 viewPos;
layout (location = 13) uniform Material material;

uniform sampler2DArray diffuse;
uniform sampler2DArray normal_maps;



struct SampleResult{
	vec4 color;
	vec3 normal;
};

SampleResult lerp_sample(SampleResult val_1, SampleResult val_2, float dt){
	val_1.color = mix(val_1.color, val_2.color, dt);
	val_1.normal = mix(val_1.normal, val_2.normal, dt);
	return val_1;
}

vec3 UnpackNormal(vec3 normalTex){
	return -normalize(normalTex.xyz * 2.0 - 1.0);
}

vec3 TranslateNormal(vec3 vertex_normal, vec3 vertex_tangent, vec3 vertex_bitangent, vec3 tangent_normal){
	mat3 TBN = mat3(vertex_tangent, vertex_bitangent, vertex_normal);
	return normalize(TBN * tangent_normal);
}

SampleResult PlainSample(vec2 pos, float bf, vec3 tint_color, int layer)
{	
	vec4 sample_col = vec4(texture(diffuse, vec3(pos.xy, layer)));
	vec4 sample_norm = vec4(texture(normal_maps, vec3(pos.xy, layer)));
	
	vec4 col = sample_col * vec4(tint_color.xyz, 1.0) * vec4(bf);
	vec4 norm = sample_norm * vec4(bf);
	
	SampleResult res;
	res.color = col;
	res.normal = norm.xyz;
	return res;
}

SampleResult TriSample(vec3 pos, vec3 normal, float scale, vec3 tint_color, int layer){
	
	// Blending factor of triplanar mapping
    vec3 bf = normalize(abs(normal));
    bf /= dot(bf, vec3(1, 1, 1));
	
	// Triplanar mapping
    vec2 tx = pos.yz * scale;
    vec2 ty = pos.zx * scale;
    vec2 tz = pos.xy * scale;
	
	SampleResult rx = PlainSample(tx, bf.x, tint_color, layer);
	SampleResult ry = PlainSample(ty, bf.y, tint_color, layer);
	SampleResult rz = PlainSample(tz, bf.z, tint_color, layer);
	
	vec4 res_color = (rx.color + ry.color + rz.color) * vec4(tint_color, 1);
	vec3 res_normal = UnpackNormal((rx.normal + ry.normal + rz.normal) / 3); // TODO: Better method of normal blending
	
	SampleResult res;
	res.color = res_color;
	res.normal = res_normal;
	return res;
}

SampleResult MatSample(vec3 pos, vec3 normal, vec2 blend_coords, vec4 material_ids){
	
	int mat_idx_1 = int(round(material_ids.x));
	int mat_idx_2 = int(round(material_ids.y));
	int mat_idx_3 = int(round(material_ids.z));
	
	SampleResult s1 = TriSample(pos, normal, 0.25f, vec3(1.0, 1.0, 1.0), mat_idx_1);
	SampleResult s2 = TriSample(pos, normal, 0.25f, vec3(1.0, 1.0, 1.0), mat_idx_2);
	SampleResult s3 = TriSample(pos, normal, 0.25f, vec3(1.0, 1.0, 1.0), mat_idx_3);
	
	SampleResult blend_x = lerp_sample(s1, s2, blend_coords.x);
	SampleResult sample_val = lerp_sample(blend_x, s3, blend_coords.y);

	return sample_val;
}


void main()
{
	vec3 resColor = vec3(0.0f);
	
	vec3 ambient = globalAmbientIntensity * globalAmbientLightColor * material.ambientColor;
	//vec3 ambient = 1.0 * globalAmbientLightColor * vec3(1.0f) * vec3(texture(material.diffuse, TexCoords));
	resColor += ambient;
	
	SampleResult sample_res = MatSample(FragPos.xyz, Normal, TexCoords, Mats);
	//SampleResult sample_res = TriSample(FragPos.xyz, Normal, 0.25f, vec3(1.0f, 1.0f, 1.0f), ivec3(2, 1, 2));
	
	vec3 diffuse_sample = sample_res.color.xyz;
	vec3 spec_sample = vec3(0.0f, 0.0f, 0.0f);
	
	vec3 frag_normal = TranslateNormal(Normal, Tangent, Bitangent, sample_res.normal);
	
	resColor += Calculate_Lighting(frag_normal, viewPos, FragPos, diffuse_sample, material.shininess, material.specular_intensity, spec_sample);
	
	
	//FragColor = vec4(resColor, 1.0f);
	FragColor = vec4(resColor, 1.0f);
}