
#include "StateH.hlsl"

#define INPUT_PATCH_SIZE 16
#define OUTPUT_PATCH_SIZE 16

uint rasterizerMode = 0;
uint HsMode = 0;
cbuffer cbPerFrame : register(b0)
{
    matrix g_mViewProjection;
    float3 g_vCameraPosWorld;
    float g_fTessellationFactor;
};


struct VS_IN
{
    float3 vPosition : POSITION;
};
struct VS_OUT
{
    float3 vPosition : POSITION;
};


struct Const_HS_OUT
{
    float Edge[4] : SV_TessFactor;
    float Inside[2] :SV_InsideTessFactor;
};

struct Control_HS_OUT
{
    float3 vPosition : POSITION;
};

struct Domain_OUT
{
    float4 posH : SV_Position;
    float3 worldW : POSITIONT;
    float3 Normal : NORMAL;
};
VS_OUT VS(VS_IN input)
{
    VS_OUT output;
    output.vPosition = input.vPosition;
    return output;
};



Const_HS_OUT ConstHS(InputPatch<VS_OUT, INPUT_PATCH_SIZE> patch, uint PatchID : SV_PrimitiveID)
{
    Const_HS_OUT output;
    output.Edge[0] = output.Edge[1] = output.Edge[2] = output.Edge[3] = g_fTessellationFactor;
    output.Inside[0] = output.Inside[1] = g_fTessellationFactor;
    return output;
};


[domain("quad")]
[partitioning("integer")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(OUTPUT_PATCH_SIZE)]
[patchconstantfunc("BezierConstantHS")]
Control_HS_OUT ControlHS(InputPatch<VS_OUT, INPUT_PATCH_SIZE> patch,uint i :SV_OutputControlPointID,uint patchId:SV_PrimitiveID)
{
    Control_HS_OUT output;
    output.vPosition = patch[i].vPosition;
    return output;
    
}
[domain("quad")]
[partitioning("fractional_odd")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(OUTPUT_PATCH_SIZE)]
[patchconstantfunc("BezierConstantHS")]
Control_HS_OUT ControlHSOdd(InputPatch<VS_OUT, INPUT_PATCH_SIZE> patch, uint i : SV_OutputControlPointID, uint patchId : SV_PrimitiveID)
{
    Control_HS_OUT output;
    output.vPosition = patch[i].vPosition;
    return output;
    
}

float4 BernsteinBasis(float t)
{
    float invT = 1.0f - t;

    return float4(invT * invT * invT,
                   3.0f * t * invT * invT,
                   3.0f * t * t * invT,
                   t * t * t);
}


float4 dBernsteinBasis(float t)
{
    float invT = 1.0f - t;

    return float4(-3 * invT * invT,
                   3 * invT * invT - 6 * t * invT,
                   6 * t * invT - 3 * t * t,
                   3 * t * t);
}

float3 BezierPos(const OutputPatch<Control_HS_OUT, OUTPUT_PATCH_SIZE> bezpatch,
                       float4 BasisU,
                       float4 BasisV)
{
    float3 Value = float3(0, 0, 0);
    Value = BasisV.x * (bezpatch[0].vPosition * BasisU.x + bezpatch[1].vPosition * BasisU.y + bezpatch[2].vPosition * BasisU.z + bezpatch[3].vPosition * BasisU.w);
    Value += BasisV.y * (bezpatch[4].vPosition * BasisU.x + bezpatch[5].vPosition * BasisU.y + bezpatch[6].vPosition * BasisU.z + bezpatch[7].vPosition * BasisU.w);
    Value += BasisV.z * (bezpatch[8].vPosition * BasisU.x + bezpatch[9].vPosition * BasisU.y + bezpatch[10].vPosition * BasisU.z + bezpatch[11].vPosition * BasisU.w);
    Value += BasisV.w * (bezpatch[12].vPosition * BasisU.x + bezpatch[13].vPosition * BasisU.y + bezpatch[14].vPosition * BasisU.z + bezpatch[15].vPosition * BasisU.w);

    return Value;
}



[domain("quad")]
Domain_OUT DS(Const_HS_OUT input, float2 UV : SV_DomainLocation,
                const OutputPatch<Control_HS_OUT, OUTPUT_PATCH_SIZE> bezpatch)
{
    Domain_OUT output;
    float4 u = BernsteinBasis(UV.x);
    float4 v = BernsteinBasis(UV.y);
    float du = dBernsteinBasis(UV.x);
    float dv = dBernsteinBasis(UV.y);
    
    float3 WorldPos = BezierPos(bezpatch, u, v);
    float3 Tangent = BezierPos(bezpatch, du,v);
    float3 BiTangent = BezierPos(bezpatch, u, dv);
    float3 Norm = normalize(cross(Tangent, BiTangent));
    output.posH = mul(float4(WorldPos, 1.f), g_mViewProjection);
    output.Normal = Norm;
    output.worldW = WorldPos;
    return output;
}


float4 PS(Domain_OUT Input)
{
    float3 N = normalize(Input.Normal);
    float3 L = normalize(Input.worldW - g_vCameraPosWorld);
    return abs(dot(N, L)) * float4(1, 0, 0, 1);
}



//HullShader HS[2] = { CompileShader(hs_5_0, ControlHS()), CompileShader(hs_5_0, ControlHSOdd()) };

technique11 Bezier
{
    pass
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetHullShader(HS[HsMode]);
        SetDomainShader(CompileShader(ds_5_0, DS()));
        SetPixelShader(CompileShader(ps_5_0, OccuPS()));
        SetRasterizerState(wireframeState[rasterizerMode]);
    }
}

