#pragma once
#include "Model.hpp"

class Monster : public Model
{
public:
	Monster(string path_);
	~Monster();

private:

};

Monster::Monster(string path_): Model(path_)
{
}

Monster::~Monster()
{
}