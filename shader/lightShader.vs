#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aColor;
layout (location = 3) in vec2 aTexCoords;

out VS_OUT{
	vec3 FragPos;
	vec3 Normal;
	vec3 Color;
	vec2 TexCoords;
} vs_out;


uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform bool shader;
void main()
{
	vs_out.FragPos = vec3(model * vec4(aPos, 1.0));
	if(shader) vs_out.Normal = mat3(transpose(inverse(model))) * aNormal; 
	else vs_out.Normal = vec3(0.0,0.0,0.0);
	vs_out.Color = vec3(aColor.x,aColor.y,aColor.z);
	vs_out.TexCoords = vec2(aTexCoords.x, aTexCoords.y);
	gl_Position = projection * view * vec4(vs_out.FragPos, 1.0);
	
}