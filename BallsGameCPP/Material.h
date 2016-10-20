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

	void SetSurfaceColor(XMFLOAT4 color) { surfaceColor = color; }

private:

	SimpleVertexShader* vertexShader;
	SimplePixelShader* pixelShader;

	XMFLOAT4 surfaceColor;

	ID3D11ShaderResourceView* texture;
	ID3D11SamplerState* sampler;
};

