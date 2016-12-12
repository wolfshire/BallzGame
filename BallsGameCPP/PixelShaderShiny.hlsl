
struct DirectionalLight
{
	float4 AmbientColor;
	float4 DiffuseColor;
	float3 Direction;
};

struct PointLight
{
	float4 Color;
	float3 Position;
};

cbuffer externalData : register(b0)
{
	DirectionalLight DirLightOne;

	PointLight PointLightOne;
	PointLight PointLightTwo;
	PointLight PointLightThree;
	PointLight PointLightFour;

	float4 SurfaceColor;
	float3 CameraPosition;
};

Texture2D Texture			: register(t0);
Texture2D ShadowMap			: register(t1);
Texture2D ShadowMap2		: register(t2);
Texture2D ShadowMap3		: register(t3);
Texture2D ShadowMap4		: register(t4);
TextureCube Sky				: register(t5);
SamplerState basicSampler	: register(s0);
SamplerComparisonState ShadowSampler : register(s1);

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
	float4 position			: SV_POSITION;
	float3 normal			: NORMAL;
	float3 worldPos			: POSITION;
	float2 uv				: TEXTCOORD;
	float4 posForShadow		: TEXCOORD1;
	float4 posForShadow2	: TEXCOORD2;
	float4 posForShadow3	: TEXCOORD3;
	float4 posForShadow4	: TEXCOORD4;
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

	//Calculate final Directional Colors
	float3 DirLightOneColor = CalculateDirectionalLightColor(input.normal, DirLightOne, textureColor);

	//Calculate Point light colors and specular lighting 
	//Point Light One
	float3 dirToPointLightOne = normalize(PointLightOne.Position - input.worldPos);
	float pointLightAmount = saturate(dot(input.normal, dirToPointLightOne));
	float3 PointLightOneColor = PointLightOne.Color * pointLightAmount * SurfaceColor * textureColor * 10;
	float3 toCamera = normalize(CameraPosition - input.worldPos);	//----------------
	float3 refl = reflect(-dirToPointLightOne, input.normal);		//	  SPECULAR
	float specPLOne = pow(max(dot(refl, toCamera), 0), 168);		//----------------

	//Point Light Two
	float3 dirToPointLightTwo = normalize(PointLightTwo.Position - input.worldPos);
	pointLightAmount = saturate(dot(input.normal, dirToPointLightTwo));
	float3 PointLightTwoColor = PointLightTwo.Color * pointLightAmount * SurfaceColor * textureColor * 10;
	refl = reflect(-dirToPointLightTwo, input.normal);
	float specPLTwo = pow(max(dot(refl, toCamera), 0), 168);

	//Point Light Three
	float3 dirToPointLightThree = normalize(PointLightThree.Position - input.worldPos);
	pointLightAmount = saturate(dot(input.normal, dirToPointLightThree));
	float3 PointLightThreeColor = PointLightThree.Color * pointLightAmount * SurfaceColor * textureColor * 10;
	refl = reflect(-dirToPointLightThree, input.normal);
	float specPLThree = pow(max(dot(refl, toCamera), 0), 168);

	//Point Light Four
	float3 dirToPointLightFour = normalize(PointLightFour.Position - input.worldPos);
	pointLightAmount = saturate(dot(input.normal, dirToPointLightFour));
	float3 PointLightFourColor = PointLightFour.Color * pointLightAmount * SurfaceColor * textureColor * 10;
	refl = reflect(-dirToPointLightFour, input.normal);
	float specPLFour = pow(max(dot(refl, toCamera), 0), 168);

	// Shadow map calculation

	//// Light One
	//// Figure out this pixel's UV in the SHADOW MAP
	//float2 shadowUV = input.posForShadow.xy / input.posForShadow.w * 0.5f + 0.5f;
	//shadowUV.y = 1.0f - shadowUV.y; // Flip the Y since UV coords and screen coords are different

	//								// Calculate this pixel's actual depth from the light
	//float depthFromLight = input.posForShadow.z / input.posForShadow.w;

	//// Sample the shadow map
	//float shadowAmount = ShadowMap.SampleCmpLevelZero(ShadowSampler, shadowUV, depthFromLight);

	//// Light Two
	//// Figure out this pixel's UV in the SHADOW MAP
	//shadowUV = input.posForShadow2.xy / input.posForShadow2.w * 0.5f + 0.5f;
	//shadowUV.y = 1.0f - shadowUV.y; // Flip the Y since UV coords and screen coords are different

	//								// Calculate this pixel's actual depth from the light
	//depthFromLight = input.posForShadow2.z / input.posForShadow2.w;

	//// Sample the shadow map
	//float shadowAmount2 = ShadowMap2.SampleCmpLevelZero(ShadowSampler, shadowUV, depthFromLight);

	//// Light Three
	//// Figure out this pixel's UV in the SHADOW MAP
	//shadowUV = input.posForShadow3.xy / input.posForShadow3.w * 0.5f + 0.5f;
	//shadowUV.y = 1.0f - shadowUV.y; // Flip the Y since UV coords and screen coords are different

	//								// Calculate this pixel's actual depth from the light
	//depthFromLight = input.posForShadow3.z / input.posForShadow3.w;

	//// Sample the shadow map
	//float shadowAmount3 = ShadowMap3.SampleCmpLevelZero(ShadowSampler, shadowUV, depthFromLight);

	//// Light Four
	//// Figure out this pixel's UV in the SHADOW MAP
	//shadowUV = input.posForShadow4.xy / input.posForShadow4.w * 0.5f + 0.5f;
	//shadowUV.y = 1.0f - shadowUV.y; // Flip the Y since UV coords and screen coords are different

	//								// Calculate this pixel's actual depth from the light
	//depthFromLight = input.posForShadow4.z / input.posForShadow4.w;

	//// Sample the shadow map
	//float shadowAmount4 = ShadowMap4.SampleCmpLevelZero(ShadowSampler, shadowUV, depthFromLight);

	// Calculate reflection to the sky and sample
	float4 skyColor = Sky.Sample(basicSampler, reflect(-toCamera, input.normal));

	float3 finalColor = DirLightOneColor +
						PointLightOneColor + //* shadowAmount +
						PointLightTwoColor +//* shadowAmount2 +
						PointLightThreeColor +//* shadowAmount3 +
						PointLightFourColor +//* shadowAmount4 +
						specPLOne +
						specPLTwo +
						specPLThree +
						specPLFour;

	float4 i = float4(finalColor, 1);

	return lerp(skyColor, i, 0.35f);
}
