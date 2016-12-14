#include "Renderer.h"



Renderer::Renderer(ID3D11DeviceContext* deviceContext)
{
	context = deviceContext;
}


Renderer::~Renderer()
{
	
}

//Sets the list of game entities to draw this frame
void Renderer::SetGameEntityList(std::vector<GameEntity*> list)
{
	gameEntityList = list;
	this->transparentIndex = gameEntityList.size();
}

void Renderer::SetGameEntityList(std::vector<GameEntity*> list, int transparentIndex)
{
	gameEntityList = list;
	this->transparentIndex = transparentIndex;
}

void Renderer::SetShadowMap(std::vector<DirectX::XMFLOAT4X4> shadowMatricies, std::vector<ID3D11ShaderResourceView*> shadowMaps, ID3D11SamplerState* shadowSampler)
{
	this->shadowMatricies = shadowMatricies;
	this->shadowMaps = shadowMaps;
	this->shadowSampler = shadowSampler;
}

void Renderer::SetSkybox(ID3D11ShaderResourceView* sky)
{
	skybox = sky;
}

void Renderer::SetPaticleInfo(ID3D11DepthStencilState* particleDepthState, ID3D11BlendState* bsAlphaBlend)
{
	this->particleDepthState = particleDepthState;
	this->particleBlendState = bsAlphaBlend;
}

void Renderer::Draw(XMFLOAT4X4 viewMatrix, XMFLOAT4X4 projectionMatrix)
{
	if (gameEntityList.size() != 0)
	{
		UINT stride = sizeof(Vertex);
		UINT offset = 0;

		ID3D11Buffer* vertexBuff;

		// Reset to default states for next frame
		float blend[4] = { 1,1,1,1 };
		context->OMSetBlendState(0, blend, 0xffffffff);
		context->OMSetDepthStencilState(0, 0);
		

		for(int i = 0; i < gameEntityList.size(); i++)
		{
			GameEntity* gameEntity = gameEntityList[i];

			//Update the World Matrix using the current position, rotation, and scale
			gameEntity->UpdateWorldMatrix();
			gameEntity->getMaterial()->PrepareMaterial(
				gameEntity->getWorldMatrix(),
				viewMatrix,
				projectionMatrix,
				skybox,
				shadowMatricies,
				shadowMaps,
				shadowSampler
			);



			Mesh* tempMesh = gameEntity->getMesh();

			//checking if a mesh exists or not
			if (tempMesh) {

				if (i >= transparentIndex)
				{
					context->OMSetBlendState(particleBlendState, blend, 0xffffffff);  // Additive blending
					context->OMSetDepthStencilState(particleDepthState, 0);			// No depth WRITING
				}

				vertexBuff = tempMesh->GetVertexBuffer();
				context->IASetVertexBuffers(0, 1, &vertexBuff, &stride, &offset);
				context->IASetIndexBuffer(tempMesh->GetIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);

				context->DrawIndexed(
					tempMesh->GetIndexCount(),     // The number of indices to use (we could draw a subset if we wanted)
					0,     // Offset to the first index we want to use
					0);    // Offset to add to each index when looking up vertices
			}

		}

		context->OMSetBlendState(0, blend, 0xffffffff);
		context->OMSetDepthStencilState(0, 0);

	}
}


