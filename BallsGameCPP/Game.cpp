#include "Game.h"
#include "Vertex.h"
#include "WICTextureLoader.h"
#include <algorithm>

// For the DirectX Math library
using namespace DirectX;

#define PI 3.14159265359f

// --------------------------------------------------------
// Constructor
//
// DXCore (base class) constructor will set up underlying fields.
// DirectX itself, and our window, are not ready yet!
//
// hInstance - the application's OS-level handle (unique ID)
// --------------------------------------------------------
Game::Game(HINSTANCE hInstance)
	: DXCore( 
		hInstance,		   // The application's handle
		"Ball Game",	   // Text for the window's title bar
		1280,			   // Width of the window's client area
		720,			   // Height of the window's client area
		true)			   // Show extra stats (fps) in title bar?
{

	vertexShader = 0;
	pixelShader = 0;
	vertexShaderNormal = 0;
	pixelShaderNormal = 0;

#if defined(DEBUG) || defined(_DEBUG)
	// Do we want a console window?  Probably only in debug mode
	CreateConsoleWindow(500, 120, 32, 120);
	printf("Console window created successfully.  Feel free to printf() here.\n");
#endif
}

// --------------------------------------------------------
// Destructor - Clean up anything our game has created:
//  - Release all DirectX objects created here
//  - Delete any objects to prevent memory leaks
// --------------------------------------------------------
Game::~Game()
{
	// Delete our simple shader objects, which
	// will clean up their own internal DirectX stuff
	delete vertexShader;
	delete pixelShader;
	delete vertexShaderNormal;
	delete pixelShaderNormal;
	delete vertexShaderShadow;

	delete renderer;
	delete mainCamera;

	shadowDSV->Release();
	shadowSRV->Release();
	shadowRasterizer->Release();
	shadowSampler->Release();

	sampler->Release();
	gamefield->Release();
	gamefieldNormal->Release();
	bricks->Release();
	woodTexture->Release();
	menu->Release();
	redTexture->Release();
	blueTexture->Release();

	if (ballManager) delete ballManager;
	//Deleting materials
	for each (Material* name in materials)
	{
		delete name;
	}
	//Deleteing Meshes
	for each (Mesh* name in meshes)
	{
		delete name;
	}
	//Deleting GameEntitys
	for each (GameEntity* name in gameEntities)
	{
		delete name;
	}
	for each(GameEntity* name in menuEntities)
	{
		delete name;
	}
	for each(GameEntity* name in p1SelectEntities)
	{
		delete name;
	}
	for each(GameEntity* name in p2SelectEntities)
	{
		delete name;
	}

	// Clean up font
	m_font.reset();
	m_spriteBatch.reset();
}

// --------------------------------------------------------
// Called once per program, after DirectX and the window
// are initialized but before the game loop.
// --------------------------------------------------------
void Game::Init()
{
	gameState = 1;
	p1Selection = 3;
	p2Selection = 3;

	shadowMapSize = 1024;
	
	ballManager = new BallManager();

	// Helper methods for loading shaders, creating some basic
	// geometry to draw and some simple camera matrices.
	//  - You'll be expanding and/or replacing these later
	LoadShaders();
	CreateMatrices();
	CreateBasicGeometry();

	// Tell the input assembler stage of the pipeline what kind of
	// geometric primitives (points, lines or triangles) we want to draw.  
	// Essentially: "What kind of shape should the GPU draw with our data?"
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Creating the renderer and passing it the shaders --added
	renderer = new Renderer(context);

	mainCamera = new Camera(width, height);

	mouseDown = false;

	CreateGameField();

	CreateLights();

	CreateShadowMap();

	CreateMenu();

	// Initialize font related objects
	m_font.reset(new SpriteFont(device, L"myfile.spritefont"));
	m_spriteBatch.reset(new SpriteBatch(context));

	// Position of the first font object
	m_p1FontPos.x = width / 6;
	m_p1FontPos.y = height / 16;

	// Position of the second font object
	m_p2FontPos.x = width * 5 / 6;
	m_p2FontPos.y = height / 16;

	DEBUG_MODE = true;
}

// --------------------------------------------------------
// Loads shaders from compiled shader object (.cso) files using
// my SimpleShader wrapper for DirectX shader manipulation.
// - SimpleShader provides helpful methods for sending
//   data to individual variables on the GPU
// --------------------------------------------------------
void Game::LoadShaders()
{
	vertexShader = new SimpleVertexShader(device, context);
	if (!vertexShader->LoadShaderFile(L"Debug/VertexShader.cso"))
		vertexShader->LoadShaderFile(L"VertexShader.cso");		

	pixelShader = new SimplePixelShader(device, context);
	if(!pixelShader->LoadShaderFile(L"Debug/PixelShader.cso"))	
		pixelShader->LoadShaderFile(L"PixelShader.cso");

	vertexShaderNormal = new SimpleVertexShader(device, context);
	if (!vertexShaderNormal->LoadShaderFile(L"Debug/VertexShaderNormal.cso"))
		vertexShaderNormal->LoadShaderFile(L"VertexShaderNormal.cso");

	pixelShaderNormal = new SimplePixelShader(device, context);
	if (!pixelShaderNormal->LoadShaderFile(L"Debug/PixelShaderNormal.cso"))
		pixelShaderNormal->LoadShaderFile(L"PixelShaderNormal.cso");

	vertexShaderShadow = new SimpleVertexShader(device, context);
	if (!vertexShaderShadow->LoadShaderFile(L"Debug/vertexShaderShadow.cso"))
		vertexShaderShadow->LoadShaderFile(L"vertexShaderShadow.cso");

	// You'll notice that the code above attempts to load each
	// compiled shader file (.cso) from two different relative paths.

	// This is because the "working directory" (where relative paths begin)
	// will be different during the following two scenarios:
	//  - Debugging in VS: The "Project Directory" (where your .cpp files are) 
	//  - Run .exe directly: The "Output Directory" (where the .exe & .cso files are)

	// Checking both paths is the easiest way to ensure both 
	// scenarios work correctly, although others exist
}



// --------------------------------------------------------
// Initializes the matrices necessary to represent our geometry's 
// transformations and our 3D camera
// --------------------------------------------------------
void Game::CreateMatrices()
{
	// Set up world matrix
	// - In an actual game, each object will need one of these and they should
	//   update when/if the object moves (every frame)
	// - You'll notice a "transpose" happening below, which is redundant for
	//   an identity matrix.  This is just to show that HLSL expects a different
	//   matrix (column major vs row major) than the DirectX Math library
	XMMATRIX W = XMMatrixIdentity();
	XMStoreFloat4x4(&worldMatrix, XMMatrixTranspose(W)); // Transpose for HLSL!

	XMMATRIX shadowViewLightOne = XMMatrixLookAtLH(
		DirectX::XMVectorSet(1.5f, -2.0f, -3.0f, 0.0f),
		DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f),
		DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
	XMStoreFloat4x4(&shadowViewMatrix, XMMatrixTranspose(shadowViewLightOne));

	XMMATRIX shadowProjectionLightOne = XMMatrixOrthographicLH(10.0f, 10.0f, 0.1f, 100.0f);
	XMStoreFloat4x4(&shadowProjectionMatrix, XMMatrixTranspose(shadowProjectionLightOne));

}


// --------------------------------------------------------
// Creates the geometry we're going to draw - a single triangle for now
// --------------------------------------------------------
void Game::CreateBasicGeometry()
{

	//Creating Textures
	HRESULT check = CreateWICTextureFromFile(
		device,
		context,
		L"Assets/Textures/grass.png",
		0,
		&gamefield
	);

	check = CreateWICTextureFromFile(
		device,
		context,
		L"Assets/Textures/grassNormal.png",
		0,
		&gamefieldNormal
	);

	check = CreateWICTextureFromFile(
		device,
		context,
		L"Assets/Textures/soccer.png",
		0,
		&bricks
	);

	check = CreateWICTextureFromFile(
		device,
		context,
		L"Assets/Textures/mainmenu.png",
		0,
		&menu
	);

	check = CreateWICTextureFromFile(
		device,
		context,
		L"Assets/Textures/wood.jpg",
		0,
		&woodTexture
	);

	check = CreateWICTextureFromFile(
		device,
		context,
		L"Assets/Textures/blueTexture.jpg",
		0,
		&blueTexture
		);

	check = CreateWICTextureFromFile(
		device,
		context,
		L"Assets/Textures/redTexture.jpg",
		0,
		&redTexture
		);


	D3D11_SAMPLER_DESC samplerDesc = {};
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.MaxAnisotropy = 16;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	check = device->CreateSamplerState(&samplerDesc, &sampler);

	//Creating Meshes
	meshes.push_back(new Mesh("../Assets/Models/cube.obj", device));									//meshes[0] - > Cube Model
	meshes.push_back(new Mesh("../Assets/Models/sphere.obj", device));									//meshes[1] - > Sphere Model

	//Creating materials
	//materials with normals
	materials.push_back(new Material(vertexShaderNormal, pixelShaderNormal, gamefield, sampler));		// materials[0] -> basic material, grassy field texture, has normal
	materials[0]->AddNormalMap(gamefieldNormal);
	
	//regular materials
	materials.push_back(new Material(vertexShader, pixelShader, bricks, sampler));						// materials[1] -> basic material, brick texture
	materials.push_back(new Material(vertexShader, pixelShader, menu, sampler));						// materials[2] -> basic material, main menu screen
	materials.push_back(new Material(vertexShader, pixelShader, woodTexture, sampler));					// materials[3] -> basic material, wood texture
	materials.push_back(new Material(vertexShader, pixelShader, redTexture, sampler));					// materials[4] -> basic material, red texture
	materials.push_back(new Material(vertexShader, pixelShader, blueTexture, sampler));					// materials[5] -> basic material, blue texture

	
	//Setting material Color -Debug
	//materials[2]->SetSurfaceColor(XMFLOAT4(1, 1, 1, 1));

	//Creating Field GameEntities
	gameEntities.push_back(new GameEntity(meshes[0], materials[0]));									// gameEntities[0] -> Game Field (Cube/Grass)
	gameEntities.push_back(new GameEntity(meshes[0], materials[3]));									// gameEntities[1] -> Top Wall (Cube/Wood)
	gameEntities.push_back(new GameEntity(meshes[0], materials[3]));									// gameEntities[2] -> Bottom Wall (Cube/Wood)
	gameEntities.push_back(new GameEntity(meshes[0], materials[3]));									// gameEntities[3] -> Left Wall (Cube/Wood)
	gameEntities.push_back(new GameEntity(meshes[0], materials[3]));									// gameEntities[4] -> Right Wall (Cube/Wood)

	//Creating Ball GameEntities
	gameEntities.push_back(new GameEntity(meshes[1], materials[1]));									// gameEntities[5] -> Ball (Sphere/Brick)
	gameEntities.push_back(new GameEntity(meshes[1], materials[3]));									// gameEntities[6] -> Ball (Sphere/Wood)
	gameEntities.push_back(new GameEntity(meshes[1], materials[1]));									// gameEntities[7] -> Ball (Sphere/Brick)

	//Creating player ball spawn objects
	p1SelectEntities.push_back(new GameEntity(meshes[0], materials[4])); //0
	p1SelectEntities.push_back(new GameEntity(meshes[0], materials[4])); //1
	p1SelectEntities.push_back(new GameEntity(meshes[0], materials[4])); //2
	p1SelectEntities.push_back(new GameEntity(meshes[0], materials[5])); //3
	p1SelectEntities.push_back(new GameEntity(meshes[0], materials[4])); //4
	p1SelectEntities.push_back(new GameEntity(meshes[0], materials[4])); //5
	p1SelectEntities.push_back(new GameEntity(meshes[0], materials[4])); //6

	p2SelectEntities.push_back(new GameEntity(meshes[0], materials[4])); //0
	p2SelectEntities.push_back(new GameEntity(meshes[0], materials[4])); //1
	p2SelectEntities.push_back(new GameEntity(meshes[0], materials[4])); //2
	p2SelectEntities.push_back(new GameEntity(meshes[0], materials[5])); //3
	p2SelectEntities.push_back(new GameEntity(meshes[0], materials[4])); //4
	p2SelectEntities.push_back(new GameEntity(meshes[0], materials[4])); //5
	p2SelectEntities.push_back(new GameEntity(meshes[0], materials[4])); //6

	//Creating MenuEntities
	menuEntities.push_back(new GameEntity(meshes[0], materials[2]));									// menuEntities[0] -> Menu
	
	//Adding balls to the manager
	ballManager->addBall(gameEntities[5], myVector(0, 0, .65f), myVector(0,.5f, 0), 1, .25, true);
	

	//Setting Scales
	/*for(int i = 0; i < gameEntities.size(); i++)
		gameEntities[i]->SetScale(2, 2, 2);*/

	/*currentGameEntities.push_back(gameEntities[0]);*/

	
}

void Game::CreateMenu() {
	menuEntities[0]->Rotate(0.0f, 0.0f, PI / 2.0f);
	menuEntities[0]->SetScale(4.0f, 7.0f, 1.0f);
}

void Game::CreateGameField() {

	//creating the floor
	gameEntities[0]->Rotate(0.0f, 0.0f, PI/2.0f);
	gameEntities[0]->SetScale(3.2f, 5.5f, 1.0f);
	//currentGameEntities.push_back(gameEntities[0]);

	//Creating the walls
	//top
	gameEntities[1]->Rotate(0.0f, PI / 2.0f, 0.0f);
	gameEntities[1]->SetScale(1.5f, .25f, 5.5f);
	gameEntities[1]->SetTranslation(0.25f, 1.73f, 0.0f);
	//currentGameEntities.push_back(gameEntities[1]);
	//bottom
	gameEntities[2]->Rotate(0.0f, PI / 2.0f, 0.0f);
	gameEntities[2]->SetScale(1.5f, .25f, 5.5f);
	gameEntities[2]->SetTranslation(0.25f, -1.73f, 0.0f);
	//currentGameEntities.push_back(gameEntities[2]);
	//left
	gameEntities[3]->Rotate(PI / 2.0f, PI / 2.0f, 0.0f);
	gameEntities[3]->SetScale(1.5f, .5f, 3.72f);
	gameEntities[3]->SetTranslation(0.25f, -3.0f, 0.0f);
	//currentGameEntities.push_back(gameEntities[3]);
	//right
	gameEntities[4]->Rotate(PI / 2.0f, PI / 2.0f, 0.0f);
	gameEntities[4]->SetScale(1.5f, .5f, 3.72f);
	gameEntities[4]->SetTranslation(0.25f, 3.0f, 0.0f);
	//currentGameEntities.push_back(gameEntities[4]);

	//Player ball spawns
	p1SelectEntities[0]->SetScale(0.1f, 0.1f, 0.1f);
	p1SelectEntities[0]->SetTranslation(-2.6f, 1.2f, -0.5f);
	p1SelectEntities[1]->SetScale(0.1f, 0.1f, 0.1f);
	p1SelectEntities[1]->SetTranslation(-2.6f, 0.8f, -0.5f);
	p1SelectEntities[2]->SetScale(0.1f, 0.1f, 0.1f);
	p1SelectEntities[2]->SetTranslation(-2.6f, 0.4f, -0.5f);
	p1SelectEntities[3]->SetScale(0.1f, 0.1f, 0.1f);
	p1SelectEntities[3]->SetTranslation(-2.6f, 0.0f, -0.5f);
	p1SelectEntities[4]->SetScale(0.1f, 0.1f, 0.1f);
	p1SelectEntities[4]->SetTranslation(-2.6f, -0.4f, -0.5f);
	p1SelectEntities[5]->SetScale(0.1f, 0.1f, 0.1f);
	p1SelectEntities[5]->SetTranslation(-2.6f, -0.8f, -0.5f);
	p1SelectEntities[6]->SetScale(0.1f, 0.1f, 0.1f);
	p1SelectEntities[6]->SetTranslation(-2.6f, -1.2f, -0.5f);

	p2SelectEntities[0]->SetScale(0.1f, 0.1f, 0.1f);
	p2SelectEntities[0]->SetTranslation(2.6f, 1.2f, -0.5f);
	p2SelectEntities[1]->SetScale(0.1f, 0.1f, 0.1f);
	p2SelectEntities[1]->SetTranslation(2.6f, 0.8f, -0.5f);
	p2SelectEntities[2]->SetScale(0.1f, 0.1f, 0.1f);
	p2SelectEntities[2]->SetTranslation(2.6f, 0.4f, -0.5f);
	p2SelectEntities[3]->SetScale(0.1f, 0.1f, 0.1f);
	p2SelectEntities[3]->SetTranslation(2.6f, 0.0f, -0.5f);
	p2SelectEntities[4]->SetScale(0.1f, 0.1f, 0.1f);
	p2SelectEntities[4]->SetTranslation(2.6f, -0.4f, -0.5f);
	p2SelectEntities[5]->SetScale(0.1f, 0.1f, 0.1f);
	p2SelectEntities[5]->SetTranslation(2.6f, -0.8f, -0.5f);
	p2SelectEntities[6]->SetScale(0.1f, 0.1f, 0.1f);
	p2SelectEntities[6]->SetTranslation(2.6f, -1.2f, -0.5f);
}

// --------------------------------------------------------
// Creates the lights
// --------------------------------------------------------
void Game::CreateLights() {
	dirLight1.AmbientColor = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	dirLight1.DiffuseColor = XMFLOAT4(.5f, .5f, .5f, 1);
	dirLight1.Direction = XMFLOAT3(0, 0, 1);

	pointLight1.Position = XMFLOAT3(1.5f, -2.0f, -3.0f);
	pointLight1.Color = XMFLOAT4(0.1f, 0.1f, 0.1f, 1);

	pointLight2.Position = XMFLOAT3(2.0f, 3.0f, -3.0f);
	pointLight2.Color = XMFLOAT4(0.1f, 0.1f, 0.1f, 1);

	pixelShader->SetData(
		"DirLightOne",				//The name of the variable in the pixel shader
		&dirLight1,					//The address of the data to copy
		sizeof(DirectionalLight));  //Size of data to copy

	pixelShader->SetData(
		"PointLightOne",
		&pointLight1,
		sizeof(PointLight));

	pixelShader->SetData(
		"PointLightTwo",
		&pointLight2,
		sizeof(PointLight));

	pixelShaderNormal->SetData(
		"DirLightOne",				//The name of the variable in the pixel shader
		&dirLight1,					//The address of the data to copy
		sizeof(DirectionalLight));  //Size of data to copy

	pixelShaderNormal->SetData(
		"PointLightOne",
		&pointLight1,
		sizeof(PointLight));

	pixelShaderNormal->SetData(
		"PointLightTwo",
		&pointLight2,
		sizeof(PointLight));
}

void Game::SortCurrentEntities() {
	currentGameEntities.clear();
	currentGameEntities.push_back(gameEntities[0]); //gamefield
	currentGameEntities.push_back(gameEntities[1]); //top wall
	currentGameEntities.push_back(gameEntities[2]);	//bottom wall
	currentGameEntities.push_back(gameEntities[3]); //left wall
	currentGameEntities.push_back(gameEntities[4]);	//right wall

	//player ball spawn locations
	for each(auto e in p1SelectEntities)
		currentGameEntities.push_back(e);
	for each(auto e in p2SelectEntities)
		currentGameEntities.push_back(e);

	//getting all the ball Game Entities and adding them to the current entity list
	std::vector<GameEntity*> list = ballManager->getBallGameEntities(); 
	for each(auto e in list) {
		currentGameEntities.push_back(e);
	}
}

//Creates the Shadow Map components
void Game::CreateShadowMap()
{
	//Create the shadow map texture
	D3D11_TEXTURE2D_DESC shadowDesc = {};
	shadowDesc.Width = shadowMapSize;
	shadowDesc.Height = shadowMapSize;
	shadowDesc.ArraySize = 1;
	shadowDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	shadowDesc.CPUAccessFlags = 0;
	shadowDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	shadowDesc.MipLevels = 1;
	shadowDesc.MiscFlags = 0;
	shadowDesc.SampleDesc.Count = 1;
	shadowDesc.SampleDesc.Quality = 0;
	shadowDesc.Usage = D3D11_USAGE_DEFAULT;
	ID3D11Texture2D* shadowTexture;
	device->CreateTexture2D(&shadowDesc, 0, &shadowTexture);

	// Create the depth/stencil
	D3D11_DEPTH_STENCIL_VIEW_DESC shadowDSDesc = {};
	shadowDSDesc.Format = DXGI_FORMAT_D32_FLOAT;
	shadowDSDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	shadowDSDesc.Texture2D.MipSlice = 0;
	device->CreateDepthStencilView(shadowTexture, &shadowDSDesc, &shadowDSV);

	//Creating the Shader Resource View for the shadow map
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;
	device->CreateShaderResourceView(shadowTexture, &srvDesc, &shadowSRV);

	shadowTexture->Release();

	// Create the special "comparison" sampler state for shadows
	D3D11_SAMPLER_DESC shadowSampDesc = {};
	shadowSampDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
	shadowSampDesc.ComparisonFunc = D3D11_COMPARISON_LESS;
	shadowSampDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	shadowSampDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	shadowSampDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	shadowSampDesc.BorderColor[0] = 1.0f;
	shadowSampDesc.BorderColor[1] = 1.0f;
	shadowSampDesc.BorderColor[2] = 1.0f;
	shadowSampDesc.BorderColor[3] = 1.0f;
	device->CreateSamplerState(&shadowSampDesc, &shadowSampler);

	// Create a rasterizer state
	D3D11_RASTERIZER_DESC shadowRastDesc = {};
	shadowRastDesc.FillMode = D3D11_FILL_SOLID;
	shadowRastDesc.CullMode = D3D11_CULL_BACK;
	shadowRastDesc.DepthClipEnable = true;
	shadowRastDesc.DepthBias = 1000; // Multiplied by (smallest possible value > 0 in depth buffer)
	shadowRastDesc.DepthBiasClamp = 0.0f;
	shadowRastDesc.SlopeScaledDepthBias = 1.0f;
	device->CreateRasterizerState(&shadowRastDesc, &shadowRasterizer);
}

// --------------------------------------------------------
// Handle resizing DirectX "stuff" to match the new window size.
// For instance, updating our projection matrix's aspect ratio.
// --------------------------------------------------------
void Game::OnResize()
{
	// Handle base-level DX resize stuff
	DXCore::OnResize();

	// Update our projection matrix
	mainCamera->resizeProjectionMatrix(width, height);
}

// --------------------------------------------------------
// Update your game here - user input, move objects, AI, etc.
// --------------------------------------------------------
void Game::Update(float deltaTime, float totalTime)
{
	// Quit if the escape key is pressed
	if (GetAsyncKeyState(VK_ESCAPE))
		Quit();

	if (gameState == 0) {
		if (GetAsyncKeyState(VK_SPACE) & 0x8000) 
		{
			gameState = 1;
		}
	}
	if (gameState == 1) {
		if (GetAsyncKeyState('W') & 0x1)
		{
			if (p1Selection > 0) {
				p1SelectEntities[p1Selection]->SetMaterial(materials[4]);
				p1Selection--;
				p1SelectEntities[p1Selection]->SetMaterial(materials[5]);
			}
		}
		if (GetAsyncKeyState('S') & 0x1)
		{
			if (p1Selection < 6) {
				p1SelectEntities[p1Selection]->SetMaterial(materials[4]);
				p1Selection++;
				p1SelectEntities[p1Selection]->SetMaterial(materials[5]);
			}
		}
		if (GetAsyncKeyState(VK_SPACE) & 0x1)
		{
			ballManager->addBall(gameEntities[6], myVector(p1SelectEntities[p1Selection]->getPosition().x, p1SelectEntities[p1Selection]->getPosition().y, p1SelectEntities[p1Selection]->getPosition().z), 
				myVector(1, 0, 0), 1, .125, false);

		}
		if (GetAsyncKeyState(VK_UP) & 0x1)
		{
			if (p2Selection > 0) {
				p2SelectEntities[p2Selection]->SetMaterial(materials[4]);
				p2Selection--;
				p2SelectEntities[p2Selection]->SetMaterial(materials[5]);
			}
		}
		if (GetAsyncKeyState(VK_DOWN) & 0x1)
		{
			if (p2Selection < 6) {
				p2SelectEntities[p2Selection]->SetMaterial(materials[4]);
				p2Selection++;
				p2SelectEntities[p2Selection]->SetMaterial(materials[5]);
			}
		}
		if (GetAsyncKeyState(VK_RCONTROL) & 0x1)
		{
			ballManager->addBall(gameEntities[6], myVector(p2SelectEntities[p2Selection]->getPosition().x, p2SelectEntities[p2Selection]->getPosition().y, p2SelectEntities[p2Selection]->getPosition().z),
				myVector(-1, 0, 0), 1, .125, false);
		}


		if (DEBUG_MODE) {
			//mainCamera->Update(deltaTime);
		}

		if (GetAsyncKeyState(VK_F1) & 0x8000) {
			DEBUG_MODE = !DEBUG_MODE;
		}

		ballManager->Update(deltaTime);
		
		SortCurrentEntities();
	}
}

// --------------------------------------------------------
// Clear the screen, redraw everything, present to the user
// --------------------------------------------------------
void Game::Draw(float deltaTime, float totalTime)
{
	//Rendering the shadow map, uncomment to have no shadows
	RenderShadowMap();

	// Set depth buffer
	context->OMSetDepthStencilState(0, 0);

	// Background color (Cornflower Blue in this case) for clearing
	const float color[4] = {0.4f, 0.7f, 0.0f, 0.0f};

	// Clear the render target and depth buffer (erases what's on the screen)
	//  - Do this ONCE PER FRAME
	//  - At the beginning of Draw (before drawing *anything*)
	context->ClearRenderTargetView(backBufferRTV, color);
	context->ClearDepthStencilView(
		depthStencilView, 
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f,
		0);

	//Send list of Game Entities to the Renderer class
	if (gameState == 0) {
		renderer->SetGameEntityList(menuEntities);
	}
	if (gameState == 1) {
		renderer->SetGameEntityList(currentGameEntities);

		pixelShader->SetFloat3("CameraPosition", mainCamera->getPosition()); //Setting camera position for specular lighting

		pixelShaderNormal->SetFloat3("CameraPosition", mainCamera->getPosition()); //Setting camera position for specular lighting

		renderer->SetShadowMap(shadowViewMatrix, shadowProjectionMatrix, shadowSRV, shadowSampler);
	}

	renderer->Draw(mainCamera->getViewMatrix(), mainCamera->getProjectionMatrix());

	// Drawing font
	m_spriteBatch->Begin();

	// First text
	const wchar_t* output = L"Score: 0\nBalls: 0";
	SimpleMath::Vector2 origin = m_font->MeasureString(output) / 2.0f;
	m_font->DrawString(m_spriteBatch.get(), output,
		m_p1FontPos, Colors::Red, 0.f, origin);

	// Second Text
	output = L"Score: 0\nBalls: 0";
	origin = m_font->MeasureString(output) / 2.0f;
	m_font->DrawString(m_spriteBatch.get(), output,
		m_p2FontPos, Colors::Blue, 0.f, origin);

	m_spriteBatch->End();

	// Reset the states!
	context->RSSetState(0);
	context->OMSetDepthStencilState(0, 0);
	pixelShader->SetShaderResourceView("ShadowMap", 0);
	// Present the back buffer to the user
	//  - Puts the final frame we're drawing into the window so the user can see it
	//  - Do this exactly ONCE PER FRAME (always at the very end of the frame)
	swapChain->Present(0, 0);
}

void Game::RenderShadowMap()
{
	// Set up targets
	context->OMSetRenderTargets(0, 0, shadowDSV);
	context->ClearDepthStencilView(shadowDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);
	context->RSSetState(shadowRasterizer);

	// Make a viewport to match the render target size
	D3D11_VIEWPORT viewport = {};
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = (float)shadowMapSize;
	viewport.Height = (float)shadowMapSize;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	context->RSSetViewports(1, &viewport);

	// Set up our shadow VS shader
	vertexShaderShadow->SetShader();
	vertexShaderShadow->SetMatrix4x4("view", shadowViewMatrix);
	vertexShaderShadow->SetMatrix4x4("projection", shadowProjectionMatrix);

	// Turn off pixel shader
	context->PSSetShader(0, 0, 0);

	// Loop through entities and draw them
	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	//Shadows on just balls
	//for (unsigned int i = 0; i < ballManager->getBallGameEntities().size(); i++)
	//{
	//	// Grab the data from the first entity's mesh
	//	GameEntity* ge = ballManager->getBallGameEntities()[i];
	//	ID3D11Buffer* vb = ge->getMesh()->GetVertexBuffer();
	//	ID3D11Buffer* ib = ge->getMesh()->GetIndexBuffer();

	//	// Set buffers in the input assembler
	//	context->IASetVertexBuffers(0, 1, &vb, &stride, &offset);
	//	context->IASetIndexBuffer(ib, DXGI_FORMAT_R32_UINT, 0);

	//	vertexShaderShadow->SetMatrix4x4("world", ge->getWorldMatrix());
	//	vertexShaderShadow->CopyAllBufferData();

	//	// Finally do the actual drawing
	//	context->DrawIndexed(ge->getMesh()->GetIndexCount(), 0, 0);
	//}

	//Shadows on every current game entity
	for (unsigned int i = 0; i < currentGameEntities.size(); i++)
	{
		// Grab the data from the first entity's mesh
		GameEntity* ge = currentGameEntities[i];
		ID3D11Buffer* vb = ge->getMesh()->GetVertexBuffer();
		ID3D11Buffer* ib = ge->getMesh()->GetIndexBuffer();

		// Set buffers in the input assembler
		context->IASetVertexBuffers(0, 1, &vb, &stride, &offset);
		context->IASetIndexBuffer(ib, DXGI_FORMAT_R32_UINT, 0);

		vertexShaderShadow->SetMatrix4x4("world", ge->getWorldMatrix());
		vertexShaderShadow->CopyAllBufferData();

		// Finally do the actual drawing
		context->DrawIndexed(ge->getMesh()->GetIndexCount(), 0, 0);
	}

	// Change everything back
	context->OMSetRenderTargets(1, &backBufferRTV, depthStencilView);
	viewport.Width = (float)width;
	viewport.Height = (float)height;
	context->RSSetViewports(1, &viewport);
	context->RSSetState(0);

}


#pragma region Mouse Input

// --------------------------------------------------------
// Helper method for mouse clicking.  We get this information
// from the OS-level messages anyway, so these helpers have
// been created to provide basic mouse input if you want it.
// --------------------------------------------------------
void Game::OnMouseDown(WPARAM buttonState, int x, int y)
{
	// Add any custom code here...

	mouseDown = true;

	// Save the previous mouse position, so we have it for the future
	prevMousePos.x = x;
	prevMousePos.y = y;

	// Caputure the mouse so we keep getting mouse move
	// events even if the mouse leaves the window.  we'll be
	// releasing the capture once a mouse button is released
	SetCapture(hWnd);
}

// --------------------------------------------------------
// Helper method for mouse release
// --------------------------------------------------------
void Game::OnMouseUp(WPARAM buttonState, int x, int y)
{
	// Add any custom code here...

	mouseDown = false;

	// We don't care about the tracking the cursor outside
	// the window anymore (we're not dragging if the mouse is up)
	ReleaseCapture();
}

// --------------------------------------------------------
// Helper method for mouse movement.  We only get this message
// if the mouse is currently over the window, or if we're 
// currently capturing the mouse.
// --------------------------------------------------------
void Game::OnMouseMove(WPARAM buttonState, int x, int y)
{
	// Add any custom code here...

	if (mouseDown) {
		int difX = prevMousePos.x - x;
		int difY = prevMousePos.y - y;

		mainCamera->rotateX(difY);
		mainCamera->rotateY(difX);
		//printf("X: %d Y: %d \n", difX, difY);

		/*if (x  <  width / 2)
			mainCamera->rotateY(true);
		else
			mainCamera->rotateY(false);
		if (y < height / 2)
			mainCamera->rotateX(true);
		else
			mainCamera->rotateX(false);*/


	}
	
	// Save the previous mouse position, so we have it for the future
	prevMousePos.x = x;
	prevMousePos.y = y;
}

// --------------------------------------------------------
// Helper method for mouse wheel scrolling.  
// WheelDelta may be positive or negative, depending 
// on the direction of the scroll
// --------------------------------------------------------
void Game::OnMouseWheel(float wheelDelta, int x, int y)
{
	// Add any custom code here...
}
#pragma endregion