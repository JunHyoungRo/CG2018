#pragma once
#include "glm/glm.hpp"
#include "GLFW/glfw3.h"
#include <string>
#include <iostream>

const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
const int CONFIG_WINDOW_HEIGHT = 1080;
const int CONFIG_WINDOW_WIDTH = 1920;
const std::string CONFIG_WINDOW_TITLE = "2013112003";
const int CONFIG_MAX_FPS = 60;
const int CONFIG_ASPECT_RATIO = (float)CONFIG_WINDOW_WIDTH / (float)CONFIG_WINDOW_HEIGHT;
const float CONFIG_DRAG = 0.612489;
const bool CONFIG_DEBUG = false;
const float CONFIG_MONSTER_GEN_TIME = 3;
const float CONFIG_ANIMATE_MAX = 0.05;
const int CONFIG_DEAD_FLAG = 10;
const glm::vec3 CONFIG_GRAVITY = glm::vec3(0, -9.8f, 0);
float g_last_time = 0.0f;
float g_accum_time = 0;
float g_delta_time = 0.0f;
float g_start_time = 0.0f;
float g_current_time = 0.0f;
float g_current_fps = 0.0f;

void print_glm_vec3(glm::vec3 t) {
	std::cout << "x:" << t.x << " y:" << t.y << " z:" << t.z << std::endl;
}

float rand_float(float a, float b) {
	float random = ((float)rand()) / (float)RAND_MAX;
	float diff = b - a;
	float r = random * diff;
	return a + r;
}