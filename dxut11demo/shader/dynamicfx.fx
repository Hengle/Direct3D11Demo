#include"PSH.hlsl"

iBaseLight g_abstractAmbientLighting;
iBaseLight g_abstractDirectionalLighting;
iBaseLight g_abstractEnvironmentLighting;
iBaseMaterial g_abstractMaterial;
    

//packoffset可以指定数据起始位置
cbuffer cbPerObject : register(b0)
{
	float4x4 g_mWVP : packoffset(c0);
	float4x4 g_mWorld : packoffset(c4);
}


struct VS_INPUT
{
	float4 vPosition : POSITION;
	float3 vNormal : NORMAL;
	float2 vTexcoord : TEXCOORD0;
};

struct VS_OUTPUT
{
	float4 vPosition : SV_POSITION;
	float3 vNormal : NORMAL;
	float2 vTexcoord0 : TEXCOORD0;
	float4 vMatrix : TEXCOORD1; // DEBUG
};

float3 UnormalToFloat(float3 inNormal)
{
	inNormal *= 2;
	return inNormal >= 1.0f ? (inNormal - 2.0f) : inNormal;
}

VS_OUTPUT VSMain(VS_INPUT Input)
{
	VS_OUTPUT output;
	float3 tmpNormal;
	output.vPosition = mul(Input.vPosition, g_mWVP);
	tmpNormal = UnormalToFloat(Input.vNormal);

	output.vNormal = mul(tmpNormal, (float3x3) g_mWorld);
    
	output.vTexcoord0 = Input.vTexcoord;

	output.vMatrix = (float4) g_mWorld[0]; // DEBUG
	return output;
}

struct PS_INPUT
{
    float4 vPosition    : SV_POSITION;
    float3 vNormal      : NORMAL;
    float2 vTexcoord    : TEXCOORD0;
    float4 vMatrix      : TEXCOORD1;    

};


float4 dynamicPs(iBaseLight ambientLighting,
		iBaseLight directionLighting,
		iBaseLight environmentLighting,
		iBaseMaterial material,
		PS_INPUT input): SV_Target
{

    float3 ambient = material.GetAmbientColor(input.vTexcoord) * ambientLighting.IlluminateAmbient(input.vNormal);
    float3 diffuse = material.GetDiffuseColor(input.vTexcoord) * directionLighting.IlluminateDiffuse(input.vNormal);
    float3 specular = directionLighting.IlluminateSpecular(input.vNormal, material.GetSpecularPower());
    specular += environmentLighting.IlluminateSpecular(input.vNormal, material.GetSpecularPower());

	float3 Light = saturate(ambient + diffuse + specular);
	return float4(Light,1.f);
}

technique11 dynamicEffect
{
    pass P0
    {
        SetRasterizerState(g_rasterizerState[g_fillMode]);
        SetVertexShader(CompileShader(vs_5_0,
                                      VSMain()));
        SetPixelShader(BindInterfaces(CompileShader(ps_5_0, dynamicPs()),
                                        g_abstractAmbientLighting,
                                      g_abstractDirectionalLighting,
                                      g_abstractEnvironmentLighting,
                                       g_abstractMaterial));
        SetDepthStencilState(EnableDepth, 0);
        SetBlendState(NoBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);

    }


}