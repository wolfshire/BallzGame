#include "Game.h"
#include "Vertex.h"
#include "WICTextureLoader.h"

// For the DirectX Math library
using namespace DirectX;

#define PI 3.14159265359

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
		"DirectX Game",	   // Text for the window's title bar
		1280,			   // Width of the window's client area
		720,			   // Height of the window's client area
		true)			   // Show extra stats (fps) in title bar?
{

	vertexShader = 0;
	pixelShader = 0;

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

	delete renderer;
	delete mainCamera;

	sampler1->Release();
	gamefield->Release();
	bricks->Release();

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
}

// --------------------------------------------------------
// Called once per program, after DirectX and the window
// are initialized but before the game loop.
// --------------------------------------------------------
void Game::Init()
{
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

	dirLight1.AmbientColor = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	dirLight1.DiffuseColor = XMFLOAT4(1, 0, 0, 1);
	dirLight1.Direction = XMFLOAT3(0, 0, -1);

	dirLight2.AmbientColor = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	dirLight2.DiffuseColor = XMFLOAT4(0, 0, 1, 1);
	dirLight2.Direction = XMFLOAT3(-1, -1, 0);

	pixelShader->SetData(
		"DirLightOne",				//The name of the variable in the pixel shader
		&dirLight1,					//The address of the data to copy
		sizeof(DirectionalLight));  //Size of data to copy

	pixelShader->SetData(
		"DirLightTwo",				//The name of the variable in the pixel shader
		&dirLight2,					//The address of the data to copy
		sizeof(DirectionalLight));  //Size of data to copy

	CreateMenu();

	gameState = 0;
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
		L"Debug/Textures/field.jpg",
		0,
		&gamefield
	);

	check = CreateWICTextureFromFile(
		device,
		context,
		L"Debug/Textures/bricks.jpg",
		0,
		&bricks
	);

	check = CreateWICTextureFromFile(
		device,
		context,
		L"Debug/Textures/mainmenu.png",
		0,
		&menu
		);

	D3D11_SAMPLER_DESC samplerDesc = {};
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.MaxAnisotropy = 16;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	check = device->CreateSamplerState(&samplerDesc, &sampler1);

	//Creating Meshes
	meshes.push_back(new Mesh("Models/cube.obj", device));									//meshes[0] - > Cube Model
	meshes.push_back(new Mesh("Models/sphere.obj", device));								//meshes[1] - > Sphere Model

	//Creating materials
	materials.push_back(new Material(vertexShader, pixelShader, gamefield, sampler1));		// materials[0] -> basic material, grassy field texture
	materials.push_back(new Material(vertexShader, pixelShader, bricks, sampler1));			// materials[1] -> basic material, brick texture
	materials.push_back(new Material(vertexShader, pixelShader, menu, sampler1));
	
	//Setting material Color -Debug
	//materials[2]->SetSurfaceColor(XMFLOAT4(1, 1, 1, 1));

	//Creating GameEntities
	gameEntities.push_back(new GameEntity(meshes[0], materials[0]));						// gameEntities[0] -> Game Field (grass)
	gameEntities.push_back(new GameEntity(meshes[1], materials[1]));						// gameEntities[1] -> Ball (brick)

	//Creating MenuEntities
	menuEntities.push_back(new GameEntity(meshes[0], materials[2]));						// menuEntities[0] -> Menu

	//Setting Scales
	/*for(int i = 0; i < gameEntities.size(); i++)
		gameEntities[i]->SetScale(2, 2, 2);*/

	/*currentGameEntities.push_back(gameEntities[0]);*/

	
}

void Game::CreateMenu() {
	menuEntities[0]->Rotate(0, 0, PI / 2.0);
	menuEntities[0]->SetScale(4, 7, 1);
}

void Game::CreateGameField() {

	//creating the floor
	gameEntities[0]->Rotate(0, 0, PI/2.0);
	gameEntities[0]->SetScale(3.2, 5.5, 1);
	currentGameEntities.push_back(gameEntities[0]);

	//Adding a ball
	gameEntities[1]->SetScale(.5f, .5f, .5f);
	gameEntities[1]->SetTranslation(0, 0, -1);
	currentGameEntities.push_back(gameEntities[1]);
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
		// Move the triangle a little
		float sinTime = (sin(totalTime) + 2.0f) / 10.0f;
		float cosTime = cos(totalTime);

		if (GetAsyncKeyState(VK_RIGHT) & 0x8000) //Right Key
		{
			gameEntities[1]->Move(deltaTime, 0, 0);
		}
		if (GetAsyncKeyState(VK_LEFT) & 0x8000) //Right Key
		{
			gameEntities[1]->Move(-deltaTime, 0, 0);
		}
		if (GetAsyncKeyState(VK_UP) & 0x8000) //Right Key
		{
			gameEntities[1]->Move(0, deltaTime, 0);
		}
		if (GetAsyncKeyState(VK_DOWN) & 0x8000) //Right Key
		{
			gameEntities[1]->Move(0, -deltaTime, 0);
		}
	}
	mainCamera->Update(deltaTime);
}

// --------------------------------------------------------
// Clear the screen, redraw everything, present to the user
// --------------------------------------------------------
void Game::Draw(float deltaTime, float totalTime)
{
	// Background color (Cornflower Blue in this case) for clearing
	const float color[4] = {0.4f, 0.6f, 0.75f, 0.0f};

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
	}

	renderer->Draw(mainCamera->getViewMatrix(), mainCamera->getProjectionMatrix());

	// Present the back buffer to the user
	//  - Puts the final frame we're drawing into the window so the user can see it
	//  - Do this exactly ONCE PER FRAME (always at the very end of the frame)
	swapChain->Present(0, 0);
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