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
    float Time;
};

cbuffer cbUserChanges
{
    float Waviness;
};

struct VS_INPUT
{
    float3 Pos          : POSITION;        
    float3 Norm         : NORMAL;          
    float2 Tex          : TEXCOORD0;       
};

struct PS_INPUT
{
    float4 Pos : SV_POSITION;
    float4 PosW : POSITION;
    float3 Norm : TEXCOORD0;
    float2 Tex : TEXCOORD1;
};



struct refPS_INPUT
{
    float4 Pos : SV_POSITION;
    float3 Norm : TEXCOORD0;
    float3 cubeTex : TEXCOORD1;
    float2 Tex : TEXCOORD2;
};

struct GS_NormalDebugINPUT
{
    float4 Pos : SV_POSITION;
    float3 Norm : TEXCOORD0;
   
};

struct PS_NormalDebugINPUT
{
    float4 Pos : SV_POSITION;
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

BlendState NoBlending
{
    AlphaToCoverageEnable = FALSE;
    BlendEnable[0] = FALSE;
};


//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT VS( VS_INPUT input )
{
    PS_INPUT output = (PS_INPUT)0;
    
    float4 inpos = float4(input.Pos, 1.f);
    //output.PosO = output.Pos;
    output.Pos = mul(inpos, World);
    output.Pos.x += sin(output.Pos.y * 0.1 + Time) * Waviness;
    output.PosW = output.Pos;
    output.Pos = mul( output.Pos, View );
    output.Pos = mul( output.Pos, Projection );
    output.Norm = normalize(mul(input.Norm, World));
    output.Tex = input.Tex;
    
    return output;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 BasePS( PS_INPUT input) : SV_Target
{
    // Calculate lighting assuming light color is <1,1,1,1>
    float fLighting = saturate( dot( input.Norm, vLightDir ) );
    float4 outputColor = g_txDiffuse.Sample( samLinear, input.Tex ) * fLighting;
    outputColor.a = 1;
    return outputColor;
}

refPS_INPUT RefVS(VS_INPUT input)
{
    refPS_INPUT output = (refPS_INPUT) 0;
    
    output.Pos = mul(float4(input.Pos, 1), World);
    float3 norV = mul(input.Norm, mul(World,View));
    float3 ref = reflect(norV, float3(0, 0, -1));
    
    output.Pos = mul(output.Pos, View);

    output.Pos = mul(output.Pos, Projection);
    output.Norm = normalize(mul(input.Norm, World));

    output.Tex = input.Tex;
    output.cubeTex = ref;
    return output;
}

float4 RefPS(refPS_INPUT input) : SV_Target
{
    // Calculate lighting assuming light color is <1,1,1,1>
    float fLighting = saturate(dot(input.Norm, vLightDir));
    float4 refColor = Waviness * g_EnvCube.Sample(samLinear, input.cubeTex);
    float4 texColor = (1 - Waviness) * g_txDiffuse.Sample(samLinear, input.Tex);
    float4 outputColor = saturate((refColor + texColor) * fLighting);
    outputColor.a = 1;
    return outputColor;
}

GS_NormalDebugINPUT normalVS(VS_INPUT input)
{
    GS_NormalDebugINPUT output;
    output.Pos = mul(float4(input.Pos, 1), World);
    output.Norm = normalize(mul(input.Norm, World));
    return output;
}

GS_NormalDebugINPUT DynamicNormalVS(VS_INPUT input)
{
    GS_NormalDebugINPUT output;
    output.Pos = float4(input.Pos, 1);
  
    output.Norm = input.Norm;

    return output;
}

[maxvertexcount(4)]
void outputGS(triangle PS_INPUT input[3],
        inout TriangleStream< PS_INPUT> output)
{
    float3 faceNormal;
    float3 faceEdge0 = input[0].PosW - input[1].PosW;
    float3 faceEdge1 = input[0].PosW - input[2].PosW;
    //float3 faceEdge2 = input[2].Pos - input[4].Pos;
    faceNormal = normalize(cross(faceEdge0, faceEdge1));
    /*
    float3 faceEdge3 = input[0].Pos - input[1].Pos;
    faceNormal[1] = normalize(cross(faceEdge3, faceEdge0));
    float3 faceEdge4 = input[0].Pos - input[5].Pos;
    faceNormal[2] = normalize(cross(faceEdge1, faceEdge4));
    float3 faceEdge5 = input[2].Pos - input[3].Pos;
   faceNormal[3] = normalize(cross(faceEdge5, faceEdge2));
    */
    PS_INPUT psIn;
    [unroll]
        for (int i = 0; i <3; i +=1)
        {
            psIn.Pos = input[i].Pos;
            psIn.Tex = input[i].Tex;
            psIn.Norm = faceNormal;
            psIn.PosW = input[i].PosW;
        //psIn.Norm
            output.Append(psIn);
       
        }
        
    
}

[maxvertexcount(2)]
void normalGS(point GS_NormalDebugINPUT input[1],
        inout LineStream<PS_NormalDebugINPUT> output)
{
    PS_NormalDebugINPUT psIn;
    psIn.Pos = float4(input[0].Pos.xyz + input[0].Norm*0.01f, 1);
    psIn.Pos = mul(psIn.Pos, View);
    psIn.Pos = mul(psIn.Pos, Projection);
    output.Append(psIn);
    psIn.Pos = float4(input[0].Pos.xyz + input[0].Norm * 10.f,1);
    psIn.Pos = mul(psIn.Pos, View);
    psIn.Pos = mul(psIn.Pos, Projection);
    output.Append(psIn);
    output.RestartStrip();

}

float4 normalPS(PS_NormalDebugINPUT input) : SV_TARGET
{
    return float4(1, 1, 1, 1);
}

//--------------------------------------------------------------------------------------
// Technique
//--------------------------------------------------------------------------------------
technique11 BaseEffect
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader(CompileShader(ps_4_0, BasePS()));

        SetDepthStencilState( EnableDepth, 0 );
        SetBlendState( NoBlending, float4( 0.0f, 0.0f, 0.0f, 0.0f ), 0xFFFFFFFF );
    }
    
}

technique11 ReflactEffect
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_4_0, RefVS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_4_0, RefPS()));

        SetDepthStencilState(EnableDepth, 0);
        SetBlendState(NoBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
    }
    
}


technique11 NormalGSEffect
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
    pass P1
    {
        SetVertexShader(CompileShader(vs_4_0, normalVS()));
        SetGeometryShader(CompileShader(gs_4_0, normalGS()));
        SetPixelShader(CompileShader(ps_4_0, normalPS()));

        SetDepthStencilState(EnableDepth, 0);
        SetBlendState(NoBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetRasterizerState(g_rasterizerState[g_fillMode]);
    }
}

//effectÁ÷Êä³ö https://docs.microsoft.com/en-us/windows/win32/direct3d11/d3d11-graphics-reference-effect-streamout
technique11 DynamicNormalGSEffect
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(ConstructGSWithSO(CompileShader(gs_5_0, outputGS()), "0:POSITION.xyz; 0:TEXCOORD0.xyz;0:TEXCOORD1.xy", NULL, NULL, NULL, 0));
        SetPixelShader(CompileShader(ps_5_0, BasePS()));

        SetDepthStencilState(EnableDepth, 0);
        SetBlendState(NoBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetRasterizerState(g_rasterizerState[g_fillMode]);
    }
    pass P1
    {

        SetVertexShader(CompileShader(vs_5_0, DynamicNormalVS()));
        SetGeometryShader(CompileShader(gs_5_0, normalGS()));
        SetPixelShader(CompileShader(ps_5_0, normalPS()));

        SetDepthStencilState(EnableDepth, 0);
        SetBlendState(NoBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetRasterizerState(g_rasterizerState[g_fillMode]);
    }
}
