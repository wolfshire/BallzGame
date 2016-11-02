#pragma once

#include "DXCore.h"
#include "Mesh.h"
#include "Renderer.h"
#include "Camera.h"
#include "Lights.h"
#include <DirectXMath.h>

class Game 
	: public DXCore
{

public:
	Game(HINSTANCE hInstance);
	~Game();

	// Overridden setup and game loop methods, which
	// will be called automatically
	void Init();
	void OnResize();
	void Update(float deltaTime, float totalTime);
	void Draw(float deltaTime, float totalTime);

	// Overridden mouse input helper methods
	void OnMouseDown (WPARAM buttonState, int x, int y);
	void OnMouseUp	 (WPARAM buttonState, int x, int y);
	void OnMouseMove (WPARAM buttonState, int x, int y);
	void OnMouseWheel(float wheelDelta,   int x, int y);
private:

	int gameState;

	//List of Game Entities, Meshes, and Materials
	std::vector<GameEntity*> menuEntities;
	std::vector<GameEntity*> gameEntities;
	std::vector<GameEntity*> currentGameEntities;
	std::vector<Mesh*> meshes;
	std::vector<Material*> materials;

	Renderer* renderer;
	Camera* mainCamera; 

	//lights
	DirectionalLight dirLight1;
	PointLight pointLight1;
	PointLight pointLight2;

	//Textures & samplers
	ID3D11ShaderResourceView* gamefield;
	ID3D11ShaderResourceView* gamefieldNormal; //Normal
	ID3D11ShaderResourceView* bricks;
	ID3D11ShaderResourceView* woodTexture; 
	ID3D11ShaderResourceView* menu;
	ID3D11SamplerState* sampler1;

	bool mouseDown;

	// Initialization helper methods - feel free to customize, combine, etc.
	void LoadShaders(); 
	void CreateMatrices();
	void CreateBasicGeometry();
	void CreateMenu();
	void CreateGameField();
	void CreateLights();

	// Wrappers for DirectX shaders to provide simplified functionality
	SimpleVertexShader* vertexShader;
	SimplePixelShader* pixelShader;
	SimpleVertexShader* vertexShaderNormal;
	SimplePixelShader* pixelShaderNormal;

	// The matrices to go from model space to screen space
	DirectX::XMFLOAT4X4 worldMatrix;
	DirectX::XMFLOAT4X4 viewMatrix;
	DirectX::XMFLOAT4X4 projectionMatrix;

	// Keeps track of the old mouse position.  Useful for 
	// determining how far the mouse moved in a single frame.
	POINT prevMousePos;

};

