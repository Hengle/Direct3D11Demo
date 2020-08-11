






struct VS_NormalDebugINPUT
{
    float3 Pos          : POSITION;        
    float3 Norm         : NORMAL;          
    float2 Tex          : TEXCOORD0;       
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

GS_NormalDebugINPUT normalVS(VS_NormalDebugINPUT input)
{
    GS_NormalDebugINPUT output;
    output.Pos = mul(float4(input.Pos, 1), World);
    output.Norm = normalize(mul(input.Norm, World));
    return output;
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

technique11 NormalGSEffect
{

    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, normalVS()));
        SetGeometryShader(CompileShader(gs_5_0, normalGS()));
        SetPixelShader(CompileShader(ps_5_0, normalPS()));

        SetDepthStencilState(EnableDepth, 0);
        SetBlendState(NoBlending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetRasterizerState(g_rasterizerState[g_fillMode]);
    }
}