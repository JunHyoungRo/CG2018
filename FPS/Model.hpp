#pragma once
#include <string>
#include <vector>
#include <map>
#include <cmath>

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "btBulletDynamicsCommon.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Mesh.hpp"


using namespace std;

enum class Object_type {
	WORLD,
	TERRARIAN,
	MONSTER,
	PLAYER
};

enum class Collision_type {
	SPHERE,
	CUBE,
	CONVEX_HULL,
	ORIGIN_MASH
};

glm::mat4 ai_mat4_to_glm_mat4(aiMatrix4x4 matrix_) {
	return glm::mat4(
		matrix_.a1, matrix_.a2, matrix_.a3, matrix_.a4,
		matrix_.b1, matrix_.b2, matrix_.b3, matrix_.b4,
		matrix_.c1, matrix_.c2, matrix_.c3, matrix_.c4,
		matrix_.d1, matrix_.d2, matrix_.d3, matrix_.d4
	);
}

class Model {
public:
	vector<Mesh> m_meshes;
	uint32_t m_id;

	Model(string path_);
	void update_physics(float time_);
	void update_meshes();
	void setup_meshes();
	void draw(Shader shader_);
	void calc_min_max_pos();
	void update_animation(float time_);
	uint32_t texture_from_file(const string path_, const string directory_);
	void set_type(Object_type type_);
	void scale(float factor_);
	void rotate(float angle_, glm::vec3 axis_);
	void rotate(glm::mat4 rot_mat_);
	void translate(glm::vec3 move_);
	void init_physics(btDiscreteDynamicsWorld * m_dynamicsWorld);
	void init_physics(btDiscreteDynamicsWorld * m_dynamicsWorld, glm::vec3 start_pos_, glm::vec3 direction_, float velocity_, float mass_);
	glm::vec3 get_center();
	
private:
	glm::vec3 m_max_pos, m_min_pos, m_center_pos;
	Object_type m_type;
	float m_mass = 50.0f;
	glm::vec3 m_velocity;
	vector<Bone_info> m_bone_info;
	map<string, uint32_t> m_bone_map;
	uint32_t m_bone_num = 0;
	string m_dir;
	const aiScene* m_scene_ptr;

	// apply bullet physics
	btCollisionShape* m_collision_shape;
	btRigidBody* m_rigid_body;

	void load_model(string path_);
	void process_node(aiNode* node_, const aiScene* scene_);
	Mesh process_mesh(aiMesh* mesh_, const aiScene* scene_);
	vector<Texture> load_material_textures(aiMaterial* mat, aiTextureType type, string type_name);
	void read_node_heirarchy(float animation_time_, const aiNode* node_ptr_, const glm::mat4 parent_transform_mat_);
	const aiNodeAnim* find_node_anim(const aiAnimation* anim_ptr_, const string node_name_);
};

void Model::scale(float factor_) {
	glm::mat4 scale_mat(factor_);
	for (auto &itr : m_meshes) {
		itr.scale(scale_mat);
	}
}

void Model::rotate(float angle_, glm::vec3 axis_) {
	glm::mat4 rot_mat = glm::rotate(angle_, axis_);

	for (auto &itr : m_meshes) {
		itr.rotate(rot_mat);
	}
}

void Model::rotate(glm::mat4 rot_mat_) {
	for (auto &itr : m_meshes) {
		itr.rotate(rot_mat_);
	}
}

void Model::translate(glm::vec3 move_) {
	for (auto &itr : m_meshes) {
		itr.translate(move_);
	}
}

// For first program run
void Model::init_physics(btDiscreteDynamicsWorld* m_dynamicsWorld) {
	for (auto &itr : m_meshes) {
		itr.init_rigid_body();
		m_dynamicsWorld->addRigidBody(itr.m_rigid_body);
	}
}

// For something generated..
void Model::init_physics(btDiscreteDynamicsWorld* m_dynamicsWorld, glm::vec3 start_pos_, glm::vec3 direction_, float velocity_, float mass_) {
	for (auto &itr : m_meshes) {
		itr.init_rigid_body(start_pos_, direction_, velocity_, mass_);
		m_dynamicsWorld->addRigidBody(itr.m_rigid_body);
	}
}

glm::vec3 Model::get_center() {
	glm::vec3 pos(0.0f, 0, 0);
	for (auto &itr : m_meshes) {
		pos += itr.m_center_pos;
	}
	pos = pos / (float)m_meshes.size();
	return pos;
}

const aiNodeAnim* Model::find_node_anim(const aiAnimation* anim_ptr_, const string node_name_) {
	for (uint32_t i = 0; i < anim_ptr_->mNumChannels; i++) {
		const aiNodeAnim* pNodeAnim = anim_ptr_->mChannels[i];

		if (string(pNodeAnim->mNodeName.data) == node_name_) {
			return pNodeAnim;
		}
	}
	return NULL;
}

void Model::read_node_heirarchy(float animation_time_, const aiNode* node_ptr_, const glm::mat4 parent_transform_mat_) {
	string node_name(node_ptr_->mName.data);
	const aiAnimation* animation_ptr = m_scene_ptr->mAnimations[0];
	glm::mat4 node_transform(ai_mat4_to_glm_mat4(node_ptr_->mTransformation));
	const aiNodeAnim* node_anim_ptr = find_node_anim(animation_ptr, node_name);

	if (node_anim_ptr != NULL) {

	}
}

void Model::update_animation(float time_) {
	float ticks_per_sec = m_scene_ptr->mAnimations[0]->mTicksPerSecond != 0 ?
		m_scene_ptr->mAnimations[0]->mTicksPerSecond : 25.0f;
	float time_in_ticks = time_ * ticks_per_sec;
	float animation_time = fmod(time_in_ticks, m_scene_ptr->mAnimations[0]->mDuration);

	//	read_node_heirarchy(animation_time, m_scene_ptr->mRootNode, );
	for (uint32_t i = 0; i < m_bone_num; i++) {
		m_bone_info[i].transform_final;
	}

	for (uint32_t i = 0; i < m_meshes.size(); i++) {
		m_meshes[i].update_animation(time_);
	}
}

Model::Model(string path_) {
	load_model(path_);
	m_id = rand();
}

void Model::update_physics(float time_) {
	for (auto &itr : m_meshes) {
		itr.update_physics();
	}
}

void Model::update_meshes() {
	for (auto &itr : m_meshes) {
		itr.update_mesh();
		//itr.update_mesh();
	}
}

void Model::setup_meshes() {
	for (auto &itr : m_meshes) {
		itr.setupMesh();
		//itr.update_mesh();
	}
}

void Model::draw(Shader shader_) {
	for (uint32_t i = 0; i < m_meshes.size(); i++) {
		m_meshes[i].draw(shader_);
	}
}

void Model::calc_min_max_pos() {
	float min_x = INFINITY, min_y = INFINITY, min_z = INFINITY;
	float max_x = -INFINITY, max_y = -INFINITY, max_z = -INFINITY;

	for (auto &itr : m_meshes) {
		itr.calc_min_max_pos();
		if (itr.m_max_pos.x > max_x) max_x = itr.m_max_pos.x;
		if (itr.m_max_pos.y > max_y) max_y = itr.m_max_pos.y;
		if (itr.m_max_pos.z > max_z) max_z = itr.m_max_pos.z;
		if (itr.m_min_pos.x < min_x) min_x = itr.m_min_pos.x;
		if (itr.m_min_pos.y < min_y) min_y = itr.m_min_pos.y;
		if (itr.m_min_pos.z < min_z) min_z = itr.m_min_pos.z;
	}
	m_max_pos = glm::vec3(max_x, max_y, max_z);
	m_min_pos = glm::vec3(min_x, min_y, min_z);
	m_center_pos = glm::vec3((max_x + min_x) / 2, (max_y + min_y) / 2, (max_z + max_z) / 2);
}

void Model::load_model(string path_) {
	Assimp::Importer import;
	const aiScene *m_scene_p = import.ReadFile(path_, aiProcess_Triangulate | aiProcess_FlipUVs);
	if (!m_scene_p || m_scene_p->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !m_scene_p->mRootNode) {
		cout << "ERROR::ASSIMP::" << import.GetErrorString() << endl;
		return;
	}
	m_dir = path_.substr(0, path_.find_last_of('/'));

	process_node(m_scene_p->mRootNode, m_scene_p);
}

void Model::process_node(aiNode* node_, const aiScene* scene_) {
	for (uint32_t i = 0; i < node_->mNumMeshes; i++) {
		aiMesh* mesh = scene_->mMeshes[node_->mMeshes[i]];
		m_meshes.push_back(process_mesh(mesh, scene_));
	}

	for (uint32_t i = 0; i < node_->mNumChildren; i++) {
		process_node(node_->mChildren[i], scene_);
	}
}

Mesh Model::process_mesh(aiMesh* mesh_, const aiScene* scene_) {
	vector<Vertex> vertices;
	vector<uint32_t> indices;
	vector<Texture> textures;
	vector<Bone> bones;

	// Process vertex position, normal, texture coord
	for (uint32_t i = 0; i < mesh_->mNumVertices; i++) {
		Vertex vtx;

		// set position
		glm::vec3 vec;
		vec.x = mesh_->mVertices[i].x;
		vec.y = mesh_->mVertices[i].y;
		vec.z = mesh_->mVertices[i].z;
		vtx.position = vec;

		// set normal
		vec.x = mesh_->mNormals[i].x;
		vec.y = mesh_->mNormals[i].y;
		vec.z = mesh_->mNormals[i].z;
		vtx.normal = vec;

		// set texture coordinates
		if (mesh_->mTextureCoords[0]) {
			glm::vec2 vec;
			vec.x = mesh_->mTextureCoords[0][i].x;
			vec.y = mesh_->mTextureCoords[0][i].y;
			vtx.tex_coord = vec;
		}
		else {
			vtx.tex_coord = glm::vec2(0, 0);
		}

		//// set tangent (?)
		//vec.x = mesh_->mTangents[i].x;
		//vec.y = mesh_->mTangents[i].y;
		//vec.z = mesh_->mTangents[i].z;
		//vtx.tangent = vec;

		//// set bitangent (?)
		//vec.x = mesh_->mBitangents[i].x;
		//vec.y = mesh_->mBitangents[i].y;
		//vec.z = mesh_->mBitangents[i].z;
		//vtx.bitangent = vec;

		vertices.push_back(vtx);
	}

	for (uint32_t i = 0; i < mesh_->mNumFaces; i++) {
		aiFace face = mesh_->mFaces[i];

		for (uint32_t j = 0; j < face.mNumIndices; j++) {
			indices.push_back(face.mIndices[j]);
		}
	}

	if (mesh_->mMaterialIndex >= 0) {
		aiMaterial* material = scene_->mMaterials[mesh_->mMaterialIndex];
		vector<Texture> diffuse_maps = load_material_textures(material, aiTextureType_DIFFUSE, "texture_diffuse");
		textures.insert(textures.end(), diffuse_maps.begin(), diffuse_maps.end());

		vector<Texture> specular_maps = load_material_textures(material, aiTextureType_SPECULAR, "texture_specular");
		textures.insert(textures.end(), specular_maps.begin(), specular_maps.end());

		// more than normal mpas, height maps...

	}

	// if mesh has bones....
	if (mesh_->mNumBones > 0) {
		bones.resize(vertices.size());
		for (uint32_t i = 0; i < mesh_->mNumBones; i++) {
			uint32_t bone_index = 0;
			string bone_name(mesh_->mBones[i]->mName.data);

			// construct bone_map
			if (m_bone_map.find(bone_name) == m_bone_map.end()) {
				bone_index = m_bone_num;
				m_bone_num++;
				m_bone_info.push_back(Bone_info());
			}
			else {
				bone_index = m_bone_map[bone_name];
			}

			m_bone_map[bone_name] = bone_index;
			m_bone_info[bone_index].offset = mesh_->mBones[i]->mOffsetMatrix;

			// insert bone data for each vertex
			for (uint32_t j = 0; j < mesh_->mBones[i]->mNumWeights; j++) {
				uint32_t vertex_id = mesh_->mBones[i]->mWeights[j].mVertexId;
				float weight = mesh_->mBones[i]->mWeights->mWeight;
				bones[i].add_data(bone_index, weight);
			}
		}
	}
	else {
		bones.push_back(Bone());
	}


	return Mesh(vertices, indices, textures);
}

vector<Texture> Model::load_material_textures(aiMaterial* mat_, aiTextureType type_, string type_name_) {
	vector<Texture> textures;
	for (uint32_t i = 0; i < mat_->GetTextureCount(type_); i++) {
		aiString str;
		mat_->GetTexture(type_, i, &str);
		Texture texture;
		texture.id = texture_from_file(str.C_Str(), m_dir);
		texture.type = type_name_;
		textures.push_back(texture);
	}

	return textures;
}

uint32_t Model::texture_from_file(const string path_, const string directory_) {
	string filename = directory_ + '/' + path_;
	GLuint texture_id;
	glGenTextures(1, &texture_id);

	int width, height, nr_components;
	uint8_t* data = stbi_load(filename.c_str(), &width, &height, &nr_components, 0);
	if (data) {
		GLenum format;
		if (nr_components == 1)
			format = GL_RED;
		else if (nr_components == 3)
			format = GL_RGB;
		else if (nr_components == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, texture_id);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else {
		cout << "Texture failed to load at path : " << path_ << endl;
		stbi_image_free(data);
	}

	return texture_id;
}

void Model::set_type(Object_type type_) {
	m_type = type_;
}
