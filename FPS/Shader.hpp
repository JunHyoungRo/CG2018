#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>

#include "GLFW/glfw3.h"

using namespace std;


class Shader
{
public:
	Shader() {};
	~Shader() {};
	size_t load(const char * vertex_file_path, const char * fragment_file_path);
	
	void set_uniform(const std::string& name, const glm::mat4 matrix) {
		GLint location = glGetUniformLocation(m_program_id, name.c_str());
		if (location != -1) {
			glUniformMatrix4fv(location, 1, false, &matrix[0][0]);
		}
	}

	void set_uniform(const std::string& name, const bool value) {
		GLint location = glGetUniformLocation(m_program_id, name.c_str());
		if (location != -1) {
			glUniform1i(location, (int)value);
		}
	}

	void set_uniform(const std::string& name, const int value) {
		GLint location = glGetUniformLocation(m_program_id, name.c_str());
		if (location != -1) {
			glUniform1i(location, value);
		}
	}

	void set_uniform(const std::string& name, const float value) {
		GLint location = glGetUniformLocation(m_program_id, name.c_str());
		if (location != -1) {
			glUniform1i(location, value);
		}
	}
	
	GLuint m_program_id;
};

size_t Shader::load (const char * vertex_file_path, const char * fragment_file_path) {

	// Create the shaders
	GLuint vertex_shader_id = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragment_shader_id = glCreateShader(GL_FRAGMENT_SHADER);

	// Read the Vertex Shader code from the file
	std::string vertex_shader_code;
	std::ifstream vertex_shader_stream(vertex_file_path, std::ios::in);
	if (vertex_shader_stream.is_open()) {
		std::string Line = "";
		while (getline(vertex_shader_stream, Line))
			vertex_shader_code += "\n" + Line;
		vertex_shader_stream.close();
	}
	else {
		cout << "Impossible to open" << vertex_file_path << ".Are you in the right directory ?" << endl;
		return false;
	}

	// Read the Fragment Shader code from the file
	std::string fragment_shader_code;
	std::ifstream fragment_shader_stream(fragment_file_path, std::ios::in);
	if (fragment_shader_stream.is_open()) {
		std::string Line = "";
		while (getline(fragment_shader_stream, Line))
			fragment_shader_code += "\n" + Line;
		fragment_shader_stream.close();
	}

	GLint result = GL_FALSE;
	
	// Compile Vertex Shader
	cout << "Compiling shader : " << vertex_file_path << endl;
	char const * vertex_shader_pointer = vertex_shader_code.c_str();
	glShaderSource(vertex_shader_id, 1, &vertex_shader_pointer, NULL);
	glCompileShader(vertex_shader_id);

	// Check Vertex Shader
	glGetShaderiv(vertex_shader_id, GL_COMPILE_STATUS, &result);
	if (result == 0) {
		int info_log_length;

		glGetShaderiv(vertex_shader_id, GL_INFO_LOG_LENGTH, &info_log_length);
		char * vertex_shader_error_msg = new char[info_log_length + 1];
		glGetShaderInfoLog(vertex_shader_id, info_log_length, NULL, &vertex_shader_error_msg[0]);
		cout << string(vertex_shader_error_msg) << endl;
		delete[] vertex_shader_error_msg;
	}



	// Compile Fragment Shader
	cout << "Compiling shader : " << fragment_file_path << endl;
	char const * FragmentSourcePointer = fragment_shader_code.c_str();
	glShaderSource(fragment_shader_id, 1, &FragmentSourcePointer, NULL);
	glCompileShader(fragment_shader_id);

	// Check Fragment Shader
	glGetShaderiv(fragment_shader_id, GL_COMPILE_STATUS, &result);

	if (result == 0) {
		int info_log_length;

		glGetShaderiv(fragment_shader_id, GL_INFO_LOG_LENGTH, &info_log_length);
		char * fragment_shader_error_msg = new char[info_log_length + 1];
		glGetShaderInfoLog(fragment_shader_id, info_log_length, NULL, &fragment_shader_error_msg[0]);
		cout << string(fragment_shader_error_msg) << endl;
		delete[] fragment_shader_error_msg;
	}


	// Link the program
	cout << "Linking program" << endl;
	size_t shader_program_id = glCreateProgram();
	glAttachShader(shader_program_id, vertex_shader_id);
	glAttachShader(shader_program_id, fragment_shader_id);
	glLinkProgram(shader_program_id);

	// Check the program
	glGetProgramiv(shader_program_id, GL_LINK_STATUS, &result);
	if (result == 0) {
		int info_log_length;

		glGetShaderiv(fragment_shader_id, GL_INFO_LOG_LENGTH, &info_log_length);
		char * link_error_msg = new char[info_log_length + 1];
		glGetShaderInfoLog(fragment_shader_id, info_log_length, NULL, &link_error_msg[0]);
		cout << string(link_error_msg) << endl;
		delete[] link_error_msg;
	}

	glDetachShader(shader_program_id, vertex_shader_id);
	glDetachShader(shader_program_id, fragment_shader_id);

	glDeleteShader(vertex_shader_id);
	glDeleteShader(fragment_shader_id);

	m_program_id = shader_program_id;
	return shader_program_id;
}

