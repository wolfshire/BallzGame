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
#include "Emitter.h"
#include <string>
#include "Vertex.h"
#include "WICTextureLoader.h"
#include "DDSTextureLoader.h"
#include <algorithm>

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

	void RenderShadowMap(int lightIndex);

	void RenderSkybox();

	// Overridden mouse input helper methods
	void OnMouseDown (WPARAM buttonState, int x, int y);
	void OnMouseUp	 (WPARAM buttonState, int x, int y);
	void OnMouseMove (WPARAM buttonState, int x, int y);
	void OnMouseWheel(float wheelDelta,   int x, int y);

	int* p1Balls;
	int* p2Balls;

	int* p1Score;
	int* p2Score;
private:
	//Gameplay variables
	int gameState;
	int p1Selection;
	int p2Selection;
	float firePeriod;
	float p1shootTimer;
	float p2shootTimer;



	float ballSpeed;

	//List of Game Entities, Meshes, and Materials
	std::vector<GameEntity*> menuEntities;
	std::vector<GameEntity*> gameOver1Entities;
	std::vector<GameEntity*> gameOver2Entities;
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
	PointLight pointLight3;
	PointLight pointLight4;

	//Textures & samplers
	ID3D11ShaderResourceView* gamefield;
	ID3D11ShaderResourceView* gamefieldNormal; //Normal
	ID3D11ShaderResourceView* bricks;
	ID3D11ShaderResourceView* explosion;
	ID3D11ShaderResourceView* woodTexture; 
	ID3D11ShaderResourceView* menu;
	ID3D11ShaderResourceView* redTexture;
	ID3D11ShaderResourceView* blueTexture;
	ID3D11ShaderResourceView* skybox;
	ID3D11ShaderResourceView* skyboxBall;
	ID3D11ShaderResourceView* regularBall;
	ID3D11ShaderResourceView* p1Win;
	ID3D11ShaderResourceView* p2Win;
	ID3D11SamplerState* sampler;
	ID3D11SamplerState* shadowSampler;

	//Shadow Map
	int shadowMapSize;
	ID3D11RasterizerState* shadowRasterizer;
	std::vector<DirectX::XMFLOAT4X4> shadowMatricies; //nth index is the shadow view matrix, and n+1 index is the shadow projection matrix
	std::vector<ID3D11ShaderResourceView*> shadowSRVs;
	std::vector<ID3D11DepthStencilView*> shadowDSVs;

	//Skybox
	ID3D11RasterizerState* skyRastState;
	ID3D11DepthStencilState* skyDepthState;

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
	void CreateSkybox();

	// Wrappers for DirectX shaders to provide simplified functionality
	SimpleVertexShader* vertexShader;
	SimplePixelShader* pixelShader;
	SimpleVertexShader* vertexShaderNormal;
	SimplePixelShader* pixelShaderNormal;
	SimpleVertexShader* vertexShaderSky;
	SimplePixelShader* pixelShaderSky;
	SimplePixelShader* pixelShaderShiny;
	SimpleVertexShader* vertexShaderShadow;

	// The matrices to go from model space to screen space
	DirectX::XMFLOAT4X4 worldMatrix;
	DirectX::XMFLOAT4X4 viewMatrix;
	DirectX::XMFLOAT4X4 projectionMatrix;

	// Keeps track of the old mouse position.  Useful for 
	// determining how far the mouse moved in a single frame.
	POINT prevMousePos;

	BallManager* ballManager;
	std::vector<Emitter*> emitters;

	// Font related objects
	std::unique_ptr<DirectX::SpriteFont> m_font;
	std::unique_ptr<DirectX::SpriteBatch> m_spriteBatch;

	// Font positions
	DirectX::SimpleMath::Vector2 m_p1FontPos;
	DirectX::SimpleMath::Vector2 m_p2FontPos;

	bool DEBUG_MODE;

	int transparentIndex;


	
	ID3D11DepthStencilState* particleDepthState;
	ID3D11BlendState* bsAlphaBlend;
};

