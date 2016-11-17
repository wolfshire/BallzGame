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

	float magnitude() { return std::sqrt(this->magSquared()); }

	float magSquared() { return (this->x * this->x) + (this->y * this->y) + (this->z * this->z); }

	float dot(myVector other) { return (this->x * other.x) + (this->y * other.y) + (this->z * other.z); }
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

	myVector& myVector::operator*=(const float& rhs)
	{
		this->x *= rhs;
		this->y *= rhs;
		this->z *= rhs;
		return *this;
	}

	myVector& myVector::operator/=(const float& rhs)
	{
		this->x /= rhs;
		this->y /= rhs;
		this->z /= rhs;
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

	float xBound;
	float yBound;

	GameEntity* mesh;

	bool despawn;
	bool isSoccerBall;
public:
	Ball(GameEntity* mesh, myVector position, myVector velocity, float mass, float radius, bool isMain)
	{
		this->mesh = new GameEntity(mesh);
		this->position = position;
		this->velocity = velocity;
		this->acceleration = myVector(0.f, 0.f, 0.f);
		this->mass = mass;
		this->radius = radius;

		this->xBound = 2.8;
		this->yBound = 1.6;

		this->mesh->SetScale(radius * 2, radius * 2, radius * 2);

		this->despawn = false;
		this->isSoccerBall = isMain;
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
		if (this->position.x + this->radius > this->xBound || this->position.x - this->radius < -this->xBound) {
			if(!this->isSoccerBall) 
				this->despawn = true;
			this->velocity.x *= -1;
		}

		if (this->position.y + this->radius > this->yBound || this->position.y - this->radius < -this->yBound)
			this->velocity.y *= -1;

		this->position.z = -.65f;
	}

	void unUpdate(float deltaTime)
	{
		this->position -= this->velocity * deltaTime;
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

	bool getDespawn() 
	{
		return this->despawn;
	}
};