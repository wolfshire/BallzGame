#pragma once

#include <vector>
#include "Ball.h"

class BallManager
{
	std::vector<Ball*>* balls;
public:
	BallManager()
	{
		balls = new std::vector<Ball*>();
	}

	~BallManager()
	{
		for (int i = 0; i < this->balls->size(); ++i)
		{
			delete (*balls)[i];
		}
		delete balls;
	}

	void addBall(GameEntity* ballMesh, myVector position, myVector velocity, float mass, float radius)
	{
		this->balls->push_back(new Ball(ballMesh, position, velocity, mass, radius));
	}

	void Update(float deltaTime)
	{
		for (auto ball : *(this->balls))
		{
			ball->update(deltaTime);
		}
	}
};