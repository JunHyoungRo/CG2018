
#include <GL/glew.h>
#include <GLFW/glfw3.h>
//#include <GL/GLU.h>
#include <cstring>
#include <stdlib.h>		  // srand, rand
#include <thread>         // std::this_thread::sleep_for
#include <chrono>         // std::chrono::seconds
#include <iostream>
#include "math.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include "Matrix4.h"
#include <fstream>
#include <string>
#include <sstream>
#include "Vector3.h"
#include "ObjReader.h"
#include "shader.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
GLFWwindow* window;
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace std;
using namespace glm;

#include "shader.hpp"

class Box
{
public:
	vec3 verticies[36];
	vector<vec3> indicies;
	Box(float size) {
		
		//verticies
		float s = size/2;
		/*
		verticies[0] = vec3(s, s, -s);
		verticies[1] = vec3(-s, s, -s);
		verticies[2] = vec3(-s, -s, -s);
		verticies[3] = vec3(s, -s, -s);
		verticies[4] = vec3(s, s, s);
		verticies[5] = vec3(-s, s, s);
		verticies[6] = vec3(-s, -s, s);
		verticies[7] = vec3(s, -s, s);
		*/

		verticies[0] = vec3(s, s, -s);
		verticies[1] = vec3(-s, -s, -s);
		verticies[2] = vec3(-s, s, -s);
		verticies[3] = vec3(s, s, -s);
		verticies[4] = vec3(s, -s, -s);
		verticies[5] = vec3(-s, -s, -s);
		verticies[6] = vec3(s, s, s);
		verticies[7] = vec3(-s, s, s);
		verticies[8] = vec3(-s, -s, s);
		verticies[9] = vec3(s, s, s);
		verticies[10] = vec3(-s, -s, s);
		verticies[11] = vec3(s, -s, s);
		verticies[12] = vec3(s, s, -s);
		verticies[13] = vec3(-s, s, -s);
		verticies[14] = vec3(-s, s, s);
		verticies[15] = vec3(s, s, -s);
		verticies[16] = vec3(-s, s, s);
		verticies[17] = vec3(s, s, s);
		verticies[18] = vec3(s, -s, -s);
		verticies[19] = vec3(-s, -s, s);
		verticies[20] = vec3(-s, -s, -s);
		verticies[21] = vec3(s, -s, -s);
		verticies[22] = vec3(s, -s, s);
		verticies[23] = vec3(-s, -s, s);
		verticies[24] = vec3(s, s, -s);
		verticies[25] = vec3(s, s, s);
		verticies[26] = vec3(s, -s, s);
		verticies[27] = vec3(s, s, -s);
		verticies[28] = vec3(s, -s, s);
		verticies[29] = vec3(s, -s, -s);
		verticies[30] = vec3(-s, s, -s);
		verticies[31] = vec3(-s, -s, -s);
		verticies[32] = vec3(-s, s, s);
		verticies[33] = vec3(-s, s, s);
		verticies[34] = vec3(-s, -s, -s);
		verticies[35] = vec3(-s, -s, s);

	};
	~Box() {};

private:

};

float degree_2_rad(float deg) {
	return (3.141592 / 180)*deg;
}


int main(void)
{
	// Initialise GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		getchar();
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow(1024, 768, "Tutorial 02 - Red triangle", NULL, NULL);
	if (window == NULL) {
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// Dark blue background
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// Create and compile our GLSL program from the shaders
	GLuint programID = LoadShaders("SimpleVertexShader.glsl", "SimpleFragmentShader.glsl");

	//Box box(0.4);
	float deg = 0;
	OBJReader obj_reader;
	obj_reader.readObj("../Dragon.obj");


	// GLuint box_buffer;
	// glGenBuffers(1, &box_buffer);
	// glBindBuffer(GL_ARRAY_BUFFER, box_buffer);
	// glBufferData(GL_ARRAY_BUFFER, sizeof(box.verticies), box.verticies, GL_STATIC_DRAW);

	GLuint dragon_buffer; 
	glGenBuffers(1, &dragon_buffer); //버퍼생성
	glBindBuffer(GL_ARRAY_BUFFER, dragon_buffer);
	glBufferData(GL_ARRAY_BUFFER, obj_reader.pos_stack_.size() *sizeof(float)*3,
	&obj_reader.pos_stack_[0],GL_STATIC_DRAW);

	do {
		glEnable(GL_DEPTH_TEST);
		deg+=1;
		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

		// Use our shader
		glUseProgram(programID);

		GLint colorID = glGetUniformLocation(programID, "color");
		float color[3] = { 1.0f, 0.8f, 0.1f };
		glProgramUniform3fv(programID, colorID, 1, color);
		GLuint MatrixID = glGetUniformLocation(programID, "MVP");
		//glm::mat4 Projection = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);
		glm::mat4 Projection = glm::ortho(-1.0f,1.0f,-1.0f,1.0f,0.0f,100.0f); // In world coordinates

		// Camera matrix
		glm::mat4 View = glm::lookAt(
			glm::vec3(0, 0, 3), // Camera is at (4,3,3), in World Space
			glm::vec3(0, 0, 0), // and looks at the origin
			glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
		);


		glm::mat4 Model = glm::mat4(1.0f);
		//Model = glm::translate(Model, glm::vec3(obj_reader.center.x_, obj_reader.center.y_, -obj_reader.center.z_));
		glm::mat4 z_rot = glm::mat4(1.0f);
		z_rot[0][0] = cos(degree_2_rad(deg));
		z_rot[0][1] = -sin(degree_2_rad(deg));
		z_rot[1][0] = sin(degree_2_rad(deg));
		z_rot[1][1] = cos(degree_2_rad(deg));
		
		glm::mat4 x_rot = glm::mat4(1.0f);
		x_rot[1][1] = cos(degree_2_rad(deg*0.5));
		x_rot[1][2] = -sin(degree_2_rad(deg*0.5));
		x_rot[2][1] = sin(degree_2_rad(deg*0.5));
		x_rot[2][2] = cos(degree_2_rad(deg*0.5));

		glm::mat4 y_rot = glm::mat4(1.0f);
		y_rot[0][0] = cos(degree_2_rad(deg*0.9));
		y_rot[0][2] = sin(degree_2_rad(deg*0.9));
		y_rot[2][0] = -sin(degree_2_rad(deg*0.9));
		y_rot[2][2] = cos(degree_2_rad(deg*0.9));

		Model = Model*x_rot*y_rot*z_rot;
		//Model = Model*glm::mat4(1.0f);
		
		
		// glm::mat4 translate(1.0f);
		// translate[0][3] = -obj_reader.center.x_;
		// translate[1][3] = -obj_reader.center.y_;
		// translate[2][3] = -obj_reader.center.z_;
		
		//glm::mat4 mymat4x4 = glm::translate(trans);
		//Model = Model*translate;

		glm::mat4 MVP = Projection * View * Model;

		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

		
		
		// glEnableVertexAttribArray(0);
		// glBindBuffer(GL_ARRAY_BUFFER, box_buffer);
		// glVertexAttribPointer(
		// 	0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
		// 	3,                  // size
		// 	GL_FLOAT,           // type
		// 	GL_FALSE,           // normalized?
		// 	0,                  // stride
		// 	(void*)0            // array buffer offset
		// );
		// glDrawArrays(GL_TRIANGLES, 0, 36); // 6 vertices


		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, dragon_buffer);
		glVertexAttribPointer
		(
			0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
			3,                  // size 
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);
		glDrawArrays(GL_LINES, 0, obj_reader.pos_stack_.size()); // 6 vertices
		
		// Draw the triangle !
		
		glDisableVertexAttribArray(0);

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0);

	// Cleanup VBO
	glDeleteBuffers(1, &dragon_buffer);
	glDeleteVertexArrays(1, &VertexArrayID);
	glDeleteProgram(programID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}