#pragma once
#include <vector>

#include "btBulletDynamicsCommon.h"

#include "Config.hpp"
#include "Model.hpp"
#include "Shader.hpp"
#include "GL2_Camera.hpp"

using namespace std;
class World
{
public:
	vector<Model> m_fixed;
	vector<Model> m_monsters;
	vector<Model> m_bullets;
	vector<float> m_bullets_life;
	vector<Model> m_objects;
	vector<Model> m_terrarians;
	vector<Model> m_particles;
	vector<float> m_particles_life;
	GL2_Camera m_camera;
	Shader m_shader;
	GLuint m_shader_id;	
	

	World();
	~World();
	void init_physics();
	void update_physics(float time_);
	void event_collision(int monster_id, int bullet_id);
	void use_shader(string vertex_path_, string fragment_path_);
	void draw();
	btDiscreteDynamicsWorld* m_dynamicsWorld;
	void drawAxes();

private:
	btBroadphaseInterface * m_broadphase;
	btDefaultCollisionConfiguration* m_collisionConfiguration;
	btCollisionDispatcher* m_dispatcher;
	btSequentialImpulseConstraintSolver* m_solver;
	float m_last_time;
	float m_delta_time;
	float m_current_time;

	// bullet test
	btRigidBody* fallRigidBody;
	btCollisionShape* groundShape;
};

void World::init_physics() {
	// set ground
	groundShape = new btStaticPlaneShape(btVector3(0, 1, 0), 1); // collision shapce 설정
	btDefaultMotionState* groundMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 0, 0))); // motition state 지정
	btRigidBody::btRigidBodyConstructionInfo // 강체정보생성 collision shape 와 motion state를 엮어줌
		groundRigidBodyCI(0, groundMotionState, groundShape, btVector3(0, 0, 0));
	btRigidBody* groundRigidBody = new btRigidBody(groundRigidBodyCI);
	m_dynamicsWorld->addRigidBody(groundRigidBody); //월드에 강체정보 등록

	// ground tests
	/*btCollisionShape* fallShape = new btSphereShape(5);
	btDefaultMotionState* fallMotionState =
		new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 50, 0)));
	btScalar mass = 1;
	btVector3 fallInertia(0, 0, 0);
	fallShape->calculateLocalInertia(mass, fallInertia);
	btRigidBody::btRigidBodyConstructionInfo fallRigidBodyCI(mass, fallMotionState, fallShape, fallInertia);
	fallRigidBody = new btRigidBody(fallRigidBodyCI);
	m_dynamicsWorld->addRigidBody(fallRigidBody);*/
}

void World::update_physics(float time_) {
	/*
	vector<Model> m_fixed; // no
	vector<Model> m_monsters;
	vector<Model> m_bullets;
	vector<Model> m_objects; // no
	vector<Model> m_terrarians; //no
	*/

	m_dynamicsWorld->stepSimulation(time_);

	// 몬스터 물리정보 업데이트
	for (auto &itr : m_monsters) {
		itr.update_physics(time_);
	}	

	// 총알 물리정보 업데이트
	for (auto &itr : m_bullets) {
		itr.update_physics(time_);
	}
	
	// 파티클 업데이트
	for (auto &itr : m_particles) {
		itr.update_physics(time_);
	}
	
	// bullet life cycle 
	for (int i = 0; i < m_bullets.size(); i++) {
		if ((g_current_time - m_bullets_life[i]) > 2) {
			m_bullets.erase(m_bullets.begin());
			m_bullets_life.erase(m_bullets_life.begin());
		}
	}

	// particle life cycle
	for (int i = 0; i < m_particles.size(); i++) {
		if ((g_current_time - m_particles_life[i]) > 2) {
			m_particles.erase(m_particles.begin());
			m_particles_life.erase(m_particles_life.begin());
		}
	}

	if (CONFIG_DEBUG) {
		// update test
		/*btTransform trans;
		fallRigidBody->getMotionState()->getWorldTransform(trans);
		std::cout << "pyhsics testing... sphere height: " << trans.getOrigin().getY() << std::endl;*/
	}
}


World::World()
{
	m_camera = GL2_Camera(CONFIG_WINDOW_WIDTH, CONFIG_WINDOW_HEIGHT, 120.0f, 0.001f, 1000000000.0f);
	m_broadphase = new btDbvtBroadphase();
	m_collisionConfiguration = new btDefaultCollisionConfiguration();
	m_dispatcher = new btCollisionDispatcher(m_collisionConfiguration);
	m_solver = new btSequentialImpulseConstraintSolver;
	m_dynamicsWorld = new btDiscreteDynamicsWorld(m_dispatcher, m_broadphase, m_solver, m_collisionConfiguration);
	m_dynamicsWorld->setGravity(btVector3(0, CONFIG_GRAVITY.y, 0));
}

World::~World()
{
}
void World::event_collision(int monster_id, int bullet_id) {

	[&]()->void {
		if (CONFIG_DEBUG) { cout << "BOOM!!" << endl; }
		// Manual physics setting

		for (int i = 0; i<m_monsters.size(); i++) {
			if (m_monsters[i].m_id == monster_id) {
				for (int j = 0; j < 10; j++) {
					Model ball(m_objects[2]);
					ball.m_id = rand();
					ball.setup_meshes();
					ball.update_meshes();
					ball.init_physics(
						m_dynamicsWorld,
						m_monsters[i].get_center(),
						glm::vec3(rand_float(0,1), rand_float(0,1), rand_float(0,1)),
						rand_float(0,3),
						5
					);

					m_particles.push_back(ball);
					m_particles_life.push_back(g_current_time);
				}
			}
		}
	}();

	for (int i = 0; i<m_monsters.size() ; i++) {
		if (m_monsters[i].m_id == monster_id) {
			m_monsters.erase(m_monsters.begin() + i);
			break;
		}
	}

	for (int i = 0; i < m_bullets.size(); i++) {
		if (m_bullets[i].m_id == bullet_id) {
			m_bullets.erase(m_bullets.begin() + i);
			m_bullets_life.erase(m_bullets_life.begin() + i);
			break;
		}
	}

	
}
void World::use_shader(string vertex_path_, string fragment_path_) {
	m_shader_id = m_shader.load(vertex_path_.c_str(), fragment_path_.c_str());
}

void World::draw() {

	/*
	vector<Model> m_fixed;
	vector<Model> m_monsters;
	vector<Model> m_bullets;
	vector<Model> m_objects;
	vector<Model> m_terrarians;
	*/
	
	glm::mat4 v = m_camera.get_view_mat();
	glm::mat4 p = m_camera.get_projection_mat();
	m_shader.set_uniform("mvp", p*v);
	for (auto &itr : m_monsters) {
		itr.draw(m_shader);
	}

	for (auto &itr : m_bullets) {
		itr.draw(m_shader);
	}

	for (auto &itr : m_particles) {
		itr.draw(m_shader);
	}

	/*for (auto &itr : m_objects) {
		itr.draw(m_shader);
	}*/

	for (auto &itr : m_terrarians) {
		itr.draw(m_shader);
	}

	
	m_shader.set_uniform("mvp",p);
	for (auto &itr : m_fixed) {
		itr.draw(m_shader);
	}
}

void World::drawAxes()
{
	glDisable(GL_LIGHTING);

	// xyz axes
	glBegin(GL_LINES);
	glColor3f(1.0, 0.0, 0.0);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(1.0, 0.0, 0.0);

	glColor3f(0.0, 1.0, 0.0);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(0.0, 1.0, 0.0);

	glColor3f(0.0, 0.0, 1.0);
	glVertex3f(0.0, 0.0, 0.0);
	glVertex3f(0.0, 0.0, 1.0);
	glEnd();
}
