//--------------------------------------------------------------------------------------
// File: Tutorial11.fx
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
Texture2D g_txDiffuse : register(t0);
TextureCube g_EnvCube;


uint g_fillMode = 0;
BlendState BlendTop
{
    AlphaToCoverageEnable = FALSE;
    BlendEnable[0] = TRUE;
    SrcBlend = SRC_ALPHA;
    DestBlend = ONE;
    BlendOp = ADD;
    SrcBlendAlpha = ZERO;
    DestBlendAlpha = Zero;
    BlendOpAlpha = ADD;
    RenderTargetWriteMask[0] = 0x0F;
};
BlendState TestBlend
{
    AlphaToCoverageEnable = FALSE;
    BlendEnable[0] = TRUE;
    SrcBlend = SRC_ALPHA;
    DestBlend = ONE;
    BlendOp = ADD;
    SrcBlendAlpha = ZERO;
    DestBlendAlpha = Zero;
    BlendOpAlpha = ADD;
    RenderTargetWriteMask[0] = 0x00;
};
RasterizerState g_rasterizerState[3]
{
    {
        FillMode = SOLID;
        MultisampleEnable = true;
    },
    {
        FillMode = WIREFRAME;
        MultisampleEnable = true;
    },

    {
        FillMode = SOLID;
        MultisampleEnable = true;
        CullMode = NONE;
    }
};
SamplerState samLinear
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

cbuffer cbConstant
{
    
};

cbuffer cbChangesEveryFrame
{
    matrix World;
    matrix View;
    matrix Projection;
    float4 vLightDir; //= float3(-0.577, 0.577, -0.577);
};

cbuffer cbUserChanges
{
    float Waviness;
};

struct VS_INPUT
{
    float3 Pos : POSITION;
    float3 Norm : NORMAL;
    float2 Tex : TEXCOORD0;
};

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float2 Tex : TEXCOORD1;
};



//--------------------------------------------------------------------------------------
// DepthStates
//--------------------------------------------------------------------------------------
DepthStencilState EnableDepth
{
    DepthEnable = TRUE;
    DepthWriteMask = ALL;
    DepthFunc = LESS_EQUAL;
};

DepthStencilState NoDepthTestWrite
{
    DepthEnable = false;
    DepthWriteMask = ZERO;
   
};

DepthStencilState DepthTestNoWrite
{
    DepthEnable = true;
    DepthWriteMask = ZERO;
   
};

BlendState NoBlending
{
    AlphaToCoverageEnable = FALSE;
    BlendEnable[0] = FALSE;
};


//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT VS(VS_INPUT input)
{
    PS_INPUT output = (PS_INPUT) 0;
    
    float4 inpos = float4(input.Pos, 1.f);
    //output.PosO = output.Pos;
    output.Pos = mul(inpos, World);
    output.Pos = mul(output.Pos, View);
    output.Pos = mul(output.Pos, Projection);
    output.Tex = input.Tex;
    
    return output;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 BasePS(PS_INPUT input) : SV_Target
{
    // Calculate lighting assuming light color is <1,1,1,1>

    float4 outputColor = g_txDiffuse.Sample(samLinear, input.Tex);
    outputColor.a = 1;
    return outputColor;
}

float4 OccuPS(PS_INPUT input) : SV_Target
{
    // Calculate lighting assuming light color is <1,1,1,1>

    
    return float4(1,0,0,0.5);
}

//--------------------------------------------------------------------------------------
// Technique
//--------------------------------------------------------------------------------------
technique11 CityEffect
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_4_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_4_0, BasePS()));

        SetDepthStencilState(EnableDepth, 0);
        SetBlendState(NoBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetRasterizerState(g_rasterizerState[g_fillMode]);
    }
    
}

technique11 OcculuderTop
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, OccuPS()));

        SetDepthStencilState(NoDepthTestWrite, 0);
        SetBlendState(BlendTop, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetRasterizerState(g_rasterizerState[2]);
    }
    
}

technique11 OccluderTest
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_4_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_4_0, OccuPS()));

        SetDepthStencilState(DepthTestNoWrite, 0);
        SetBlendState(TestBlend, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetRasterizerState(g_rasterizerState[2]);
    }
    
}