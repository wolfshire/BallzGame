#pragma once

#include "SimpleShader.h"

using namespace DirectX;

class Material
{
public:
	Material(SimpleVertexShader* vs, SimplePixelShader* ps, ID3D11ShaderResourceView* tx, ID3D11SamplerState* ss);
	~Material();

	SimpleVertexShader* getVertexShader() { return vertexShader; }
	SimplePixelShader* getPixelShader() { return pixelShader; }
	ID3D11ShaderResourceView* getShaderResourceView() { return texture; }
	ID3D11SamplerState* getSamplerState() { return sampler; }

	void PrepareMaterial(XMFLOAT4X4 worldMatrix, XMFLOAT4X4 viewMatrix, XMFLOAT4X4 projectionMatrix);

	void PrepareMaterial(XMFLOAT4X4 worldMatrix, XMFLOAT4X4 viewMatrix, XMFLOAT4X4 projectionMatrix, XMFLOAT4X4 view, XMFLOAT4X4 proj, ID3D11ShaderResourceView * shadowMap, ID3D11SamplerState * shadowSampler);

	void PrepareMaterial(XMFLOAT4X4 worldMatrix, XMFLOAT4X4 viewMatrix, XMFLOAT4X4 projectionMatrix, std::vector<XMFLOAT4X4> shadowMatricies, std::vector<ID3D11ShaderResourceView*> shadowMap, ID3D11SamplerState * shadowSampler);

	void PrepareMaterial(XMFLOAT4X4 worldMatrix, XMFLOAT4X4 viewMatrix, XMFLOAT4X4 projectionMatrix, std::vector<XMFLOAT4X4> shadowMatricies, ID3D11ShaderResourceView * shadowMap, ID3D11SamplerState * shadowSampler);

	void SetSurfaceColor(XMFLOAT4 color) { surfaceColor = color; }
	void AddNormalMap(ID3D11ShaderResourceView* n) { normalMap = n; }

private:

	SimpleVertexShader* vertexShader;
	SimplePixelShader* pixelShader;

	XMFLOAT4 surfaceColor;

	ID3D11ShaderResourceView* texture;
	ID3D11ShaderResourceView* normalMap;
	ID3D11SamplerState* sampler;

};

