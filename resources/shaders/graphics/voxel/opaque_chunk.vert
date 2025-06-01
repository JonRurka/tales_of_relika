#version 450 core

layout (location = 0) in vec4 aPos;
layout (location = 1) in vec4 aNormal;
layout (location = 2) in vec4 aMat;
//layout (location = 2) in vec3 aColor;


layout (location = 0) out vec3 FragPos; 
layout (location = 1) out vec3 Normal;
layout (location = 2) out vec3 Tangent;
layout (location = 3) out vec3 Bitangent;
layout (location = 4) out vec2 TexCoords;
layout (location = 5) out vec4 Mats;
//layout (location = 2) out vec3 Color;
//

layout (location = 0) uniform mat3 normal_mat;
layout (location = 1) uniform mat4 model_mat;
layout (location = 2) uniform mat4 view_mat;
layout (location = 3) uniform mat4 projection_mat;

float inversesqrt(float val){
	return 1.0 / sqrt(val);
}

// from glm::quatLookAt()
mat3 LookAt(vec3 dir, vec3 up){
	mat3 res;
	res[2] = -dir;
	vec3 right = cross(up, -dir);
	res[0] = right * inversesqrt(max(0.00001, dot(right, right)));
	res[1] = cross(-dir, res[0]);
	return res;
}

void main()
{
    gl_Position = projection_mat * view_mat * model_mat * vec4(aPos.xyz, 1.0);
	FragPos = vec3(model_mat * vec4(aPos.xyz, 1.0));
	Normal = normal_mat * aNormal.xyz;
    TexCoords = vec2(aPos.w, aNormal.w);
	Mats = aMat;
	
	//mat3 TBN = LookAt(Normal, vec3(0, 1, 0));
	//Tangent = TBN[0];
	//Bitangent = TBN[1];
}