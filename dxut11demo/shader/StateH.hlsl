
#define SolidState 0
#define WireframeState 1
#define NoCull 2
#define NoCullWireframe 3
//--------------------------------------------------------------------------------------
// Rasterization State
//--------------------------------------------------------------------------------------
RasterizerState g_rasterizerState[4]
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
	CullMode = NONE;
    MultisampleEnable = true;
},
{
    FillMode = WIREFRAME;
	CullMode = NONE;
    MultisampleEnable = true;
}

};
uint g_rasterizerStateCount = 0;
//--------------------------------------------------------------------------------------
// Textures and Samplers
//--------------------------------------------------------------------------------------


SamplerState   g_samLinear : register( s0 )
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = WRAP;
    AddressV = WRAP;
    AddressW = WRAP;
};
SamplerState   g_samLinearMIRROR : register( s1 )
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = MIRROR;
    AddressV =MIRROR;
    AddressW = MIRROR;
};

SamplerComparisonState gShadowSample : register(s1)
{
    Filter = COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
    AddressU = BORDER;
    AddressV = BORDER;
    AddressW = BORDER;
    MipLODBias = 0.f;
    MaxAnisotropy = 16;
    ComparisonFunc = LESS_EQUAL;
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

static const float pi = 3.1415926f;
static const float invpi = 1.f/pi;
