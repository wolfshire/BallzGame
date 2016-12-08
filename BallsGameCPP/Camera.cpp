#include "Camera.h"
#include "DXCore.h"



Camera::Camera(int width, int height)
{
	// Create the View matrix
	// - In an actual game, recreate this matrix every time the camera 
	//    moves (potentially every frame)
	// - We're using the LOOK TO function, which takes the position of the
	//    camera and the direction vector along which to look (as well as "up")
	// - Another option is the LOOK AT function, to look towards a specific
	//    point in 3D space
	XMVECTOR pos = XMVectorSet(0, 0, -6, 0);
	XMVECTOR dir = XMVectorSet(0, 0, 1, 0);
	XMVECTOR up = XMVectorSet(0, 1, 0, 0);
	XMMATRIX V = XMMatrixLookToLH(
		pos,     // The position of the "camera"
		dir,     // Direction the camera is looking
		up);     // "Up" direction in 3D space (prevents roll)
	XMStoreFloat4x4(&viewMatrix, XMMatrixTranspose(V)); // Transpose for HLSL!

	resizeProjectionMatrix(width, height);

	xRotation = 0;
	yRotation = 0;
	XMStoreFloat3(&position, pos);
	XMStoreFloat3(&direction, dir);

	printf("\nClick a mouse button to rotate the camera\n");
}
 

Camera::~Camera()
{
}

//if dir is true, moving up
void Camera::rotateX(bool dir)
{
	dir ? xRotation -= debugRotateSpeed : xRotation += debugRotateSpeed;
}

//if dir is true, moving to the left
void Camera::rotateY(bool dir)
{
	dir ? yRotation -= debugRotateSpeed : yRotation += debugRotateSpeed;
}

void Camera::rotateX(int value)
{
	xRotation -= value * rotateSpeed;
	//Limit how far you can rotate
	if (xRotation >= 1.5708)
		xRotation = 1.57;
	if (xRotation <= -1.5708)
		xRotation = -1.57;
}

void Camera::rotateY(int value)
{
	yRotation -= value * rotateSpeed;
}

void Camera::resizeProjectionMatrix(int width, int height)
{
	// Create the Projection matrix
	// - This should match the window's aspect ratio, and also update anytime
	//   the window resizes (which is already happening in OnResize() below)
	XMMATRIX P = XMMatrixPerspectiveFovLH(
		0.25f * 3.1415926535f,		// Field of View Angle
		(float)width / height,			// Aspect ratio -- hardcoded from Game Constructor
		0.1f,						// Near clip plane distance
		100.0f);					// Far clip plane distance
	XMStoreFloat4x4(&projectionMatrix, XMMatrixTranspose(P)); // Transpose for HLSL!
}

void Camera::Update(float deltaTime)
{
	XMVECTOR forward = XMVectorSet(0, 0, 1, 0);
	XMVECTOR up = XMVectorSet(0, 1, 0, 0);

	XMVECTOR currentRotation = XMQuaternionRotationRollPitchYaw(xRotation, yRotation, 0);

	XMVECTOR dir = XMVector3Rotate(forward, currentRotation);
	
	//Getting keyboard input
	if (GetAsyncKeyState('T') & 0x8000) { //forward
		position.x += direction.x * deltaTime * cameraSpeed;
		position.y += direction.y * deltaTime * cameraSpeed;
		position.z += direction.z * deltaTime * cameraSpeed;
	}
	if (GetAsyncKeyState('G') & 0x8000) { //back
		position.x -= direction.x * deltaTime * cameraSpeed;
		position.y -= direction.y * deltaTime * cameraSpeed;
		position.z -= direction.z * deltaTime * cameraSpeed;
	}
	if (GetAsyncKeyState('F') & 0x8000) { //left
		XMVECTOR leftRight = XMVector3Cross(up, dir);
		XMFLOAT3 v;
		XMStoreFloat3(&v, leftRight);
		position.x -= v.x * deltaTime * cameraSpeed;
		position.y -= v.y * deltaTime * cameraSpeed;
		position.z -= v.z * deltaTime * cameraSpeed;
	}
	if (GetAsyncKeyState('H') & 0x8000) { //right
		XMVECTOR leftRight = XMVector3Cross(up, dir);
		XMFLOAT3 v;
		XMStoreFloat3(&v, leftRight);
		position.x += v.x * deltaTime * cameraSpeed;
		position.y += v.y * deltaTime * cameraSpeed;
		position.z += v.z * deltaTime * cameraSpeed;
	}
	if (GetAsyncKeyState(VK_SPACE) & 0x8000) { //up
		position.y += cameraSpeed * deltaTime;
	}
	if (GetAsyncKeyState('X') & 0x8000) { //down
		position.y -= cameraSpeed * deltaTime;
	}
	if (GetAsyncKeyState(VK_NUMPAD4) & 0x8000) { //debugRotation
		rotateY(true);
	}
	if (GetAsyncKeyState(VK_NUMPAD6) & 0x8000) { //debugRotation
		rotateY(false);
	}
	if (GetAsyncKeyState(VK_NUMPAD8) & 0x8000) { //debugRotation
		rotateX(true);
	}
	if (GetAsyncKeyState(VK_NUMPAD2) & 0x8000) { //debugRotation
		rotateX(false);
	}

	XMVECTOR pos = XMLoadFloat3(&position);

	XMStoreFloat3(&direction, dir);

	XMMATRIX V = XMMatrixLookToLH(
		pos,     // The position of the "camera"
		dir,     // Direction the camera is looking
		up);     // "Up" direction in 3D space (prevents roll)
	XMStoreFloat4x4(&viewMatrix, XMMatrixTranspose(V)); // Transpose for HLSL!
	
}
