#pragma once

#include "Mesh.h"
#include "Material.h"
#include "DirectXMath.h"

using namespace DirectX;

class GameEntity
{
public:
	GameEntity(Mesh* mesh, Material* material);
	GameEntity(GameEntity* copy);
	~GameEntity();

	void UpdateWorldMatrix();

	void Move(float x, float y, float z);
	void Rotate(float x, float y, float z);

	void SetTranslation(float x, float y, float z);
	void SetRotation(float x, float y, float z);
	void SetScale(float x, float y, float z);

	XMFLOAT4X4 getWorldMatrix() { return worldMatrix; }
	XMFLOAT3 getPosition() { return myPosition; }
	XMFLOAT3 getRotation() { return myRotation; }
	XMFLOAT3 getScale() { return myScale; }
	Mesh* getMesh(); 
	Material* getMaterial(); 

private:
	
	XMFLOAT4X4 worldMatrix;
	XMFLOAT3 myPosition;
	XMFLOAT3 myRotation;
	XMFLOAT3 myScale;

	Mesh* myMesh;
	Material* myMaterial;

};

