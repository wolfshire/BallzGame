
// Constant Buffer
// - Allows us to define a buffer of individual variables 
//    which will (eventually) hold data from our C++ code
// - All non-pipeline variables that get their values from 
//    our C++ code must be defined inside a Constant Buffer
// - The name of the cbuffer itself is unimportant
cbuffer externalData : register(b0)
{
	matrix world;
	matrix view;
	matrix projection;

	matrix shadowView;
	matrix shadowProjection;
	matrix shadowView2;
	matrix shadowProjection2;
	matrix shadowView3;
	matrix shadowProjection3;
	matrix shadowView4;
	matrix shadowProjection4;
};

// Struct representing a single vertex worth of data
// - This should match the vertex definition in our C++ code
// - By "match", I mean the size, order and number of members
// - The name of the struct itself is unimportant, but should be descriptive
// - Each variable must have a semantic, which defines its usage
struct VertexShaderInput
{
	// Data type
	//  |
	//  |   Name          Semantic
	//  |    |                |
	//  v    v                v
	float3 position		: POSITION;     // XYZ position
	float2 uv			: TEXCOORD;
	float3 normal		: NORMAL;
	float3 tangent		: TANGENT;
	
};

// Struct representing the data we're sending down the pipeline
// - Should match our pixel shader's input (hence the name: Vertex to Pixel)
// - At a minimum, we need a piece of data defined tagged as SV_POSITION
// - The name of the struct itself is unimportant, but should be descriptive
// - Each variable must have a semantic, which defines its usage
struct VertexToPixel
{
	// Data type
	//  |
	//  |   Name          Semantic
	//  |    |                |
	//  v    v                v
	float4 position			: SV_POSITION;	// XYZW position (System Value Position)
	float3 normal			: NORMAL;
	float3 tangent			: TANGENT;
	float3 worldPos			: POSITION;
	float2 uv				: TEXCOORD;
	float4 posForShadow		: TEXCOORD1;
	float4 posForShadow2	: TEXCOORD2;
	float4 posForShadow3	: TEXCOORD3;
	float4 posForShadow4	: TEXCOORD4;
};

// --------------------------------------------------------
// The entry point (main method) for our vertex shader
// 
// - Input is exactly one vertex worth of data (defined by a struct)
// - Output is a single struct of data to pass down the pipeline
// - Named "main" because that's the default the shader compiler looks for
// --------------------------------------------------------
VertexToPixel main(VertexShaderInput input)
{
	// Set up output struct
	VertexToPixel output;

	// The vertex's position (input.position) must be converted to world space,
	// then camera space (relative to our 3D camera), then to proper homogenous 
	// screen-space coordinates.  This is taken care of by our world, view and
	// projection matrices.  
	//
	// First we multiply them together to get a single matrix which represents
	// all of those transformations (world to view to projection space)
	matrix worldViewProj = mul(mul(world, view), projection);

	// Then we convert our 3-component position vector to a 4-component vector
	// and multiply it by our final 4x4 matrix.
	//
	// The result is essentially the position (XY) of the vertex on our 2D 
	// screen and the distance (Z) from the camera (the "depth" of the pixel)
	output.position = mul(float4(input.position, 1.0f), worldViewProj);

	//Calculate the world position
	output.worldPos = mul(float4(input.position, 1), world).xyz;

	// Get the normal to the pixel shader
	// (This is ASSUMING UNIFORM SCALING! - Otherwise you need
	//  inverse transpose of the world matrix)
	output.normal = mul(input.normal, (float3x3)world);
	output.normal = normalize(output.normal);

	//Getting the tangent in world space
	output.tangent = mul(input.tangent, (float3x3)world);

	output.uv = input.uv;

	// Do shadow map calc
	matrix shadowWVP = mul(mul(world, shadowView), shadowProjection);
	output.posForShadow = mul(float4(input.position, 1), shadowWVP);

	shadowWVP = mul(mul(world, shadowView2), shadowProjection2);
	output.posForShadow2 = mul(float4(input.position, 1), shadowWVP);

	shadowWVP = mul(mul(world, shadowView3), shadowProjection3);
	output.posForShadow3 = mul(float4(input.position, 1), shadowWVP);

	shadowWVP = mul(mul(world, shadowView4), shadowProjection4);
	output.posForShadow4 = mul(float4(input.position, 1), shadowWVP);


	// Whatever we return will make its way through the pipeline to the
	// next programmable stage we're using (the pixel shader for now)
	return output;
}