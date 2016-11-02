#pragma once

#include "Vertex.h"
#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>
#include <fstream>

class Mesh
{
public:
	Mesh(Vertex vertexes[], int numVerticies, unsigned int indices[], int numberOfIndicies, ID3D11Device* device);
	Mesh(const char* objFile, ID3D11Device* device);
	~Mesh(void);

	//Get Meathods
	ID3D11Buffer* GetVertexBuffer();
	ID3D11Buffer* GetIndexBuffer();
	int GetIndexCount();

private:

	//Buffers
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;

	int numIndices;

	void CreateBuffers(Vertex* vertArray, int numVerts, unsigned int* indexArray, int numIndices, ID3D11Device* device);
	void CalculateTangents(Vertex * verts, int numVerts, unsigned int * indices, int numIndices);
};

