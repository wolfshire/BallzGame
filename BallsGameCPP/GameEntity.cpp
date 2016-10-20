#include "GameEntity.h"


GameEntity::GameEntity(Mesh * mesh, Material* material)
{
	myMesh = mesh;
	myMaterial = material;
	XMMATRIX identity = XMMatrixIdentity();
	XMStoreFloat4x4(&worldMatrix, XMMatrixTranspose(identity));
	myPosition = XMFLOAT3(0, 0, 0);
	myRotation = XMFLOAT3(0, 0, 0);
	myScale = XMFLOAT3(1.0f, 1.0f, 1.0f);
	int i = 0;
}

GameEntity::~GameEntity()
{
}

//Updates the World Matrix for the current position, rotation, and scale
void GameEntity::UpdateWorldMatrix()
{
	XMMATRIX translation = XMMatrixTranslationFromVector(XMLoadFloat3(&myPosition));
	XMMATRIX rotation = XMMatrixRotationRollPitchYawFromVector(XMLoadFloat3(&myRotation));
	XMMATRIX scale = XMMatrixScalingFromVector(XMLoadFloat3(&myScale));

	XMMATRIX world = scale * translation * rotation;

	XMStoreFloat4x4(&worldMatrix, XMMatrixTranspose(world));
}

void GameEntity::Move(float x, float y, float z)
{
	myPosition.x += x;
	myPosition.y += y;
	myPosition.z += z;
}

void GameEntity::Rotate(float x, float y, float z)
{
	 myRotation.x += x;	
	 myRotation.y += y;	
	 myRotation.z += z; 
}

void GameEntity::SetTranslation(float x, float y, float z)
{
	myPosition = XMFLOAT3(x, y, z);
}

void GameEntity::SetRotation(float x, float y, float z)
{
	myRotation = XMFLOAT3(x, y, z);
}

void GameEntity::SetScale(float x, float y, float z)
{
	myScale = XMFLOAT3(x, y, z);
}

Mesh * GameEntity::getMesh()
{
	if (myMesh)
		return myMesh;
	return nullptr;
}

Material * GameEntity::getMaterial()
{
	if (myMaterial)
		return myMaterial;
	return nullptr;
}
