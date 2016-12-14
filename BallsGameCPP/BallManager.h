#pragma once

#include <vector>
#include <cmath>
#include "Ball.h"
#include "Emitter.h"
#include "Mesh.h"
#include "Material.h"


class BallManager
{
	std::vector<Ball*>* balls;
	std::vector<Emitter*> explosions;
	Mesh* explosionMesh;
	Material* explosionMaterial;
	float maxSpeed;

public:
	BallManager()
	{
		balls = new std::vector<Ball*>();
		maxSpeed = 2;
	}

	~BallManager()
	{
		for (int i = 0; i < this->balls->size(); ++i)
		{
			delete (*balls)[i];
		}
		delete balls;
		for (int i = 0; i < this->explosions.size(); ++i)
		{
			delete this->explosions[i];
		}
	}

	void setExpMesh(Mesh* explosionMesh)
	{
		this->explosionMesh = explosionMesh;
	}

	void setExpMat(Material* explosionMaterial)
	{
		this->explosionMaterial = explosionMaterial;
	}

	void addBall(GameEntity* ballMesh, myVector position, myVector velocity, float mass, float radius, bool isMain)
	{
		this->balls->push_back(new Ball(ballMesh, position, velocity, mass, radius, isMain));
	}

	std::vector<GameEntity*> getActiveParticles()
	{
		std::vector<GameEntity*> toReturn = std::vector<GameEntity*>();
		for (auto& emitter : this->explosions)
		{
			std::vector<GameEntity*> temp = emitter->getActiveParticles();
			toReturn.insert(toReturn.end(), temp.begin(), temp.end());
		}
		return toReturn;
	}

	void Update(float deltaTime)
	{
		for (int i = 0; i < this->explosions.size(); ++i)
		{
			if (!(this->explosions[i]->isAlive()))
			{
				delete this->explosions[i];
				this->explosions.erase(this->explosions.begin() + i);
				i--;
			}
		}
		for (auto& emitter : explosions)
		{
			emitter->update(deltaTime);
		}

		for (int i = 0; i < this->balls->size(); ++i)
		{
			Ball* ball = (*(this->balls))[i];
			ball->update(deltaTime);
			if (ball->getDespawn())
			{
					//Despawn the ball here
					delete ball;
					this->balls->erase(this->balls->begin() + i);
			}
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

					if (ballOneVel.magSquared() == 0)
						ballOneVel.x = 0.000000000001;
					if (ballTwoVel.magSquared() == 0)
						ballTwoVel.x = 0.000000000001;

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


					if (newVelOne.magnitude() > this->maxSpeed)
						newVelOne /= newVelOne.magnitude() / this->maxSpeed;

					if (newVelTwo.magnitude() > this->maxSpeed)
						newVelTwo /= newVelTwo.magnitude() / this->maxSpeed;

					(*balls)[i]->setVelocity(myVector(newVelOne.x, newVelOne.y, 0.f));
					(*balls)[j]->setVelocity(myVector(newVelTwo.x, newVelTwo.y, 0.f));


					(*balls)[i]->update(deltaTime);
					(*balls)[j]->update(deltaTime);

					myVector collisionPoint = (ballOnePos + ballTwoPos) / 2;

					std::vector<GameEntity*> temp = std::vector<GameEntity*>();
					for (int i = 0; i < 100; ++i)
					{
						temp.push_back(new GameEntity(explosionMesh, explosionMaterial));
						temp[i]->SetScale(0.1, 0.1, 0.1);
					}

					explosions.push_back(new Emitter(0.5, &temp, collisionPoint, 0.01, 1));
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