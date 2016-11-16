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
		for (int i = 0; i < this->balls->size();++i)
		{
			for (int j = i + 1; j < this->balls->size();++j)
			{
				if (isColliding((*balls)[i], (*balls)[j]))
				{
					myVector ballOneVel = (*balls)[i]->getVelocity();
					myVector ballTwoVel = (*balls)[j]->getVelocity();

					myVector ballOnePos = (*balls)[i]->getPosition(); 
					myVector ballTwoPos = (*balls)[j]->getPosition();

					float ballOneMass = (*balls)[i]->getMass();
					float ballTwoMass = (*balls)[j]->getMass();

					float newOneX = (ballOneVel.x * (ballOneMass - ballTwoMass) + (2 * ballTwoMass * ballTwoVel.x)) / (ballOneMass + ballTwoMass);
					float newOneY = (ballOneVel.y * (ballOneMass - ballTwoMass) + (2 * ballTwoMass * ballTwoVel.y)) / (ballOneMass + ballTwoMass);

					float newTwoX = (ballTwoVel.x * (ballTwoMass - ballOneMass) + (2 * ballOneMass * ballOneVel.x)) / (ballOneMass + ballTwoMass);
					float newTwoY = (ballTwoVel.y * (ballTwoMass - ballOneMass) + (2 * ballOneMass * ballOneVel.y)) / (ballOneMass + ballTwoMass);

					(*balls)[i]->setVelocity(myVector(newOneX, newOneY, ballOneVel.z));
					(*balls)[j]->setVelocity(myVector(newTwoX, newTwoY, ballTwoVel.z));

					(*balls)[i]->update(deltaTime);
					(*balls)[j]->update(deltaTime);
				}
			}
		}
	}

	bool isColliding(Ball* ballOne, Ball* ballTwo)
	{
		myVector dif = ballOne->getPosition() - ballTwo->getPosition();
		float magSquared = (dif.x * dif.x) + (dif.y * dif.y) + (dif.z * dif.z);

		float distanceSquared = (ballOne->getRadius() + ballTwo->getRadius()) * (ballOne->getRadius() + ballTwo->getRadius());

		if (magSquared <= distanceSquared)
		{
			return true;
		}
		return false;
	}

	std::vector<GameEntity*> getBallGameEntities() {
		std::vector<GameEntity*> entities;

		for (auto i = balls->begin(); i != balls->end(); i++) {
			Ball* b = *i;
			entities.push_back(b->getMesh());
		}

		return entities;
	}
};