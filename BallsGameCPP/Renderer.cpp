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
}

void Renderer::SetShadowMap(XMFLOAT4X4 view, XMFLOAT4X4 proj, ID3D11ShaderResourceView* shadowMap, ID3D11SamplerState* shadowSampler)
{
	shadowView = view;
	shadowProjection = proj;
	this->shadowMap = shadowMap;
	this->shadowSampler = shadowSampler;
}

void Renderer::Draw(XMFLOAT4X4 viewMatrix, XMFLOAT4X4 projectionMatrix)
{
	if (gameEntityList.size() != 0)
	{
		UINT stride = sizeof(Vertex);
		UINT offset = 0;

		ID3D11Buffer* vertexBuff;

		for each (GameEntity* gameEntity in gameEntityList)
		{
			//Update the World Matrix using the current position, rotation, and scale
			gameEntity->UpdateWorldMatrix();
			gameEntity->getMaterial()->PrepareMaterial(
				gameEntity->getWorldMatrix(),
				viewMatrix,
				projectionMatrix,
				shadowView,
				shadowProjection,
				shadowMap,
				shadowSampler
			);



			Mesh* tempMesh = gameEntity->getMesh();

			//checking if a mesh exists or not
			if (tempMesh) {
				vertexBuff = tempMesh->GetVertexBuffer();
				context->IASetVertexBuffers(0, 1, &vertexBuff, &stride, &offset);
				context->IASetIndexBuffer(tempMesh->GetIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);

				context->DrawIndexed(
					tempMesh->GetIndexCount(),     // The number of indices to use (we could draw a subset if we wanted)
					0,     // Offset to the first index we want to use
					0);    // Offset to add to each index when looking up vertices
			}
		}
	}
}


