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
	void SetShadowMap(XMFLOAT4X4 view, XMFLOAT4X4 proj, ID3D11ShaderResourceView * shadowMap, ID3D11SamplerState* shadowSampler);
	void Draw(XMFLOAT4X4 viewMatrix, XMFLOAT4X4 projectionMatrix);

private:

	std::vector<GameEntity*> gameEntityList;

	XMFLOAT4X4 worldMatrix;
	XMFLOAT4X4 viewMatrix;
	XMFLOAT4X4 projectionMatrix;

	XMFLOAT4X4 shadowView;
	XMFLOAT4X4 shadowProjection;
	ID3D11ShaderResourceView* shadowMap;
	ID3D11SamplerState* shadowSampler;


	ID3D11DeviceContext* context;

};

