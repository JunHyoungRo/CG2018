#version 330 core
// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertexPosition_modelspace;

// Values that stay constant for the whole mesh.

uniform mat4 MVP;
out vec3 colour;

void main(){

//    gl_Position.xyz = vertexPosition_modelspace;
//    gl_Position.w = 1.0;

// Output position of the vertex, in clip space : MVP * position

	gl_Position =  MVP * vec4(vertexPosition_modelspace,1);
	colour = (vertexPosition_modelspace*3.309f);


}