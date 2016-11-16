#pragma once

#include <vector>
#include <cmath>
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

					(*balls)[i]->unUpdate(deltaTime);
					(*balls)[j]->unUpdate(deltaTime);


					myVector ballOneVel = (*balls)[i]->getVelocity();
					myVector ballTwoVel = (*balls)[j]->getVelocity();

					myVector ballOnePos = (*balls)[i]->getPosition(); 
					myVector ballTwoPos = (*balls)[j]->getPosition();

					float ballOneMass = (*balls)[i]->getMass();
					float ballTwoMass = (*balls)[j]->getMass();

					myVector normal = ballOnePos - ballTwoPos;
					normal /= normal.magnitude();

					myVector ballOneCollisionComp = normal * normal.dot(ballOneVel);
					myVector ballOneOrthoComp = ballOneVel - ballOneCollisionComp;

					myVector ballTwoCollisionComp = normal * normal.dot(ballTwoVel);
					myVector ballTwoOrthoComp = ballTwoVel - ballTwoCollisionComp;

					float mag1 = ballOneCollisionComp.magnitude() * std::sin(normal.dot(ballOneVel));
					float mag2 = ballTwoCollisionComp.magnitude() * std::sin(normal.dot(ballTwoVel));

					float commonVel = 2 * (ballOneMass * mag1 + ballTwoMass * mag2) / (ballOneMass + ballTwoMass);

					float mag1post = commonVel - mag1;
					float mag2post = commonVel - mag2;

					ballOneCollisionComp *= (mag1post / mag1);
					ballTwoCollisionComp *= (mag2post / mag2);

					myVector newVelOne = ballOneCollisionComp + ballOneOrthoComp;
					myVector newVelTwo = ballTwoCollisionComp + ballTwoOrthoComp;


					if (newVelOne.magnitude() > 2)
					{
						newVelOne /= newVelOne.magnitude() / 2;
					}

					if (newVelTwo.magnitude() > 2)
					{
						newVelTwo /= newVelTwo.magnitude() / 2;
					}

					(*balls)[i]->setVelocity(myVector(newVelOne.x, newVelOne.y, 0.f));
					(*balls)[j]->setVelocity(myVector(newVelTwo.x, newVelTwo.y, 0.f));


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