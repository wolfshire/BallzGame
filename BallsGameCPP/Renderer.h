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
	void SetGameEntityList(std::vector<GameEntity*> list, int transparentIndex);
	void SetShadowMap(std::vector<DirectX::XMFLOAT4X4> shadowMatricies, std::vector<ID3D11ShaderResourceView*> shadowMaps, ID3D11SamplerState * shadowSampler);
	void SetSkybox(ID3D11ShaderResourceView * sky);
	void SetPaticleInfo(ID3D11DepthStencilState * particleDepthState, ID3D11BlendState * bsAlphaBlend);
	void Draw(XMFLOAT4X4 viewMatrix, XMFLOAT4X4 projectionMatrix);

private:

	std::vector<GameEntity*> gameEntityList;

	XMFLOAT4X4 worldMatrix;
	XMFLOAT4X4 viewMatrix;
	XMFLOAT4X4 projectionMatrix;

	ID3D11ShaderResourceView* skybox;

	ID3D11ShaderResourceView* shadowMap;
	ID3D11SamplerState* shadowSampler;

	std::vector<DirectX::XMFLOAT4X4> shadowMatricies;
	std::vector<ID3D11ShaderResourceView*> shadowMaps;

	ID3D11DeviceContext* context;

	int transparentIndex;
	ID3D11DepthStencilState* particleDepthState;
	ID3D11BlendState* particleBlendState;


};

