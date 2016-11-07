#pragma once

#include <DirectXMath.h>
#include "GameEntity.h"

struct myVector
{
	float x;
	float y;
	float z;

	myVector() : x(0.f), y(0.f), z(0.f) {}

	myVector(float x, float y, float z) : x(x), y(y), z(z) {}

	myVector(const myVector& other) : x(other.x), y(other.y), z(other.z) {}
#pragma region Operators
	friend myVector operator+(const myVector &v1, const myVector &v2)
	{
		return myVector(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
	}

	friend myVector operator-(const myVector &v1, const myVector &v2)
	{
		return myVector(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
	}

	friend myVector operator*(const myVector &v, const float &scalar)
	{
		return myVector(v.x * scalar, v.y * scalar, v.z * scalar);
	}

	friend myVector operator*(const float &scalar, const myVector &v)
	{
		return myVector(v.x * scalar, v.y * scalar, v.z * scalar);
	}

	friend myVector operator/(const myVector &v, const float &scalar)
	{
		return myVector(v.x / scalar, v.y / scalar, v.z / scalar);
	}

	friend myVector operator/(const float &scalar, const myVector &v)
	{
		return myVector(v.x / scalar, v.y / scalar, v.z / scalar);
	}

	myVector& myVector::operator+=(const myVector& rhs)
	{
		this->x += rhs.x;
		this->y += rhs.y;
		this->z += rhs.z;
		return *this;
	}

	myVector& myVector::operator-=(const myVector& rhs)
	{
		this->x -= rhs.x;
		this->y -= rhs.y;
		this->z -= rhs.z;
		return *this;
	}

	void operator = (const myVector &V)
	{
		this->x = V.x;
		this->y = V.y;
		this->z = V.z;
	}
#pragma endregion
};


class Ball
{
	myVector position;
	myVector velocity;
	myVector acceleration;

	float mass;
	float radius;

	GameEntity* mesh;

public:
	Ball(GameEntity* mesh, myVector position, myVector velocity, float mass, float radius)
	{
		this->mesh = new GameEntity(mesh);
		this->position = position;
		this->velocity = velocity;
		this->acceleration = myVector(0.f, 0.f, 0.f);
		this->mass = mass;
		this->radius = radius;

		this->mesh->SetScale(radius, radius, radius);
	}

	~Ball()
	{
		delete mesh;
	}

	void applyForce(myVector force)
	{ 
		this->acceleration += force;
	}

	void update(float deltaTime)
	{
		this->velocity += this->acceleration * deltaTime;
		this->position += this->velocity * deltaTime;
		this->acceleration = { 0,0,0 };
		this->mesh->SetTranslation(this->position.x, this->position.y, this->position.z);

		// Check if walls are hit - bounce back
		if (position.x > 2.3 || position.x < -2.3) {
			velocity.x *= -1;
		}
	}

	myVector getPosition()
	{
		return this->position;
	}

	myVector getVelocity()
	{
		return this->velocity;
	}

	void setVelocity(myVector newVelocity)
	{
		this->velocity = newVelocity;
	}

	float getRadius()
	{
		return this->radius;
	}

	float getMass()
	{
		return this->mass;
	}
	
	GameEntity* getMesh()
	{
		return this->mesh;
	}
};