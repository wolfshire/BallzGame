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
	bool err;
	vertexShader->SetShader();
	pixelShader->SetShader();
	// Send data to shader variables
	vertexShader->SetMatrix4x4("view", viewMatrix);
	vertexShader->SetMatrix4x4("projection", projectionMatrix);
	vertexShader->SetMatrix4x4("world", worldMatrix);

	err = pixelShader->SetFloat4("SurfaceColor", surfaceColor);
	err = pixelShader->SetSamplerState("basicSampler", sampler);
	err = pixelShader->SetShaderResourceView("Texture", texture);
	err = pixelShader->SetShaderResourceView("NormalMap", normalMap);

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
void Material::PrepareMaterial(XMFLOAT4X4 worldMatrix, XMFLOAT4X4 viewMatrix, XMFLOAT4X4 projectionMatrix, XMFLOAT4X4 view, XMFLOAT4X4 proj, ID3D11ShaderResourceView * shadowMap, ID3D11SamplerState* shadowSampler)
{

	vertexShader->SetShader();
	pixelShader->SetShader();
	// Send data to shader variables
	vertexShader->SetMatrix4x4("view", viewMatrix);
	vertexShader->SetMatrix4x4("projection", projectionMatrix);
	vertexShader->SetMatrix4x4("world", worldMatrix);
	vertexShader->SetMatrix4x4("shadowView", view);
	vertexShader->SetMatrix4x4("shadowProjection", proj);

	pixelShader->SetFloat4("SurfaceColor", surfaceColor);
	pixelShader->SetShaderResourceView("Texture", texture);
	pixelShader->SetShaderResourceView("NormalMap", normalMap);
	pixelShader->SetShaderResourceView("ShadowMap", shadowMap);
	pixelShader->SetSamplerState("basicSampler", sampler);
	pixelShader->SetSamplerState("ShadowSampler", shadowSampler);

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

void Material::PrepareMaterial(XMFLOAT4X4 worldMatrix, XMFLOAT4X4 viewMatrix,  XMFLOAT4X4 projectionMatrix, ID3D11ShaderResourceView * skybox, std::vector<XMFLOAT4X4> shadowMatricies, std::vector<ID3D11ShaderResourceView*>  shadowMap, ID3D11SamplerState* shadowSampler)
{

	vertexShader->SetShader();
	pixelShader->SetShader();
	// Send data to shader variables
	vertexShader->SetMatrix4x4("view", viewMatrix);
	vertexShader->SetMatrix4x4("projection", projectionMatrix);
	vertexShader->SetMatrix4x4("world", worldMatrix);
	vertexShader->SetMatrix4x4("shadowView", shadowMatricies[0]);
	vertexShader->SetMatrix4x4("shadowProjection", shadowMatricies[1]);
	vertexShader->SetMatrix4x4("shadowView2", shadowMatricies[2]);
	vertexShader->SetMatrix4x4("shadowProjection2", shadowMatricies[3]);
	vertexShader->SetMatrix4x4("shadowView3", shadowMatricies[4]);
	vertexShader->SetMatrix4x4("shadowProjection3", shadowMatricies[5]);
	vertexShader->SetMatrix4x4("shadowView4", shadowMatricies[6]);
	vertexShader->SetMatrix4x4("shadowProjection4", shadowMatricies[7]);
	
	pixelShader->SetFloat4("SurfaceColor", surfaceColor);
	pixelShader->SetShaderResourceView("Texture", texture);
	pixelShader->SetShaderResourceView("NormalMap", normalMap);
	pixelShader->SetShaderResourceView("ShadowMap", shadowMap[0]);
	pixelShader->SetShaderResourceView("ShadowMap2", shadowMap[1]);
	pixelShader->SetShaderResourceView("ShadowMap3", shadowMap[2]);
	pixelShader->SetShaderResourceView("ShadowMap4", shadowMap[3]);
	pixelShader->SetShaderResourceView("Sky", skybox);
	pixelShader->SetSamplerState("basicSampler", sampler);
	pixelShader->SetSamplerState("ShadowSampler", shadowSampler);

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



