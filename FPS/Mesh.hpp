#pragma once
#define NUM_BONES_PER_VERTEX 3
#include <string>
#include <vector>

#include "glm/glm.hpp"
#include "GL/GLU.h"
#include "GL/glew.h"
#include "glm/gtx/quaternion.hpp"
#include "glm/gtc/quaternion.hpp"

#include "Shader.hpp"
#include "Config.hpp"

using namespace std;

class Bone_info {
public:
	aiMatrix4x4 offset;
	aiMatrix4x4 transform_final;
};

class Bone
{
public:
	uint32_t id[NUM_BONES_PER_VERTEX];
	float weight[NUM_BONES_PER_VERTEX];

	Bone() {
		for (uint32_t i = 0; i < NUM_BONES_PER_VERTEX; i++) {
			id[i] = 0;
			weight[i] = 0.0;
		}
	}

	void add_data(uint32_t id_, float weight_) {
		for (uint32_t i = 0; i < NUM_BONES_PER_VERTEX; i++) {
			if (weight[i] == 0.0) {
				id[i] = id_;
				weight[i] = weight_;
				return;
			}
		}
	}
};

class Vertex {
public:
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 tex_coord;
};

class Texture {
public:
	size_t id;
	string type;
	string path;
};

class Mesh {
public:
	vector<Vertex> m_vertices;
	vector<uint32_t> m_indices;
	vector<Texture> m_textures;
	vector<Bone> m_bones;
	glm::quat m_rot;
	btCollisionShape* m_collision_shape;
	btDefaultMotionState* m_motion_state;
	btRigidBody* m_rigid_body;

	Mesh(vector<Vertex> vertices_, vector<uint32_t> indices_, vector<Texture> textures_);
	void update_mesh();
	void draw(Shader shader_);
	void calc_min_max_pos();
	void translate(glm::vec3 move_, bool re_calc_center_ = true);
	void rotate(glm::mat4 rot_mat_);
	void scale(glm::mat4 scale_mat_);
	void rotate(glm::quat rot_);
	void init_rigid_body();
	void init_rigid_body(glm::vec3 start_pos, glm::vec3 direction_, float velocity_, float mass_);
	void update_animation(float time_);
	void update_physics();
	glm::mat4x4 get_bone_transform(float time_, vector<aiMatrix4x4>& Transforms);
	glm::vec3 m_max_pos, m_min_pos, m_center_pos;
	void setupMesh();
	
private:
	GLuint VAO, VBO, EBO;
	glm::vec3 m_velocity = glm::vec3(0, 0, 0);
};

void Mesh::init_rigid_body() {
	calc_min_max_pos();

	btConvexHullShape* convexHull = new btConvexHullShape();
	for (auto &itr : m_vertices) {
		convexHull->addPoint(btVector3(itr.position.x, itr.position.y, itr.position.z), false);
	}
	convexHull->recalcLocalAabb();
	convexHull->optimizeConvexHull();
	convexHull->setMargin(0.001);
	m_collision_shape = convexHull;
	//m_collision_shape = new btSphereShape(1);

	m_motion_state = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(m_center_pos.x, m_center_pos.y, m_center_pos.z)));

	btVector3 fallInertia(0, 0, 0);
	m_collision_shape->calculateLocalInertia(1, fallInertia);

	btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(
		10,                  // mass
		m_motion_state,        // initial position
		m_collision_shape,              // collision shape of body
		btVector3(0, 0, 0)    // local inertia
	);
	m_rigid_body = new btRigidBody(rigidBodyCI);
	m_rigid_body->setCollisionFlags(m_rigid_body->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);

	// example
	{
		/*	btCollisionShape* fallShape = new btSphereShape(1);
			btDefaultMotionState* fallMotionState =
				new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 50, 0)));
			btScalar mass = 1;
			btVector3 fallInertia(0, 0, 0);
			fallShape->calculateLocalInertia(mass, fallInertia);
			btRigidBody::btRigidBodyConstructionInfo fallRigidBodyCI(mass, fallMotionState, fallShape, fallInertia);
			fallRigidBody = new btRigidBody(fallRigidBodyCI);
			m_dynamicsWorld->addRigidBody(fallRigidBody);*/
	}
}

void Mesh::init_rigid_body(glm::vec3 start_pos_, glm::vec3 direction_, float velocity_, float mass_) {
	calc_min_max_pos();

	btConvexHullShape* convexHull = new btConvexHullShape();
	for (auto &itr : m_vertices) {
		convexHull->addPoint(btVector3(itr.position.x, itr.position.y, itr.position.z), false);
	}
	convexHull->recalcLocalAabb();
	convexHull->optimizeConvexHull();
	convexHull->setMargin(0.001);
	m_collision_shape = convexHull;
	//m_collision_shape = new btSphereShape(1);

	m_motion_state = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(start_pos_.x, start_pos_.y, start_pos_.z)));

	btVector3 fallInertia(0, 0, 0);
	m_collision_shape->calculateLocalInertia(1, fallInertia);

	btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(
		mass_,                  // mass
		m_motion_state,        // initial position
		m_collision_shape,              // collision shape of body
		btVector3(0, 0, 0)    // local inertia
	);
	m_rigid_body = new btRigidBody(rigidBodyCI);
	m_rigid_body->setLinearVelocity(btVector3(direction_.x*velocity_, direction_.y*velocity_, direction_.z*velocity_));
	m_rigid_body->setCollisionFlags(m_rigid_body->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
}

void Mesh::update_animation(float time_) {

}

void Mesh::update_physics() {
	btTransform trans;
	m_rigid_body->getMotionState()->getWorldTransform(trans);
	glm::vec3 after_pos(trans.getOrigin().getX(), trans.getOrigin().getY(), trans.getOrigin().getZ());
	/*cout << "center pos >> "; print_glm_vec3(m_center_pos);
	cout << "after pos >> "; print_glm_vec3(after_pos);
	cout << "movement >> "; print_glm_vec3(m_center_pos - after_pos);*/
	translate(after_pos - m_center_pos);
	m_center_pos = after_pos;

	update_mesh();
}

Mesh::Mesh(vector<Vertex> vertices_, vector<uint32_t> indices_, vector<Texture> textures_) {
	m_vertices = vertices_;
	m_indices = indices_;
	m_textures = textures_;

	setupMesh();
}

void Mesh::update_mesh() {
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, m_vertices.size() * sizeof(Vertex), &m_vertices[0]);      //replace data in VBO with new data
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Mesh::setupMesh() {
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(Vertex), &m_vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(uint32_t), &m_indices[0], GL_STATIC_DRAW);

	// vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
	// vertex normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
	// vertex texture coords
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tex_coord));

	glBindVertexArray(0);

	//// Bone id
	//glEnableVertexAttribArray(3);
	//glVertexAttribIPointer(3, NUM_BONES_PER_VERTEX, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, bone.id));

	//// Bone weight
	//glEnableVertexAttribArray(4);
	//glVertexAttribPointer(4, NUM_BONES_PER_VERTEX, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, bone.weight));
}

void Mesh::draw(Shader shader_) {
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

	GLuint diffuse_num = 3;
	GLuint specular_num = 1;
	for (uint32_t i = 0; i < m_textures.size(); i++) {
		glActiveTexture(GL_TEXTURE0 + i);
		string num;
		string name = m_textures[i].type;
		if (name == "texture_diffuse")
			num = to_string(diffuse_num++);
		else if (name == "texture_specular")
			num = to_string(specular_num++);
		shader_.set_uniform("material_" + name + num, (float)i);
		glBindTexture(GL_TEXTURE_2D, m_textures[i].id);
	}
	glActiveTexture(GL_TEXTURE0);

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void Mesh::calc_min_max_pos() {
	float min_x = INFINITY, min_y = INFINITY, min_z = INFINITY;
	float max_x = -INFINITY, max_y = -INFINITY, max_z = -INFINITY;

	for (auto &itr : m_vertices) {
		if (itr.position.x > max_x) max_x = itr.position.x;
		if (itr.position.y > max_y) max_y = itr.position.y;
		if (itr.position.z > max_z) max_z = itr.position.z;
		if (itr.position.x < min_x) min_x = itr.position.x;
		if (itr.position.y < min_y) min_y = itr.position.y;
		if (itr.position.z < min_z) min_z = itr.position.z;
	}

	m_max_pos = glm::vec3(max_x, max_y, max_z);
	m_min_pos = glm::vec3(min_x, min_y, min_z);
	m_center_pos = glm::vec3((max_x + min_x) / 2, (max_y + min_y) / 2, (max_z + max_z) / 2);
}

void Mesh::translate(glm::vec3 move_, bool re_calc_center_) {

	float min_x = INFINITY, min_y = INFINITY, min_z = INFINITY;
	float max_x = -INFINITY, max_y = -INFINITY, max_z = -INFINITY;

	for (auto &itr : m_vertices) {
		itr.position += move_;
		if (re_calc_center_) {
			if (itr.position.x > max_x) max_x = itr.position.x;
			if (itr.position.y > max_y) max_y = itr.position.y;
			if (itr.position.z > max_z) max_z = itr.position.z;
			if (itr.position.x < min_x) min_x = itr.position.x;
			if (itr.position.y < min_y) min_y = itr.position.y;
			if (itr.position.z < min_z) min_z = itr.position.z;
		}
	}

	if (re_calc_center_) {
		m_max_pos = glm::vec3(max_x, max_y, max_z);
		m_min_pos = glm::vec3(min_x, min_y, min_z);
		m_center_pos = glm::vec3((max_x + min_x) / 2, (max_y + min_y) / 2, (max_z + max_z) / 2);
	}
}

void Mesh::rotate(glm::mat4 rot_mat_) {
	for (auto &itr : m_vertices) {
		itr.position = rot_mat_*glm::vec4(itr.position, 1);
	}
}

void Mesh::scale(glm::mat4 scale_mat_) {
	for (auto &itr : m_vertices) {
		itr.position = scale_mat_ * glm::vec4(itr.position, 1);
	}
}