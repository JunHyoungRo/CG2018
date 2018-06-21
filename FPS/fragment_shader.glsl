#version 330 core
out vec4 FragColor;
uniform sampler2D material_texture_diffuse1;
uniform sampler2D material_texture_specular1;

in vec2 uv;
in vec3 pos;

void main() {
	//FragColor = vec4(pos.xyz*10.0f, 1.0f);
	FragColor = vec4(texture(material_texture_diffuse1, uv.xy).rgb, 1.0);
}