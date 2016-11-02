#include "Material.h"



Material::Material(SimpleVertexShader* vs, SimplePixelShader* ps, ID3D11ShaderResourceView* tx, ID3D11SamplerState* ss)
{
	vertexShader = vs;
	pixelShader = ps;

	texture = tx;
	sampler = ss;

	surfaceColor = XMFLOAT4(1,1,1,1);
}

Material::~Material()
{
}

void Material::PrepareMaterial(XMFLOAT4X4 worldMatrix, XMFLOAT4X4 viewMatrix, XMFLOAT4X4 projectionMatrix)
{
	bool hi;
	vertexShader->SetShader();
	pixelShader->SetShader();
	// Send data to shader variables
	vertexShader->SetMatrix4x4("view", viewMatrix);
	vertexShader->SetMatrix4x4("projection", projectionMatrix);
	vertexShader->SetMatrix4x4("world", worldMatrix);

	hi = pixelShader->SetFloat4("SurfaceColor", surfaceColor);
	hi = pixelShader->SetSamplerState("basicSampler", sampler);
	hi = pixelShader->SetShaderResourceView("Texture", texture);
	hi = pixelShader->SetShaderResourceView("NormalMap", normalMap);

	// Once you've set all of the data you care to change for
	// the next draw call, you need to actually send it to the GPU
	//  - If you skip this, the "SetMatrix" calls above won't make it to the GPU!
	vertexShader->CopyAllBufferData();
	pixelShader->CopyAllBufferData();

	// Set the vertex and pixel shaders to use for the next Draw() command
	//  - These don't technically need to be set every frame...YET
	//  - Once you start applying different shaders to different objects,
	//    you'll need to swap the current shaders before each draw

}

