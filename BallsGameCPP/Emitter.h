#pragma once

#include "Ball.h"
#include <vector>
#include "GameEntity.h"

struct particle
{
	myVector position;
	myVector direction;
	float lifetime;
	GameEntity* quad;

	particle(myVector position, myVector direction, float lifetime, GameEntity* quad) : position(position), direction(direction), lifetime(lifetime), quad(quad) {}
};


class Emitter
{
	std::vector<particle>* particles;
	float maxLifetime;
	float period;
	int numLiveParticles;
	myVector origin;
	int index;
	float timer;
	float emitterTimer;


public:
	Emitter(float maxLifetime, std::vector<GameEntity*>* quads, myVector origin, float particleVelocity)
	{
		this->maxLifetime = maxLifetime;
		this->numLiveParticles = quads->size() / 2; 
		this->period = this->maxLifetime / this->numLiveParticles;
		this->origin = origin;
		this->particles = new std::vector<particle>();
		this->index = 0;
		this->timer = 0;
		for (int i = 0; i < quads->size(); ++i)
		{
			this->particles->push_back(particle(myVector(0,0,0.65), myVector::randVector() * particleVelocity, 0, (*quads)[i]));
		}
		(*(this->particles))[0].lifetime = this->maxLifetime;
		(*(this->particles))[0].position = origin;
		this->emitterTimer = 0;
	}

	Emitter(float maxLifetime, std::vector<GameEntity*>* quads, myVector origin, float particleVelocity, float emitterTimer)
	{
		this->maxLifetime = maxLifetime;
		this->numLiveParticles = quads->size() / 2;
		this->period = this->maxLifetime / this->numLiveParticles;
		this->origin = origin;
		this->particles = new std::vector<particle>();
		this->index = 0;
		this->timer = 0;
		for (int i = 0; i < quads->size(); ++i)
		{
			this->particles->push_back(particle(myVector(0, 0, 0.65), myVector::randVector() * particleVelocity, 0, (*quads)[i]));
		}
		(*(this->particles))[0].lifetime = this->maxLifetime;
		(*(this->particles))[0].position = origin;
		this->emitterTimer = emitterTimer;
	}

	std::vector<GameEntity*> getActiveParticles()
	{
		std::vector<GameEntity*> toReturn = std::vector<GameEntity*>();
		for (auto& particle : *(this->particles))
		{
			if (particle.lifetime > 0)
				toReturn.push_back(particle.quad);
			
		}
		return toReturn;
	}

	void update(float deltaTime)
	{
		this->timer += deltaTime;
		if (this->timer > this->period)
		{
			this->timer = 0;
			this->index++;
			if (this->index >= this->particles->size())
				this->index = 0;
			(*(this->particles))[index].lifetime = this->maxLifetime;
			(*(this->particles))[index].position = origin;
		}

		for (auto& particle : *(this->particles))
		{
			if (particle.lifetime > 0)
			{
				particle.position += particle.direction;
				particle.lifetime -= deltaTime;
				particle.quad->SetTranslation(particle.position.x, particle.position.y, particle.position.z);
			}
		}
		if (this->emitterTimer > 0)
		{
			this->emitterTimer -= deltaTime;
			if (this->emitterTimer < 0)
				this->emitterTimer = 0;
		}
	}

	bool isAlive()
	{
		return this->emitterTimer;
	}

	~Emitter()
	{
		for (int i = 0; i < this->particles->size(); ++i)
		{
			delete (*(this->particles))[i].quad;
		}
		delete this->particles;
	}
};

