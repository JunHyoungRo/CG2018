#pragma once
#include "glm/glm.hpp"
#include "btBulletDynamicsCommon.h"

class Particle
{
public:
	glm::vec3 m_pos;
	Particle(glm::vec3 pos_);
	~Particle();

	btCollisionShape* m_collision_shape;
	btDefaultMotionState* m_motion_state;
	btRigidBody* m_rigid_body;
private:

};

Particle::Particle(glm::vec3 pos_)
{
	m_pos = pos_;
	m_collision_shape = new btSphereShape(5);
	m_motion_state = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(pos_.x, pos_.y, pos_.z)));
	btScalar mass = 50;
	btVector3 fallInertia(0, 0, 0);
	m_collision_shape->calculateLocalInertia(mass, fallInertia);
	btRigidBody::btRigidBodyConstructionInfo fallRigidBodyCI(mass, m_motion_state, m_collision_shape, fallInertia);
	m_rigid_body = new btRigidBody(fallRigidBodyCI);
}

Particle::~Particle()
{
}