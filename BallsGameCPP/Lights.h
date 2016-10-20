#pragma once

#include <DirectXMath.h>

// Directional Light Struct
struct DirectionalLight
{

	DirectX::XMFLOAT4 AmbientColor;
	DirectX::XMFLOAT4 DiffuseColor;
	DirectX::XMFLOAT3 Direction;

};