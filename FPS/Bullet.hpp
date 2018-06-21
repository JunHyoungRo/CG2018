#pragma once
#include "Model.hpp"

class Bullet : public Model
{
public:
	Bullet(string path_);
	Bullet(Model* model);
	~Bullet();

private:

};

Bullet::Bullet(string path_) : Model(path_)
{
}
Bullet::~Bullet()
{
}