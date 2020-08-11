
#include"StateH.hlsl"

Texture2D g_txBaseColor : register(t0);
Texture2D g_txEmission  : register(t1);
Texture2D g_txMetalness : register(t2);
Texture2D g_txNormal    : register(t3);
Texture2D g_txRoughness : register(t4);
Texture2D g_txOcclusion : register(t5);
TextureCube g_txEnv     : register(t6);

cbuffer PerFrame : register(b0)
{
	
	float4x4 g_mViewProj;
	float3 g_vEyePosW;
};
static const int lightCount = 3;
cbuffer PerLight : register(b1)
{
    struct Light
    {
        float3 g_vLightDir ;
        float atten;
        float3 g_vLightColor;
        float radius;
    } g_Lights[lightCount];
    
};

struct VSIN
{
    float3 Position : POSITION;
    float3 Normal : NORMAL0;
    float2 TexCoord : TEXCOORD;
    float4 Tangent : TANGENT;
};



struct PSIN
{
    float4 PosH : SV_POSITION;
    float2 TexCoord : TEXCOORD;
    float3 Normal : NORMAL;
    float4 TM : TEXCOORD1;
    float4 BM : TEXCOORD2;
    float4 NM : TEXCOORD3;
};


PSIN PBR_VS(VSIN input)
{
	PSIN output = (PSIN)0;
    output.PosH = mul(float4(input.Position,1.f), g_mViewProj);
    float3 bitTangent = normalize(cross(input.Normal, input.Tangent.xyz)*input.Tangent.w);
	float3 posW = input.Position;
    output.Normal = input.Normal;
	output.TM = float4(input.Tangent.x,bitTangent.x,input.Normal.x,posW.x);
    output.BM = float4(input.Tangent.y, bitTangent.y, input.Normal.y, posW.y);
    output.NM = float4(input.Tangent.z, bitTangent.z, input.Normal.z, posW.z);
	output.TexCoord = input.TexCoord;
	return output;
}


float3 CustomDisneyDiffuseTerm(float NdotV, float NdotL, float LdotH, float
roughness, float3 baseColor)
{
    float fd90 = 0.5 + 2 * LdotH * LdotH * roughness;
    // Two schlick fresnel term
    float lightScatter = (1 + (fd90 - 1) * pow(1 - NdotL, 5));
    float viewScatter = (1 + (fd90 - 1) * pow(1 - NdotV, 5));
    return baseColor * (1/pi) * lightScatter * viewScatter;
}
float CustomSmithJointGGXVisibilityTerm(float NdotL, float NdotV, float roughness)
{
// Original formulation:
// lambda_v = (-1 + sqrt(a2 * (1 - NdotL2) / NdotL2 + 1)) * 0.5f;
// lambda_l = (-1 + sqrt(a2 * (1 - NdotV2) / NdotV2 + 1)) * 0.5f;
// G = 1 / (1 + lambda_v + lambda_l);
    float a2 = roughness * roughness;
    float lambdaV = NdotL * (NdotV * (1 - a2) + a2);
    float lambdaL = NdotV * (NdotL * (1 - a2) + a2);
    return 0.5f / (lambdaV + lambdaL + 1e-5f);
}
inline float3 CustomFresnelTerm(float3 c, float cosA)
{
    float t = pow(1 - cosA, 5);
    return c + (1 - c) * t;
}
float CustomGGXTerm(float NdotH, float roughness)
{
    float a2 = roughness * roughness;
    float d = (NdotH * a2 - NdotH) * NdotH + 1.0f;
    return invpi * a2 / (d * d + 1e-7f);
}
float3 CustomFresnelLerp(float3 c0, float3 c1, float cosA)
{
    float t = pow(1 - cosA, 5);
    return lerp(c0, c1, t);
}
static const float3 Ambient = float3(0.01, 0.01, 0.01);
float3 PBR(float3 vEye,float3 vLight,float3 vLightColor,float3 specularColor,float3 diffuseColor,float3 normal,float2 tex,float metalness)
{

    
    float3 reflectV = reflect(-vEye, normal);
    
    float3 floatDir = normalize(vLight + vEye);
    float nv = saturate(dot(normal, vEye));
    float nl = saturate(dot(normal, vLight));
    float nh = saturate(dot(normal, floatDir));
    float lv = saturate(dot(vLight, vEye));
    float lh = saturate(dot(vLight, floatDir));
    float roughness = g_txRoughness.Sample(g_samLinearMIRROR, tex);

   
  
    
   
    float specuScale = 0.5;
    //float3 specularColor = saturate(0.08 * specuScale * (1 - metalness) + baseColor * metalness);

    float V = CustomSmithJointGGXVisibilityTerm(nl, nv, roughness);
    float D = CustomGGXTerm(nh, roughness * roughness);
    float3 F = CustomFresnelTerm(specularColor,  lh);
    float3 specularTerm = F * V * D;
    float3 diffuseTerm = CustomDisneyDiffuseTerm(nv, nl, lh, roughness, diffuseColor);
    
  
    float perceptualRoughness = roughness * (1.7 - 0.7 * roughness);
    float mip = perceptualRoughness * 25;
    float3 reflectC = g_txEnv.SampleLevel(g_samLinearMIRROR, reflectV, mip);
    float oneMinusReflectivity = 1 - max(max(specularColor.r, specularColor.g), specularColor.b);
    float grazingTerm = saturate((1 - roughness) + (1 - oneMinusReflectivity));
    float surfaceReduction = 1.0 / (roughness * roughness + 1.0);
    float3 indirectSpecular = surfaceReduction * reflectC * CustomFresnelLerp(pow(specularColor,4), grazingTerm * 1.5, nv);
    
    
    //blinn-phone
    //float alpha = (1 - roughness) * 255;
    //float3 specTerm = (alpha + 2) / 8 * pow(nh, alpha) * CustomFresnelTerm(specularColor, lh);

    //return pow(nh, alpha); //;(diffuseColor + specTerm) * vLightColor * nl + indirectSpecular;
    float kSpecular = specularColor;
    float kDiffuse = (1 - specularColor)*(1 - metalness);
    return pi * (kDiffuse * diffuseTerm + kSpecular * specularTerm) * vLightColor * nl + indirectSpecular;
    
    // pi * (kDiffuse * diffuseTerm + kSpecular * specularTerm) * vLightColor * nl + indirectSpecular;

}

float4 PBR_PS(PSIN input):SV_TARGET
{
    float3x3 TBN = float3x3(input.TM.xyz, input.BM.xyz, input.NM.xyz);
    float3 posW = float3(input.TM.w, input.BM.w, input.NM.w);
	
    float3 normal = input.Normal;
    //float3 normal = g_txNormal.Sample(g_samLinearMIRROR, input.TexCoord);
    //normal = normal * 0.5 + 0.5;
    //normal = normalize(mul(normal, TBN));
    float Occlusion = g_txOcclusion.Sample(g_samLinearMIRROR, input.TexCoord).r;
    float3 emissionC = g_txEmission.Sample(g_samLinearMIRROR, input.TexCoord);
   
     
    float metalness = g_txMetalness.Sample(g_samLinearMIRROR, input.TexCoord);
    float3 baseColor = g_txBaseColor.Sample(g_samLinearMIRROR, input.TexCoord);
    float3 diffuseColor = baseColor * (1 - metalness);
    float3 specularColor = lerp(0.04, baseColor, metalness);
    
    
    
    float3 vEye = normalize(g_vEyePosW - posW);
    float3 PBRC = 0;
    [unroll]
    for (int i = 0; i < lightCount; i++)
    {
        PBRC += PBR(vEye, g_Lights[i].g_vLightDir, g_Lights[i].g_vLightColor, specularColor, diffuseColor, normal, input.TexCoord, metalness);
    }
   
   // PBRC = PBR(vEye, g_Lights[0].g_vLightDir, g_Lights[0].g_vLightColor, normal, input.TexCoord);
    float3 all = emissionC+PBRC;
    //float3 emissionC = g_txEmission.Sample(g_samLinearMIRROR, input.TexCoord);
    //float metalness = g_txMetalness.Sample(g_samLinearMIRROR, input.TexCoord).r;
    //float3 baseColor = g_txBaseColor.Sample(g_samLinearMIRROR, input.TexCoord) * (1 - metalness);
    //float roughness = g_txRoughness.Sample(g_samLinearMIRROR, input.TexCoord);
    
    
    
    return float4(all, 1.f);

}


VertexShader ComPBRVS = CompileShader(vs_5_0, PBR_VS());
PixelShader ComPBRPS = CompileShader(ps_5_0, PBR_PS());
technique11 PBREffect
{
    pass P0
    {
        SetVertexShader(ComPBRVS);
        SetPixelShader(ComPBRPS);

        SetDepthStencilState(EnableDepth, 0);
        SetBlendState(NoBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetRasterizerState(g_rasterizerState[g_rasterizerStateCount]);

    }
}