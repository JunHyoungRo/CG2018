#pragma once
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/quaternion.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/vec3.hpp"
#include "GLFW/glfw3.h"
extern float g_current_fps;

enum class Camera_move_status {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT,
	STOP,
	RIGHT_FORWARD,
	LEFT_FORWARD,
	RIGHT_BACKWARD,
	LEFT_BACKWARD,
	FAST_FORAWRD
};

class GL2_Camera
{
public:
	GL2_Camera() {};
	GL2_Camera(int width, int height, float fov_, float z_near, float z_far_);
	~GL2_Camera();

	glm::mat4 get_view_mat() {
		return glm::lookAt(m_cam_pos, m_cam_pos + m_forward_direction, m_up_direction);
	}

	glm::mat4 get_projection_mat() {
		return m_projection_mat;
	}

	glm::mat4 get_pv_mat() {
		return m_projection_mat
			* glm::lookAt(m_cam_pos, m_cam_pos + m_forward_direction, m_up_direction);
	}

	void set_projection_mat(const float fov_, const float aspect_, const float z_near_, const float z_far_) {
		m_projection_mat = glm::perspective(fov_, aspect_, z_near_, z_far_);
	}

	void update_pos(glm::vec3 pos_) {
		m_cam_pos += pos_;
	}

	void update_pos() {
		glm::vec3 groundFront;
		switch (m_status)
		{
		case Camera_move_status::FORWARD:
			//update_dolly(-m_key_sensitivity);
			groundFront = m_forward_direction;
			groundFront.y = 0;
			m_cam_pos += m_key_sensitivity * groundFront;
			break;
		case Camera_move_status::BACKWARD:
			//update_dolly(m_key_sensitivity);
			groundFront = m_forward_direction;
			groundFront.y = 0;
			m_cam_pos -= m_key_sensitivity * groundFront;
			break;
		case Camera_move_status::LEFT:
			//update_truck(m_key_sensitivity);
			groundFront = m_forward_direction;
			groundFront.y = 0;
			m_cam_pos -= glm::normalize(glm::cross(groundFront, m_up_direction)) * m_key_sensitivity;
			break;
		case Camera_move_status::RIGHT:
			//update_truck(-m_key_sensitivity);
			groundFront = m_forward_direction;
			groundFront.y = 0;
			m_cam_pos += glm::normalize(glm::cross(groundFront, m_up_direction)) * m_key_sensitivity;
			break;
		case Camera_move_status::STOP:
			break;
		case Camera_move_status::RIGHT_FORWARD:
			//update_truck(-m_key_sensitivity);
			//update_dolly(m_key_sensitivity);
			groundFront = m_forward_direction;
			groundFront.y = 0;
			m_cam_pos += m_key_sensitivity * groundFront;
			m_cam_pos += glm::normalize(glm::cross(groundFront, m_up_direction)) * m_key_sensitivity;
			break;
		case Camera_move_status::LEFT_FORWARD:
			//update_truck(m_key_sensitivity);
			//update_dolly(m_key_sensitivity);
			groundFront = m_forward_direction;
			groundFront.y = 0;
			m_cam_pos += m_key_sensitivity * groundFront;
			m_cam_pos -= glm::normalize(glm::cross(groundFront, m_up_direction)) * m_key_sensitivity;
			break;
		case Camera_move_status::RIGHT_BACKWARD:
			//update_truck(-m_key_sensitivity);
			//update_dolly(-m_key_sensitivity);
			groundFront = m_forward_direction;
			groundFront.y = 0;
			m_cam_pos -= m_key_sensitivity * groundFront;
			m_cam_pos += glm::normalize(glm::cross(groundFront, m_up_direction)) * m_key_sensitivity;
			break;
		case Camera_move_status::LEFT_BACKWARD:
			//update_truck(m_key_sensitivity);
			//update_dolly(-m_key_sensitivity);
			groundFront = m_forward_direction;
			groundFront.y = 0;
			m_cam_pos -= m_key_sensitivity * groundFront;
			m_cam_pos -= glm::normalize(glm::cross(groundFront, m_up_direction)) * m_key_sensitivity;
			break;
		case Camera_move_status::FAST_FORAWRD:
			//update_dolly(m_key_sensitivity*5);
			groundFront = m_forward_direction;
			groundFront.y = 0;
			m_cam_pos += m_key_sensitivity * 3 * groundFront;
			break;
		default:
			break;
		}

	}

	void update_dolly(float dolly_) {
		glm::vec4 tmp = get_pv_mat()*glm::vec4(0, 0, dolly_, 0);
		m_dolly += glm::vec3(tmp.x, tmp.y, tmp.z);
	}

	void update_pedistal(float pedistal_) {
		glm::vec4 tmp = get_pv_mat()*glm::vec4(0, pedistal_, 0, 0);
		m_pedistal += glm::vec3(tmp.x, tmp.y, tmp.z);
	}

	void update_truck(float truck_) {
		glm::vec4 tmp = get_pv_mat()*glm::vec4(truck_, 0, 0, 0);
		m_truck += glm::vec3(tmp.x, tmp.y, tmp.z);
	}

	void set_pos(glm::vec3 pos_) {
		m_dolly = glm::vec3 (0,0,pos_.z);
		m_pedistal = glm::vec3(0, pos_.y, 0);
		m_truck = glm::vec3(pos_.x, 0, 0);
	}


	void update_pan(float angle_) {
		glm::quat rot_increase = glm::angleAxis(angle_, glm::vec3(0, 1, 0));
		m_rotation = rot_increase * m_rotation;
	}

	void upate_roll(float angle_) {
		glm::quat rot_increase = glm::angleAxis(angle_, glm::vec3(0, 0, 1));
		m_rotation = rot_increase * m_rotation;
	}

	void update_tilt(float angle_) {
		glm::quat rot_increase = glm::angleAxis(angle_, glm::vec3(1, 0, 0));
		m_rotation = rot_increase * m_rotation;
	}

	void update_rot(glm::quat rot_increase_) {
		m_rotation = rot_increase_ * m_rotation;
	}

	void set_rot(glm::quat rot) {
		m_rotation = rot;
	}

	void resize(int width_, int height_, float fov_ = 120.0, float z_near_ = 0.001, float z_far_ = 100) {
		m_width = width_;
		m_height = height_;
		set_projection_mat(fov_, (float)width_ / height_, z_near_, z_far_);
	}

	void update_mouse_pos(GLFWwindow* window, float x_, float y_) {
		if (first_mouse)
		{
			glfwSetCursorPos(window, m_width / 2, m_height / 2);
			m_x_pos = x_;
			m_y_pos = y_;
			m_last_dx = 0;
			m_last_dy = 0;
			first_mouse = false;
		}
		

		//std::cout << "dx : " << m_last_dx << " / dy : " << m_last_dy << " | " << " x : "<< m_x_pos << " y : " << m_y_pos << std::endl;
		
		float dx = m_last_dx = x_ - m_x_pos;
		float dy = m_last_dy = y_ - m_y_pos;
		dx *= m_mouse_sensitivity;
		dy *= m_mouse_sensitivity;

		yaw += dx;
		pitch += -dy;

		if (pitch > 89.0f)
			pitch = 89.0f;
		if (pitch < -89.0f)
			pitch = -89.0f;
		glm::vec3 front;
		front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		front.y = sin(glm::radians(pitch));
		front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		glm::vec3 movement = m_forward_direction - (front);
		m_forward_direction = glm::normalize(front);

		

		
		m_x_pos = x_;
		m_y_pos = y_;
	}

	glm::vec3 get_pos() {
		return m_cam_pos;
	}

	glm::vec3 get_direction() {
		return m_forward_direction;
	}

	float get_yaw() {
		return yaw;
	}

	float get_pitch() {
		return pitch;
	}

public:
	float m_mouse_sensitivity = 0.1f;
	float m_key_sensitivity = 0.3f;
	Camera_move_status m_status = Camera_move_status::STOP;

private:
	bool first_mouse = true;
	int m_width, m_height;
	float m_x_pos, m_y_pos;
	float m_last_dx, m_last_dy;
	float m_dx, m_dy;


	glm::vec3 m_cam_pos = glm::vec3(0.0f, 4.0f, 20.0f);
	glm::vec3 m_forward_direction = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 m_up_direction = glm::vec3(0.0f, 1.0f, 0.0f);
	float yaw = -90.0f;
	float pitch = 0.0f;

	glm::vec3 m_dolly;
	glm::vec3 m_truck;
	glm::vec3 m_pedistal;
	glm::quat m_rotation;

	glm::mat4 m_projection_mat;
};

GL2_Camera::GL2_Camera(int width_, int height_, float fov_ = 120.0, float z_near_ = 0.001, float z_far_ = 100)
{
	m_width = width_;
	m_height = height_;
	set_projection_mat(fov_, (float)width_ / height_, z_near_, z_far_);

}

GL2_Camera::~GL2_Camera()
{
}
