#version 330 core
layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec2 a_normal; 
layout (location = 2) in vec2 a_texture; 


const int MAX_BONES = 100;

uniform mat4 mvp;
uniform mat4 g_bones[MAX_BONES];

out vec2 uv;
out vec3 pos;

void main() {
	//mat4  bone_transform = g_bones[a_bone_id[0]]i;

    gl_Position = mvp * vec4(a_pos.x, a_pos.y, a_pos.z, 1.0);
	uv = a_texture;
	pos = a_pos;
}
