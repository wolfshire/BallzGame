#pragma once
#include "DirectXMath.h"

using namespace DirectX;

class Camera
{
public:
	Camera(int width, int height);
	~Camera();

	XMFLOAT4X4 getViewMatrix() { return viewMatrix; }
	XMFLOAT4X4 getProjectionMatrix() { return projectionMatrix; }
	XMFLOAT3 getPosition() { return position; }

	void rotateX(bool dir);
	void rotateY(bool dir);
	void rotateX(int value);
	void rotateY(int value);

	void resizeProjectionMatrix(int width, int height);

	void Update(float deltaTime);

private:
	
	XMFLOAT3 position;
	XMFLOAT3 direction;

	XMFLOAT4X4 viewMatrix;
	XMFLOAT4X4 projectionMatrix;

	float xRotation;
	float yRotation;

	const float cameraSpeed = 2.5f;
	const float debugRotateSpeed = .0001f;
	const float rotateSpeed = .0025f;
};

