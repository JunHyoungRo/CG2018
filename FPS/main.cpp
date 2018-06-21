#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <ctime>
#include <Windows.h>
#include <mmsystem.h>
#pragma comment(lib,"Winmm.lib")


#include "GL/glew.h"
#include "GL/glut.h"
#include "GLFW/glfw3.h"
#include "GL/GLU.h"
#include "btBulletDynamicsCommon.h"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/quaternion.hpp"
#include "ft2build.h"
#include FT_FREETYPE_H

#include "GL2_Camera.hpp"
#include "Config.hpp"
#include "Shader.hpp"
#include "Model.hpp"
#include "World.hpp"
#include "Debug_Draw.hpp"
#include "Character.hpp"

/*****************************************************************/
/*****************************************************************/

using namespace std;
World world;


static string status = "stop";
static float move_stat = 0;
static bool flag = false;
void aminmate_gun(float time_, World& world) {
	if (flag == true) {
		if (status == "stop") {
			status = "up";
		}
		else if (status == "up") {
			if (move_stat >= CONFIG_ANIMATE_MAX) {
				status = "down";
				move_stat = CONFIG_ANIMATE_MAX;
			}
			else {
				move_stat += time_;
				world.m_fixed[0].translate(time_*glm::vec3(1, 1, 0));
			}
		}
		else if (status == "down") {
			if (move_stat <= 0) {
				status = "stop";
				move_stat = 0;
				flag = false;
			}
			else {
				move_stat -= time_;
				world.m_fixed[0].translate(-time_ * glm::vec3(1, 1, 0));
			}
		}
		for (auto &itr : world.m_fixed) {
			itr.update_meshes();
		}
	}
	
}

// callback funcions
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	world.m_camera.resize(width, height);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	using namespace std;

	if (action == GLFW_PRESS) {
		if (key == GLFW_KEY_W) {
			if (world.m_camera.m_status == Camera_move_status::LEFT) {
				world.m_camera.m_status = Camera_move_status::LEFT_FORWARD;
			}
			else if (world.m_camera.m_status == Camera_move_status::RIGHT) {
				world.m_camera.m_status = Camera_move_status::RIGHT_FORWARD;
			}
			else {
				world.m_camera.m_status = Camera_move_status::FORWARD;
			}
		}
		if (key == GLFW_KEY_A) {
			if (world.m_camera.m_status == Camera_move_status::FORWARD) {
				world.m_camera.m_status = Camera_move_status::LEFT_FORWARD;
			}
			else if (world.m_camera.m_status == Camera_move_status::BACKWARD) {
				world.m_camera.m_status = Camera_move_status::LEFT_BACKWARD;
			}
			else {
				world.m_camera.m_status = Camera_move_status::LEFT;
			}
		}
		if (key == GLFW_KEY_S) {
			if (world.m_camera.m_status == Camera_move_status::LEFT) {
				world.m_camera.m_status = Camera_move_status::LEFT_BACKWARD;
			}
			else if (world.m_camera.m_status == Camera_move_status::RIGHT) {
				world.m_camera.m_status = Camera_move_status::RIGHT_BACKWARD;
			}
			else {
				world.m_camera.m_status = Camera_move_status::BACKWARD;
			}
		}
		if (key == GLFW_KEY_D) {
			if (world.m_camera.m_status == Camera_move_status::FORWARD) {
				world.m_camera.m_status = Camera_move_status::RIGHT_FORWARD;
			}
			else if (world.m_camera.m_status == Camera_move_status::BACKWARD) {
				world.m_camera.m_status = Camera_move_status::RIGHT_BACKWARD;
			}
			else {
				world.m_camera.m_status = Camera_move_status::RIGHT;
			}
		}
		if (key == GLFW_KEY_LEFT_SHIFT) {
			if (world.m_camera.m_status == Camera_move_status::FORWARD) {
				world.m_camera.m_status = Camera_move_status::FAST_FORAWRD;
			}
		}
	}
	else if (action == GLFW_RELEASE) {
		if (key == GLFW_KEY_W) {
			if (world.m_camera.m_status == Camera_move_status::LEFT_FORWARD) {
				world.m_camera.m_status = Camera_move_status::LEFT;
			}
			else if (world.m_camera.m_status == Camera_move_status::RIGHT_FORWARD) {
				world.m_camera.m_status = Camera_move_status::RIGHT;
			}
			else {
				world.m_camera.m_status = Camera_move_status::STOP;
			}
		}
		if (key == GLFW_KEY_A) {
			if (world.m_camera.m_status == Camera_move_status::LEFT_FORWARD) {
				world.m_camera.m_status = Camera_move_status::FORWARD;
			}
			else if (world.m_camera.m_status == Camera_move_status::LEFT_BACKWARD) {
				world.m_camera.m_status = Camera_move_status::BACKWARD;
			}
			else {
				world.m_camera.m_status = Camera_move_status::STOP;
			}
		}
		if (key == GLFW_KEY_S) {
			if (world.m_camera.m_status == Camera_move_status::LEFT_BACKWARD) {
				world.m_camera.m_status = Camera_move_status::LEFT;
			}
			else if (world.m_camera.m_status == Camera_move_status::RIGHT_BACKWARD) {
				world.m_camera.m_status = Camera_move_status::RIGHT;
			}
			else {
				world.m_camera.m_status = Camera_move_status::STOP;
			}
		}
		if (key == GLFW_KEY_D) {
			if (world.m_camera.m_status == Camera_move_status::RIGHT_FORWARD) {
				world.m_camera.m_status = Camera_move_status::FORWARD;
			}
			else if (world.m_camera.m_status == Camera_move_status::RIGHT_BACKWARD) {
				world.m_camera.m_status = Camera_move_status::BACKWARD;
			}
			else {
				world.m_camera.m_status = Camera_move_status::STOP;
			}
		}
		if (key == GLFW_KEY_LEFT_SHIFT) {
			if (world.m_camera.m_status == Camera_move_status::FAST_FORAWRD) {
				world.m_camera.m_status = Camera_move_status::FORWARD;
			}
		}
	}
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	world.m_camera.update_mouse_pos(window, xpos, ypos);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	PlaySound(TEXT("../Assets/effect.wav"), NULL, SND_FILENAME | SND_ASYNC);
	//sndPlaySoundA("../Assets/effect.wav", SND_ASYNC | SND_FILENAME | SND_LOOP);
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		flag = true;
		if (CONFIG_DEBUG) {
			std::cout << "SHOOT!!" << std::endl;
		}
		Model bullet(world.m_objects[1]);
		bullet.m_id = rand();
		bullet.setup_meshes();
		bullet.rotate(glm::radians(world.m_camera.get_pitch()), glm::vec3(1, 0, 0));
		bullet.rotate(glm::radians(-world.m_camera.get_yaw() - 90), glm::vec3(0, 1, 0));
		bullet.update_meshes();
		bullet.init_physics(
			world.m_dynamicsWorld,
			world.m_camera.get_pos(),
			world.m_camera.get_direction(),
			200.0f,
			10
		);


		//// Push to Monster Vector!!!!
		world.m_bullets.push_back(bullet);
		world.m_bullets_life.push_back(g_current_time);


		// Userpointer test
		for (auto &itr : bullet.m_meshes) {
			itr.m_rigid_body->setUserPointer(&world.m_bullets[world.m_bullets.size() - 1]);
		}

	}
}
// Callback funcions end

void draw_axes() {
	//const GLfloat triangle_vertices[] = {
	//	0.0f, 0.0f, 0.0f,
	//	1.0f, 0.0f, 0.0f,
	//	0.0f, 0.0f, 0.0f,
	//	0.0f, 1.0f, 0.0f,
	//	0.0f, 0.0f, 0.0f,
	//	0.0f, 0.0f, 1.0f,
	//};


	//GLuint vertexbuffer;
	//glGenBuffers(1, &vertexbuffer);
	//glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(triangle_vertices), triangle_vertices, GL_STATIC_DRAW);
	//glEnableVertexAttribArray(0);
	//glVertexAttribPointer(
	//	0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
	//	3,                  // size
	//	GL_FLOAT,           // type
	//	GL_FALSE,           // normalized?
	//	0,                  // stride
	//	(void*)0            // array buffer offset
	//);

	//// Draw the triangle !
	//glLineWidth(100);
	//glDrawArrays(GL_LINES, 0, 6); // 6 vertices
	//glDeleteBuffers(1, &vertexbuffer);
}

// bullet physics collision callback func
bool collision_callback(btManifoldPoint& cp, const btCollisionObjectWrapper* colObj0Wrap, int partId0, int index0, const btCollisionObjectWrapper* colObj1Wrap, int partId1, int index1) {
	Model* obj1 = static_cast<Model*>(colObj0Wrap->getCollisionObject()->getUserPointer());
	Model* obj2 = static_cast<Model*>(colObj1Wrap->getCollisionObject()->getUserPointer());
	if (obj2 && obj1) {
		world.event_collision(obj1->m_id, obj2->m_id);
	}
	return false;
}

// monster generation
void gen_mob() {
	g_accum_time += g_delta_time;
	if (g_accum_time > CONFIG_MONSTER_GEN_TIME) {
		g_accum_time = 0;
		if (CONFIG_DEBUG) { std::cout << " time out!! moster gen.. "; }
		Model tmp_mob(world.m_objects[0]);
		tmp_mob.m_id = rand();
		tmp_mob.setup_meshes();
		tmp_mob.init_physics(
			world.m_dynamicsWorld,
			glm::vec3(0, 30, 0),
			glm::vec3(rand_float(0, 1), rand_float(0, 1), rand_float(0, 1)),
			rand_float(0, 40),
			rand_float(0, 50)
		);

		// push to MONSTER VECTOR!!!!
		world.m_monsters.push_back(tmp_mob);
		for (auto &itr : tmp_mob.m_meshes) {
			itr.m_rigid_body->setUserPointer(&world.m_monsters[world.m_monsters.size() - 1]);
		}
	}
}

int main() {

	/******* GLEW, GLFW init... **************************************/
	glewExperimental = true; // Needed for core profile
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		return -1;
	}

	gContactAddedCallback = collision_callback;
	srand(time(NULL));
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(CONFIG_WINDOW_WIDTH, CONFIG_WINDOW_HEIGHT, CONFIG_WINDOW_TITLE.c_str(), NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetCursorPos(window, CONFIG_WINDOW_WIDTH / 2, CONFIG_WINDOW_HEIGHT / 2);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// callback here
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);

	glewExperimental = true;


	glewExperimental = true; // Needed in core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		return -1;
	}
	fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));

	glClearColor(1, 1, 1, 1);
	std::cout << glGetString(GL_VERSION) << endl;
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_NORMALIZE);
	glShadeModel(GL_SMOOTH);
	glDisable(GL_COLOR_MATERIAL);
	glLoadIdentity();
	/******* Init END! ***********************************************/


	world.use_shader("vertex_shader.glsl", "fragment_shader.glsl");
	glUseProgram(world.m_shader_id);


	/************* Asset loading Start... ****************************/
	// set gun
	world.m_fixed.push_back(Model("../Assets/wapon.dae"));
	world.m_fixed[0].scale(0.2f);
	world.m_fixed[0].rotate(glm::radians(-100.0f), glm::vec3(1, 0, 0));
	world.m_fixed[0].rotate(glm::radians(190.0f), glm::vec3(0, 1, 0));
	world.m_fixed[0].rotate(glm::radians(-10.0f), glm::vec3(0, 0, 1));
	world.m_fixed[0].translate(glm::vec3(0.3f, -0.4f, -0.5f));
	for (auto &itr : world.m_fixed) {
		itr.update_meshes();
	}

	// main object
	world.m_objects.push_back(Model("../Assets/monster.dae"));
	world.m_objects[0].rotate(glm::radians(-90.0f), glm::vec3(1, 0, 0));
	world.m_objects.push_back(Model("../Assets/bullet.dae"));
	world.m_objects[1].rotate(glm::radians(-90.0f), glm::vec3(1, 0, 0));
	world.m_objects.push_back(Model("../Assets/particle.dae"));
	for (auto &itr : world.m_objects) {
		itr.update_meshes();
	}

	// set terrarians 
	world.m_terrarians.push_back(Model("../Assets/plane.dae"));
	world.m_terrarians.push_back(Model("../Assets/ship.dae"));
	world.m_terrarians[1].scale(2.0f);
	world.m_terrarians[1].rotate(glm::radians(-90.0f), glm::vec3(1, 0, 0));
	world.m_terrarians[1].translate(glm::vec3(0,30,0));
	world.m_terrarians.push_back(Model("../Assets/sky.dae"));
	for (auto &itr : world.m_terrarians) {
		itr.update_meshes();
	}

	cout << "Asseets load end " << endl;
	/********* Asset loading End... **********************************/


	world.init_physics();
	//GL_btDebugDraw bulletDebugugger;
	//bulletDebugugger.setDebugMode(btIDebugDraw::DBG_DrawWireframe);
	//world.m_dynamicsWorld->setDebugDrawer(&bulletDebugugger);
	//world.m_dynamicsWorld->getDebugDrawer()->setDebugMode(btIDebugDraw::DBG_DrawWireframe);



	/********** Text representing test *******************************/
	// set text represent 
	Shader chracter_shader;
	chracter_shader.load("chracter_vs.glsl", "chracter_fs.glsl");
	glm::mat4 tmp = world.m_camera.get_projection_mat();
	glUniformMatrix4fv(glGetUniformLocation(chracter_shader.m_program_id, "projection"), 1, GL_FALSE, glm::value_ptr(tmp));

	FT_Library ft;
	if (FT_Init_FreeType(&ft))
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;

	FT_Face face;
	if (FT_New_Face(ft, "C:/Windows/Fonts/Arial.ttf", 0, &face))
		std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
	FT_Set_Pixel_Sizes(face, 0, 48);
	// Load first 128 characters of ASCII set
	for (GLubyte c = 0; c < 128; c++)
	{
		// Load character glyph 
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
		{
			std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
			continue;
		}
		// Generate texture
		GLuint texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,
			face->glyph->bitmap.width,
			face->glyph->bitmap.rows,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			face->glyph->bitmap.buffer
		);
		// Set texture options
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// Now store character for later use
		Character character = {
			texture,
			glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
			face->glyph->advance.x
		};
		Characters.insert(std::pair<GLchar, Character>(c, character));
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	// Destroy FreeType once we're finished
	FT_Done_Face(face);
	FT_Done_FreeType(ft);
	/********* Text testing end **************************************/

	g_start_time = glfwGetTime();
	float tmp_total_time = 0;
	// render loop
	while (!glfwWindowShouldClose(window)) {
		aminmate_gun(g_delta_time, world);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(world.m_shader_id);
		world.m_camera.update_pos();
		world.update_physics(g_delta_time);
		world.draw();
		//world.m_dynamicsWorld->debugDrawWorld();

		
		gen_mob();
		glUseProgram(0);
		if (world.m_monsters.size() > CONFIG_DEAD_FLAG) break;

		RenderText(chracter_shader, "This is sample text", 25.0f, 25.0f, 1.0f, glm::vec3(0.5, 0.8f, 0.2f));
		RenderText(chracter_shader, "(C) LearnOpenGL.com", 540.0f, 570.0f, 0.5f, glm::vec3(0.3, 0.7f, 0.9f));

		glfwSwapBuffers(window);
		glfwPollEvents();

		// frame control
		g_current_time = glfwGetTime();
		g_delta_time = g_current_time - g_last_time;
		g_last_time = g_current_time;

		if (g_delta_time <= (1 / CONFIG_MAX_FPS)) {
			std::this_thread::sleep_for(std::chrono::duration<float>(1 / CONFIG_MAX_FPS));
			g_current_fps = 60;
		}
		else {
			g_current_fps = 1 / g_delta_time;
		}
	}
	cout << "게임종료! 당신의 점수는 " << g_current_time - g_start_time << " 입니다. 수고하셨습니다!" << endl;

	glfwTerminate();
	return 0;
}


