#include"dynamicLight.h"

#include"dynamicMaterial.hlsl"
//--------------------------------------------------------------------------------------
// Constant Buffers
//--------------------------------------------------------------------------------------

cbuffer cbPerFrame : register(b0)
{
	cAmbientLight g_ambientLight;
	cHemiAmbientLight g_hemiAmbientLight;
	cDirectionalLight g_directionalLight;
	cEnvironmentLight g_environmentLight;
	float4 g_vEyeDir;
}

cbuffer cbPerPrimitive:register(b1)
{
	cPlasticMaterial				g_plasticMaterial;
	cPlasticTexturedMaterial		g_plasticTexturedMaterial;
	cRoughMaterial                g_roughMaterial;
	cRoughTexturedMaterial        g_roughTexturedMaterial;
}
//--------------------------------------------------------------------------------------
// Textures and Samplers
//--------------------------------------------------------------------------------------


Texture2D      g_txDiffuse : register( t0 );
Texture2D      g_txNormalMap : register( t1 );
TextureCube    g_txEnvironmentMap : register( t2 );

SamplerState   g_samLinear : register( s0 )
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = WRAP;
    AddressV = WRAP;
    AddressW = WRAP;
};

DepthStencilState EnableDepth
{
    DepthEnable = TRUE;
    DepthWriteMask = ALL;
    DepthFunc = LESS_EQUAL;
};

BlendState NoBlending
{
    AlphaToCoverageEnable = FALSE;
    BlendEnable[0] = FALSE;
};

//--------------------------------------------------------------------------------------
// Rasterization State
//--------------------------------------------------------------------------------------
uint g_fillMode = 0;
   
RasterizerState g_rasterizerState[2]
{
{
    FillMode = SOLID;
    MultisampleEnable = true;
},
{
    FillMode = WIREFRAME;
    MultisampleEnable = true;
}
};

//--------------------------------------------------------------------------------------
// Lighting Class Methods
//--------------------------------------------------------------------------------------



float3 cAmbientLight::IlluminateAmbient(float3 vNormal)
{
	return m_vLightColor*m_bEnable;
}

float3 cHemiAmbientLight::IlluminateAmbient(float3 vNormal)
{
	float theta = (dot(m_vDirUp,vNormal)+1.f)/2,f;
	return lerp(m_vGroundColor.xyz, m_vLightColor, theta) * m_bEnable;
}

float3 cDirectionalLight::IlluminateDiffuse( float3 vNormal ) 
{
   float lambert = saturate(dot( vNormal, m_vLightDir.xyz ));
   return ((float3)lambert * m_vLightColor * m_bEnable);
}

float3 cDirectionalLight::IlluminateSpecular( float3 vNormal,int specularPower ) 
{
	float3 H =normalize(normalize(-g_vEyeDir.xyz) + m_vLightDir.xyz);
	float Blin = saturate( dot(H,normalize(vNormal)));

	return (float3)pow(Blin,specularPower) * m_vLightColor*m_bEnable;
}

// Omni Light Class
float3 cOmniLight::IlluminateDiffuse( float3 vNormal ) 
{
   return (float3)0.0f; // TO DO!
}


float3 cEnvironmentLight::IlluminateSpecular( float3 vNormal, int specularPower )
{
	float3 N = normalize(vNormal);
	float3 E = normalize(g_vEyeDir);
	float3 R = reflect(E,N);
	float fresnel = 1 - dot(-E,N);
	fresnel = fresnel*fresnel*fresnel;
	float3 Specular = g_txEnvironmentMap.Sample(g_samLinear,R).xyz*fresnel;
	return Specular * (float3)m_bEnable;
}

//--------------------------------------------------------------------------------------
// Material Class Methods
//--------------------------------------------------------------------------------------
float3 cPlasticTexturedMaterial::GetAmbientColor(float2 vTexcoord)
{
	float4 vDiffuse = (float4)1.f;
	vDiffuse = g_txDiffuse.Sample(g_samLinear,vTexcoord);
	return m_vColor * vDiffuse.xyz;

}

float3 cPlasticTexturedMaterial::GetDiffuseColor(float2 vTexcoord)
{
	float4 vDiffuse = (float4)1.f;
	vDiffuse = g_txDiffuse.Sample(g_samLinear,vTexcoord);
	return m_vColor * vDiffuse.xyz;

}

float3 cRoughTexturedMaterial::GetAmbientColor(float2 vTexcoord)
{
	float4 vDiffuse =  (float4)1.f;
	vDiffuse = g_txDiffuse.Sample(g_samLinear,vTexcoord);
	return m_vColor * vDiffuse;

}

float3 cRoughTexturedMaterial::GetDiffuseColor(float2 vTexcoord)
{
	float4 vDiffuse = (float4)1.f;
	vDiffuse = g_txDiffuse.Sample(g_samLinear,vTexcoord);
	return m_vColor * vDiffuse;

}