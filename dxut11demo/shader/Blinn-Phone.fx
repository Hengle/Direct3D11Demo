Texture2D txDiffuse : register(t0);
SamplerState samLinear : register(s0);


cbuffer cbNeverChange : register(b1)
{
	float3 g_lightDir;
};


cbuffer cbChangeEverFrame : register(b0)
{
	matrix WorldViewProj;
	matrix World;
	float4 vMeshColor;
};


struct VS_INPUT
{
	float4 Pos : POSITION;
	float3 Normal : NORMAL;
	float2 Tex : TEXCOORD;
};

struct PS_INPUT
{
	float4 Pos : SV_POSITION;
	float3 Normal : NORMAL;
	float2 Tex : TEXCOORD0;
	
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT VS(VS_INPUT input)
{
	PS_INPUT output = (PS_INPUT) 0;
	output.Pos = mul(input.Pos, WorldViewProj);
	output.Tex = input.Tex;
	output.Normal = mul(float4(input.Normal,0), transpose(World));
	return output;
}




//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(PS_INPUT input) : SV_Target
{
	float lightColor = dot(input.Normal, g_lightDir);
	return lightColor * txDiffuse.Sample(samLinear, input.Tex) * vMeshColor;
}


