
#include"stateH.hlsl"



TextureCube g_txEnvCube : register(t0);

Texture2D g_txShadowMap : register(t1);

TextureCube g_txDynamicBox : register(t2);

cbuffer PerFrame : register(b0)
{
	
	float4x4 g_mViewProj;
	float3 g_vEyePosW;
};

cbuffer PerObj : register(b1)
{
    
    float3 g_vColor : packoffset(c0);
    float g_fScale : packoffset(c0.w);
    float4x4 g_mWorld : packoffset(c1);
}
static const uint g_iNumLights = 1;

cbuffer PerLight : register(b2)
{
    struct LightDataStruct
    {
        matrix m_mLightViewProj;
        float4 m_vLightPos;
        float4 m_vLightDir;
        float4 m_vLightColor;
        float4 m_vFalloffs; // x = dist end, y = dist range, z = cos angle end, w = cos range
    } g_LightData[g_iNumLights] : packoffset(c0);
};

struct VSIN
{
	float3 pos : POSITION;
    float3 normal : NORMAL;
	float2 tex : TEXCOORD;

};

struct PSIN
{
    float4 posH : SV_POSITION;
    float3 normalW : NORMAL;
    float3 posW : TEXCOORD0;
};

struct EnvPSIN
{
	float4 posH : SV_POSITION;
	float3 posL : POSITION;
};


EnvPSIN EnvVS(VSIN input)
{
    EnvPSIN output = (EnvPSIN)0;
    
    float4 inpos = float4(input.pos, 1.f);
 
    output.posL = input.normal;
	
	
    float4 PosW =mul(inpos, g_mWorld);
	PosW.xyz += g_vEyePosW;
	output.posH = mul( PosW, g_mViewProj);
	output.posH = output.posH.xyww;
 
    return output;
}



float4 EnvPS( EnvPSIN input) : SV_Target
{
    // Calculate lighting assuming light color is <1,1,1,1>
    return g_txEnvCube.Sample(g_samLinear, input.posL);
    
}


PSIN SceneVS(VSIN input)
{
    PSIN output = (PSIN) 0;
 
    output.normalW = mul(input.normal,g_mWorld);
	
	
    float4 PosW = mul(float4(input.pos * g_fScale, 1.0f), g_mWorld);
    output.posW = PosW.xyz;
    output.posH = mul(PosW, g_mViewProj);
    
    return output;
}


const float3 vlight = (1,1,1);
static const float3 AmbientColor = (0.05,0.05,0.05);
uint g_SceneStateCount = 0;

float3 CaluSpotLight(LightDataStruct Light,float3 posW,float3 normal)
{
    float3 LightToPixel = posW - Light.m_vLightPos.xyz;
    float disFalloff = saturate((Light.m_vFalloffs.x - length(LightToPixel)) / Light.m_vFalloffs.y);
    float3 lightToPixelNormalize = normalize(LightToPixel);
    
    float cosAngle = dot(lightToPixelNormalize, Light.m_vLightDir.xyz);
    float AngleFalloff = saturate((cosAngle - Light.m_vFalloffs.z) / Light.m_vFalloffs.w);
    float lambert = saturate(dot(normal,-lightToPixelNormalize));
    return Light.m_vLightColor * AngleFalloff * disFalloff * lambert;
}
float CaluateShadowFactor(float3 posW,float4x4 vp)
{
    float4 LightPosH = mul(float4(posW, 1.f), vp);
    LightPosH.xyz /= LightPosH.w;
    float depth = LightPosH.z - 0.0005f;
    float2 tex = LightPosH.xy * 0.5 + 0.5;
    tex.y = 1.f - tex.y;
    return g_txShadowMap.SampleCmpLevelZero(gShadowSample, tex, depth).r;
    //float z = g_txShadowMap.Sample(g_samLinear, tex);
    //return z >= depth?1:0;
}

float4 ScenePS(PSIN input) : SV_Target
{
    float3 diffuseL = float3(0, 0, 0);
    float sf = CaluateShadowFactor(input.posW, g_LightData[0].m_mLightViewProj);
    
    [unroll]
    for (int i = 0; i < g_iNumLights;i++)
    {
        diffuseL += CaluSpotLight(g_LightData[i], input.posW, normalize(input.normalW)) *g_vColor*sf;

    }
    
    return float4(diffuseL + AmbientColor * g_vColor, 1.f);
}
float4 dynamicBoxPS(PSIN input) : SV_Target
{
    float3 EyeToPos = normalize(input.posW - g_vEyePosW);
    
    float3 vRef = reflect(EyeToPos, vRef);
    float3 diffuseL = float3(0, 0, 0);
    float3 RefleltC = g_txDynamicBox.Sample(g_samLinear, input.normalW) * dot(-EyeToPos,input.normalW);
    float sf = CaluateShadowFactor(input.posW, g_LightData[0].m_mLightViewProj);
    
    [unroll]
    for (int i = 0; i < g_iNumLights; i++)
    {
        diffuseL += CaluSpotLight(g_LightData[i], input.posW, normalize(input.normalW))  * sf;
    }
    float3 ret = saturate(diffuseL + AmbientColor * g_vColor + RefleltC);
    return float4(ret, 1.f);
}
technique11 SkyboxEffect
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, EnvVS()));
        SetPixelShader(CompileShader(ps_5_0, EnvPS()));

        SetDepthStencilState(EnableDepth, 0);
        SetBlendState(NoBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetRasterizerState(g_rasterizerState[g_rasterizerStateCount]);

    }
}
VertexShader ComSceneVS = CompileShader(vs_5_0, SceneVS());
PixelShader ComScenePS = CompileShader(ps_5_0, ScenePS());
PixelShader ComDynamicBoxPS = CompileShader(ps_5_0, dynamicBoxPS());

technique11 SceneEffect
{
    pass P0
    {
        SetVertexShader(ComSceneVS);
        SetPixelShader(ComScenePS);

        SetDepthStencilState(EnableDepth, 0);
        SetBlendState(NoBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetRasterizerState(g_rasterizerState[g_SceneStateCount]);

    }
}




