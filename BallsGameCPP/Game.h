#pragma once

#include "DXCore.h"
#include "Mesh.h"
#include "Renderer.h"
#include "Camera.h"
#include "Lights.h"
#include <DirectXMath.h>
#include "BallManager.h"
#include "SpriteFont.h"
#include "SimpleMath.h"

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

	void RenderShadowMap();

	// Overridden mouse input helper methods
	void OnMouseDown (WPARAM buttonState, int x, int y);
	void OnMouseUp	 (WPARAM buttonState, int x, int y);
	void OnMouseMove (WPARAM buttonState, int x, int y);
	void OnMouseWheel(float wheelDelta,   int x, int y);
private:
	//Gameplay variables
	int gameState;
	int p1Selection;
	int p2Selection;

	//List of Game Entities, Meshes, and Materials
	std::vector<GameEntity*> menuEntities;
	std::vector<GameEntity*> gameEntities;
	std::vector<GameEntity*> p1SelectEntities;
	std::vector<GameEntity*> p2SelectEntities;
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
	ID3D11ShaderResourceView* redTexture;
	ID3D11ShaderResourceView* blueTexture;
	ID3D11SamplerState* sampler;
	ID3D11SamplerState* shadowSampler;

	//Shadow Map
	int shadowMapSize;
	ID3D11DepthStencilView* shadowDSV;
	ID3D11ShaderResourceView* shadowSRV;
	ID3D11RasterizerState* shadowRasterizer;
	DirectX::XMFLOAT4X4 shadowViewMatrix;
	DirectX::XMFLOAT4X4 shadowProjectionMatrix;

	bool mouseDown;

	// Initialization helper methods - feel free to customize, combine, etc.
	void LoadShaders(); 
	void CreateMatrices();
	void CreateBasicGeometry();
	void CreateMenu();
	void CreateGameField();
	void CreateLights();
	void SortCurrentEntities();

	void CreateShadowMap();

	// Wrappers for DirectX shaders to provide simplified functionality
	SimpleVertexShader* vertexShader;
	SimplePixelShader* pixelShader;
	SimpleVertexShader* vertexShaderNormal;
	SimplePixelShader* pixelShaderNormal;
	SimpleVertexShader* vertexShaderShadow;

	// The matrices to go from model space to screen space
	DirectX::XMFLOAT4X4 worldMatrix;
	DirectX::XMFLOAT4X4 viewMatrix;
	DirectX::XMFLOAT4X4 projectionMatrix;

	// Keeps track of the old mouse position.  Useful for 
	// determining how far the mouse moved in a single frame.
	POINT prevMousePos;

	BallManager* ballManager;

	// Font related objects
	std::unique_ptr<DirectX::SpriteFont> m_font;
	std::unique_ptr<DirectX::SpriteBatch> m_spriteBatch;

	// Font positions
	DirectX::SimpleMath::Vector2 m_p1FontPos;
	DirectX::SimpleMath::Vector2 m_p2FontPos;

	bool DEBUG_MODE;
};

