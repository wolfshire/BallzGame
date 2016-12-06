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
	void SetShadowMap(std::vector<DirectX::XMFLOAT4X4> shadowMatricies, std::vector<ID3D11ShaderResourceView*> shadowMaps, ID3D11SamplerState * shadowSampler);
	void Draw(XMFLOAT4X4 viewMatrix, XMFLOAT4X4 projectionMatrix);

private:

	std::vector<GameEntity*> gameEntityList;

	XMFLOAT4X4 worldMatrix;
	XMFLOAT4X4 viewMatrix;
	XMFLOAT4X4 projectionMatrix;

	ID3D11ShaderResourceView* shadowMap;
	ID3D11SamplerState* shadowSampler;

	std::vector<DirectX::XMFLOAT4X4> shadowMatricies;
	std::vector<ID3D11ShaderResourceView*> shadowMaps;

	ID3D11DeviceContext* context;


};

