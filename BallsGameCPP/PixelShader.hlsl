
struct DirectionalLight
{
	float4 AmbientColor;
	float4 DiffuseColor;
	float3 Direction;
};

cbuffer externalData : register(b0)
{
	DirectionalLight DirLightOne;
	DirectionalLight DirLightTwo;

	float4 SurfaceColor;
};

Texture2D Texture			: register(t0);
SamplerState basicSampler	: register(s0);

// Struct representing the data we expect to receive from earlier pipeline stages
// - Should match the output of our corresponding vertex shader
// - The name of the struct itself is unimportant
// - The variable names don't have to match other shaders (just the semantics)
// - Each variable must have a semantic, which defines its usage
struct VertexToPixel
{
	// Data type
	//  |
	//  |   Name          Semantic
	//  |    |                |
	//  v    v                v
	float4 position		: SV_POSITION;
	float3 normal		: NORMAL;
	float2 uv			: TEXTCOORD;
};

// --------------------------------------------------------
// Helper method to calculate Directional Light color
// --------------------------------------------------------
float3 CalculateDirectionalLightColor(float3 normal, DirectionalLight light, float4 textureColor)
{
	float3 DirToLight = -normalize(light.Direction);					//Get the direction from the pixel to the Light
	float LightAmount = saturate(dot(normal, DirToLight));					//Get the amount of light for that pixel

	float3 finalColor =
		(light.DiffuseColor * LightAmount) +
		(light.AmbientColor);

	return finalColor * SurfaceColor * textureColor;
}

// --------------------------------------------------------
// The entry point (main method) for our pixel shader
// 
// - Input is the data coming down the pipeline (defined by the struct)
// - Output is a single color (float4)
// - Has a special semantic (SV_TARGET), which means 
//    "put the output of this into the current render target"
// - Named "main" because that's the default the shader compiler looks for
// --------------------------------------------------------
float4 main(VertexToPixel input) : SV_TARGET
{
	//Normalize the normal
	input.normal = normalize(input.normal);

	//Get texture color
	float4 textureColor = Texture.Sample(basicSampler, input.uv);

	//Calculate final Colors
	float3 DirLightOneColor = CalculateDirectionalLightColor(input.normal, DirLightOne, textureColor);
	float3 DirLightTwoColor = CalculateDirectionalLightColor(input.normal, DirLightTwo, textureColor);

	float3 finalColor = DirLightOneColor + DirLightTwoColor;

	return float4(finalColor, 1);
}
