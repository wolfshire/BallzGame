#pragma once

#include <vector>
#include "GameEntity.h"
#include "SimpleShader.h"

class Renderer
{
public:
	Renderer(ID3D11DeviceContext* context);
	~Renderer();

	void SetGameEntityList(std::vector<GameEntity*> list);
	void Draw(XMFLOAT4X4 viewMatrix, XMFLOAT4X4 projectionMatrix);

private:

	std::vector<GameEntity*> gameEntityList;

	XMFLOAT4X4 worldMatrix;
	XMFLOAT4X4 viewMatrix;
	XMFLOAT4X4 projectionMatrix;

	ID3D11DeviceContext* context;

};

