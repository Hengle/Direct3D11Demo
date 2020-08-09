
#include"DXUT.h"
#include"defaultScene.h"
#include <d3dx11effect.h>
#include <process.h>

#include <algorithm>
using namespace DirectX;

enum RenderType
{
	shadowRender,
	AllRender,
	DynamicRender
};

static CModelViewerCamera          g_Camera;

static ComPtr<ID3D11InputLayout>          g_pVertexLayout = nullptr;
static XMMATRIX                    g_World;
static XMMATRIX                    g_View;
static XMMATRIX                    g_Projection;
static XMFLOAT4                    g_vMeshColor(0.7f, 0.7f, 0.7f, 1.0f);
static XMVECTORF32 				   g_vLightDir = { -0.577,0.577,-0.577 };

static ID3D11ShaderResourceView*          cubeSRV = nullptr;
//static ID3D11Buffer*						SOVertexBuffer = nullptr;
static ID3D11Buffer* nullBuffer = nullptr;
void renderScene(ID3D11DeviceContext* pd3dContext,bool bIsShadow);
void renderSkyBox(ID3D11DeviceContext* pd3dContext);
void renderShadow(ID3D11DeviceContext* pd3dContext);
void renderAllScene(ID3D11DeviceContext* pd3dContext);
void renderDynamicCube(ID3D11DeviceContext* pd3dContext, int instance);
//--------------------------------------------------------------------------------------
// Effect Declare
//--------------------------------------------------------------------------------------
static ID3DX11Effect*					   g_pEffect = nullptr;
static ID3DX11EffectTechnique*             g_pSkyBoxTechnique = nullptr;
static ID3DX11EffectTechnique*             g_pSceneTechnique = nullptr;
static ID3DX11EffectMatrixVariable*        g_pWorldVariable = nullptr;
static ID3DX11EffectMatrixVariable*        g_pViewProjectionVariable = nullptr;
static ID3DX11EffectShaderResourceVariable*g_ptxEnvVariable = nullptr;
static ID3DX11EffectShaderResourceVariable*g_ptxShadowVariable = nullptr;
static ID3DX11EffectVectorVariable*		   g_pEyePosWVariable = nullptr;
static ID3DX11EffectScalarVariable*		   g_pStateIndexVariable = nullptr;
static ID3DX11EffectScalarVariable*		   g_pSceneStateIndexVariable = nullptr;
static ID3DX11EffectConstantBuffer*		   g_pLightCbuffer = nullptr;
static ID3DX11EffectConstantBuffer*		   g_pFrameCbufferVariable = nullptr;
static ID3DX11EffectConstantBuffer*		   g_pObjCbufferVariable = nullptr;
static ID3DX11EffectDepthStencilVariable*  g_pDepthStencilStateVariable = nullptr;
static ID3DX11EffectRasterizerVariable*	   g_pRasterizerVariable = nullptr;
static ID3DX11EffectBlendVariable*		   g_pBlendStateVariable = nullptr;
static ID3DX11EffectShaderVariable*		   g_pVertexShaderVariable = nullptr;
static ID3DX11EffectShaderVariable*		   g_pPixelShaderVariable = nullptr;
static ID3DX11EffectShaderVariable*		   g_pDynamicBoxPixelShaderVariable = nullptr;
static ID3DX11EffectSamplerVariable*	   g_pSamplerVariable = nullptr;
static ID3DX11EffectSamplerVariable*	   g_pLinearSamplerVariable = nullptr;

static ID3DX11EffectScalarVariable*		   g_pObjScaleVariable = nullptr;
static ID3DX11EffectVectorVariable*		   g_pObjColorVariable = nullptr;

static ID3D11Buffer*					   g_pLightBuffer = nullptr;
static ID3D11Buffer*					   g_pFrameBuffer = nullptr;
static ID3D11Buffer*					   g_pObjBuffer = nullptr;
static ID3D11DepthStencilState*			   g_pDepthStencilState = nullptr;
static ID3D11RasterizerState*			   g_pRasterizerState = nullptr;
static ID3D11BlendState*				   g_pBlendState = nullptr;
static ID3D11VertexShader*				   g_pVertexShader = nullptr;
static ID3D11PixelShader*				   g_pPixelShader = nullptr;
static ID3D11PixelShader*				   g_pDynamicBoxPixelShader = nullptr;
static ID3D11SamplerState*				   g_pSampler = nullptr;
static ID3D11SamplerState*				   g_pLinearSampler = nullptr;
static int								   g_iRasterizerCount = 0;

//--------------------------------------------------------------------------------------
// Mesh
//--------------------------------------------------------------------------------------
static CDXUTSDKMesh                        g_Mesh;
static CDXUTSDKMesh                        g_SkyBox;
static CDXUTSDKMesh						   g_Sphere;
static CDXUTSDKMesh						   g_Plane;

//--------------------------------------------------------------------------------------
// Variable
//--------------------------------------------------------------------------------------
static int									EffectIndex = 0;
static int									preEffectIndex = 0;
static int									g_isphereRowCount = 6;
static XMMATRIX							    g_SpherePos[25];
static bool g_bIsShadow = false;
static const XMVECTORF32 s_Eye = { 0.0f, 1.0f, -1.0f, 0.f };
static const XMVECTORF32 s_At = { 0.0f, 0.0f, 0.0f, 0.f };


static bool g_bCameraView = true;
static D3D11_INPUT_ELEMENT_DESC layout[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};
//--------------------------------------------------------------------------------------
// Const Buffer
//--------------------------------------------------------------------------------------
static struct PerObj
{
	XMFLOAT3 color;
	float scale;
	XMFLOAT4X4 m_mWorld;
};


static struct PerFrame
{
	XMFLOAT4X4 m_mViewProj;
	
	XMFLOAT4 m_vEyePosW;
};
//--------------------------------------------------------------------------------------
// Light
//--------------------------------------------------------------------------------------
const int g_iLightNum = 1;
struct PerLight
{
	struct LightData
	{
		XMFLOAT4X4  m_mLightViewProj;
		XMFLOAT4 m_vLightPos;
		XMFLOAT4 m_vLightDir;
		XMFLOAT4 m_vLightColor;
		XMFLOAT4 m_vFalloffs;    // x = dist end, y = dist range, z = cos angle end, w = cos range
	}m_LightData[g_iLightNum];
}g_cbPerLight;
static XMMATRIX g_vViewProj[2];
static float g_fLightRadius = 10;

static float g_aLightFarPlane[g_iLightNum];
static float g_aLightNearPlane[g_iLightNum];
static float g_aLightFov[g_iLightNum];
static float g_aLightAspect[g_iLightNum];

void initLight()
{
	XMVECTOR Dir = { -0.67f, -1.f, -0.21f, 0.f };
	auto& lightData = g_cbPerLight.m_LightData;
	lightData[0].m_vLightColor = XMFLOAT4(3.0f * 0.160f, 3.0f * 0.341f, 3.0f * 1.000f, 1.000f);
	XMStoreFloat4(&lightData[0].m_vLightDir, Dir);
	XMStoreFloat4(&lightData[0].m_vLightPos, -g_fLightRadius * Dir);

	for (int i = 0; i < g_iLightNum; i++)
	{
		g_aLightFarPlane[i] = g_fLightRadius + 5;
		g_aLightNearPlane[i] = 0.1f;
		g_aLightAspect[i] = 1.f;
		g_aLightFov[i] = XM_PI / 2.0f;

		lightData[i].m_vFalloffs.x = g_aLightFarPlane[i];
		lightData[i].m_vFalloffs.y = g_fLightRadius;
		lightData[i].m_vFalloffs.z = cos(g_aLightFov[i] * 0.5f);
		lightData[i].m_vFalloffs.w = 0.1f;
	}
}


//--------------------------------------------------------------------------------------
// shadow
//--------------------------------------------------------------------------------------
const int g_iNumShadows = 1;
static ID3D11Texture2D*				g_pShadowTex[g_iNumShadows] = {nullptr};
static ID3D11ShaderResourceView*	g_pShadowResourceView[g_iNumShadows] = {nullptr};
static ID3D11DepthStencilView*     g_pShadowDepthStencilView[g_iNumShadows] = { nullptr };
static D3D11_VIEWPORT              g_ShadowViewport[g_iNumShadows] = { 0 };





void initShadowResource(ID3D11Device* pd3dDevice)
{
	for (int i = 0; i < g_iNumShadows; i++)
	{
		D3D11_TEXTURE2D_DESC txDesc;
		txDesc.Width = 1024;
		txDesc.Height = 1024;
		txDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;
		txDesc.Usage = D3D11_USAGE_DEFAULT;
		txDesc.MipLevels = 1;
		txDesc.ArraySize = 1;
		txDesc.Format = DXGI_FORMAT_R32_TYPELESS;
		txDesc.SampleDesc.Count = 1;
		txDesc.SampleDesc.Quality = 0;
		txDesc.CPUAccessFlags = 0;
		txDesc.MiscFlags = 0;

		D3D11_DEPTH_STENCIL_VIEW_DESC dsDesc;
		dsDesc.Texture2D.MipSlice = 0;
		dsDesc.Format = DXGI_FORMAT_D32_FLOAT;
		dsDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		dsDesc.Flags = 0;
		
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		srvDesc.Texture2D.MipLevels = 1;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;

		pd3dDevice->CreateTexture2D(&txDesc, nullptr, &g_pShadowTex[i]);
		pd3dDevice->CreateDepthStencilView(g_pShadowTex[i], &dsDesc, &g_pShadowDepthStencilView[i]);
		pd3dDevice->CreateShaderResourceView(g_pShadowTex[i], &srvDesc, &g_pShadowResourceView[i]);
		g_ShadowViewport[i].Height = 1024;
		g_ShadowViewport[i].Width = 1024;
		g_ShadowViewport[i].MaxDepth = 1;
		g_ShadowViewport[i].MinDepth = 0;
		g_ShadowViewport[i].TopLeftX = 0;
		g_ShadowViewport[i].TopLeftY = 0;
	}


}
//--------------------------------------------------------------------------------------
// dynamicCube
//--------------------------------------------------------------------------------------
static const int g_iCubeCount = 1;
static bool g_bdynamicCube = false;
static ID3D11Texture2D*					   g_pCube2D= { nullptr };
static ID3D11ShaderResourceView*		   g_pCubeSRV = { nullptr };
static ID3D11RenderTargetView*			   g_pCubeRTV[6] = { nullptr };
static ID3D11Texture2D*					   g_pDepthTex = nullptr;
static ID3D11DepthStencilView*			   g_pCubeDSV = nullptr;
static D3D11_VIEWPORT					   g_CubeVP;
static XMMATRIX							   g_mCubeViewProj[6];
static XMVECTOR							   g_vDirect[6] = { {1,0,0,0},{-1,0,0,0},{0,1,0,0},{0,-1,0,0},{0,0,1,0},{0,0,-1,0}};
static XMVECTOR							   g_vUp[6] = { {0,1,0,0},{0,1,0,0},{0,0,-1,0},{0,0,1,0},{0,1,0,0},{0,1,0,0}, };
static XMVECTOR							   g_vCenter = { 0,3,0,0 };
HRESULT initdynamicCube(ID3D11Device* pd3dDevice)
{
	int w, h;
	w = h = 520;
	for (int i = 0; i < g_iCubeCount; i++) {
		HRESULT hr;
		D3D11_TEXTURE2D_DESC txDesc;
		txDesc.Width = w;
		txDesc.Height = h;
		txDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
		txDesc.Usage = D3D11_USAGE_DEFAULT;
		txDesc.MipLevels = 1;
		txDesc.ArraySize = 6;
		txDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;;
		txDesc.SampleDesc.Count = 1;
		txDesc.SampleDesc.Quality = 0;
		txDesc.CPUAccessFlags = 0;
		txDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
		pd3dDevice->CreateTexture2D(&txDesc, nullptr, &g_pCube2D);
		D3D11_RENDER_TARGET_VIEW_DESC rtDesc;

		for (int j = 0; j < 6; j++) {
			rtDesc.Texture2DArray.FirstArraySlice = j;
			rtDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;;
			rtDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
			rtDesc.Texture2DArray.ArraySize = 1;
			rtDesc.Texture2DArray.MipSlice = 0;
			V_RETURN(pd3dDevice->CreateRenderTargetView(g_pCube2D, &rtDesc, &g_pCubeRTV[j]));
		}
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		srvDesc.Texture2D.MipLevels = -1;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
		pd3dDevice->CreateShaderResourceView(g_pCube2D, &srvDesc, &g_pCubeSRV);

	}

	D3D11_TEXTURE2D_DESC txDesc;
	txDesc.Width = w;
	txDesc.Height = h;
	txDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	txDesc.Usage = D3D11_USAGE_DEFAULT;
	txDesc.MipLevels = 1;
	txDesc.ArraySize = 1;
	txDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	txDesc.SampleDesc.Count = 1;
	txDesc.SampleDesc.Quality = 0;
	txDesc.CPUAccessFlags = 0;
	txDesc.MiscFlags = 0;

	D3D11_DEPTH_STENCIL_VIEW_DESC dsDesc;
	dsDesc.Texture2D.MipSlice = 0;
	dsDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsDesc.Flags = 0;

	g_CubeVP.Height = h;
	g_CubeVP.Width = w;
	g_CubeVP.MaxDepth = 1;
	g_CubeVP.MinDepth = 0;
	g_CubeVP.TopLeftX = 0;
	g_CubeVP.TopLeftY = 0;

	pd3dDevice->CreateTexture2D(&txDesc, nullptr, &g_pDepthTex);
	pd3dDevice->CreateDepthStencilView(g_pDepthTex, &dsDesc, &g_pCubeDSV);

	//g_mDirect[0] = XMVectorSet(1, 0, 0, 0);
	//g_mDirect[1] = XMVectorSet(-1, 0, 0, 0);
	//g_mDirect[2] = XMVectorSet(0, 1, 0, 0);
	//g_mDirect[3] = XMVectorSet(0, -1, 0, 0);
	//g_mDirect[4] = XMVectorSet(0, 0, 1, 0);
	//g_mDirect[5] = XMVectorSet(0, 0, -1, 0);
	
	for (int i = 0; i < 6; i++)
	{
		XMMATRIX view = XMMatrixLookAtLH(g_vCenter, g_vCenter + g_vDirect[i], g_vUp[i]);
		XMMATRIX proj = XMMatrixPerspectiveFovLH(XM_PI*0.5f,1.f,0.5,10);
		g_mCubeViewProj[i] = view * proj;
	}
}
//--------------------------------------------------------------------------------------
// Thread
//--------------------------------------------------------------------------------------
enum ProcessType{
	Immediate,
	MultiThread
};
const int g_iNumSceneThread =		g_iNumShadows  + g_iCubeCount*6 + 1;
static HANDLE						g_hPerSceneRenderDeferredThread[g_iNumSceneThread];
static HANDLE						g_hBeginSceneThreadEvent[g_iNumSceneThread];
static HANDLE						g_hEndSceneThreadEvent[g_iNumSceneThread];
static ID3D11DeviceContext*			g_pd3dSceneDeferredContext[g_iNumSceneThread];
static ID3D11CommandList*			g_pd3dSceneCommandList[g_iNumSceneThread];
static int							g_iSceneThreadInstance[g_iNumSceneThread];
//由于Effect1无法保证Apply的操作原子性，因此需要使用信号量使其操作时，其他线程无法使用
HANDLE								g_hEffectSemaphore;
static ProcessType					g_bMutiThread = Immediate;

unsigned int WINAPI _PerSceneRenderDeferredProc(LPVOID lpParameter);
HRESULT InitWorkThreads(ID3D11Device*pd3dDevice)
{
	g_hEffectSemaphore = CreateSemaphore(nullptr, 1, 1, nullptr);
	HRESULT hr = S_OK;
	for (int i = 0; i < g_iNumSceneThread; i++) {
		g_iSceneThreadInstance[i] = i;
		g_hBeginSceneThreadEvent[i] = CreateEvent(nullptr, FALSE, FALSE, nullptr);
		g_hEndSceneThreadEvent[i] = CreateEvent(nullptr, FALSE, FALSE, nullptr);
		V_RETURN(pd3dDevice->CreateDeferredContext(0, &g_pd3dSceneDeferredContext[i]));
		g_hPerSceneRenderDeferredThread[i] = (HANDLE)_beginthreadex(nullptr, 0, _PerSceneRenderDeferredProc, &g_iSceneThreadInstance[i],CREATE_SUSPENDED,nullptr);
		
		ResumeThread(g_hPerSceneRenderDeferredThread[i]);
	}
	return S_OK;
}
unsigned int WINAPI _PerSceneRenderDeferredProc(LPVOID lpParameter)
{
	HRESULT hr;
	const int instance = *(int*)lpParameter;
	ID3D11DeviceContext* pd3dDeferredContex = g_pd3dSceneDeferredContext[instance];
	ID3D11CommandList*& pd3dCommandList = g_pd3dSceneCommandList[instance];
	
	while (true)
	{

		WaitForSingleObject(g_hBeginSceneThreadEvent[instance], INFINITE);
		
		if (instance < g_iNumShadows)
		{
			renderShadow(pd3dDeferredContex);
		}
		else if (instance < g_iNumShadows + g_iCubeCount)
		{
			renderDynamicCube(pd3dDeferredContex, instance - g_iNumShadows);
		}
		else
		{
			V(DXUTSetupD3D11Views(pd3dDeferredContex));
			renderAllScene(pd3dDeferredContex);
		}
		V(pd3dDeferredContex->FinishCommandList(false,&pd3dCommandList));
		SetEvent(g_hEndSceneThreadEvent[instance]);
	}

}


//--------------------------------------------------------------------------------------
//MainFunction
//--------------------------------------------------------------------------------------
static void FrameUI();
static HRESULT UpdateEffect(ID3D11Device* pd3dDevice);
std::string MultiThreadScene::getName()
{
	return sceneName;
}


HRESULT InitState(ID3D11Device* pd3dDevice)
{
	HRESULT hr;
	g_pLightCbuffer->GetConstantBuffer(&g_pLightBuffer);
	D3D11_BUFFER_DESC desc;
	g_pLightBuffer->GetDesc(&desc);
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.Usage = D3D11_USAGE_DYNAMIC;
	SAFE_RELEASE(g_pLightBuffer);
	V_RETURN(pd3dDevice->CreateBuffer(&desc, nullptr, &g_pLightBuffer));
	g_pLightCbuffer->SetConstantBuffer(g_pLightBuffer);


	g_pFrameCbufferVariable->GetConstantBuffer(&g_pFrameBuffer);

	g_pFrameBuffer->GetDesc(&desc);
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.Usage = D3D11_USAGE_DYNAMIC;
	SAFE_RELEASE(g_pFrameBuffer);
	V_RETURN(pd3dDevice->CreateBuffer(&desc, nullptr, &g_pFrameBuffer));
	g_pFrameCbufferVariable->SetConstantBuffer(g_pFrameBuffer);


	g_pObjCbufferVariable->GetConstantBuffer(&g_pObjBuffer);

	g_pObjBuffer->GetDesc(&desc);
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.Usage = D3D11_USAGE_DYNAMIC;
	SAFE_RELEASE(g_pObjBuffer);
	V_RETURN(pd3dDevice->CreateBuffer(&desc, nullptr, &g_pObjBuffer));
	g_pObjCbufferVariable->SetConstantBuffer(g_pObjBuffer);


	g_pRasterizerVariable->GetRasterizerState(1, &g_pRasterizerState);
	g_pBlendStateVariable->GetBlendState(0, &g_pBlendState);
	g_pDepthStencilStateVariable->GetDepthStencilState(0, &g_pDepthStencilState);
	g_pVertexShaderVariable->GetVertexShader(0, &g_pVertexShader);
	g_pPixelShaderVariable->GetPixelShader(0, &g_pPixelShader);
	g_pDynamicBoxPixelShaderVariable->GetPixelShader(0, &g_pDynamicBoxPixelShader);
	g_pSamplerVariable->GetSampler(0, &g_pSampler);
	g_pLinearSamplerVariable->GetSampler(0, &g_pLinearSampler);
	return hr;
}
HRESULT MultiThreadScene::initScene(ID3D11Device* pd3dDevice)
{
	isInit = true;
	HRESULT hr = S_OK;

	auto pd3dImmediateContext = DXUTGetD3D11DeviceContext();


	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#ifdef _DEBUG
	// Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
	// Setting this flag improves the shader debugging experience, but still allows 
	// the shaders to be optimized and to run exactly the way they will run in 
	// the release configuration of this program.
	dwShaderFlags |= D3DCOMPILE_DEBUG;

	// Disable optimizations to further improve shader debugging
	dwShaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

#if D3D_COMPILER_VERSION >= 46 

	// Read the D3DX effect file
	WCHAR str[MAX_PATH];
	V_RETURN(DXUTFindDXSDKMediaFileCch(str, MAX_PATH, L"shader//MultiThreadShahder.fx"));
	ID3DBlob* errorMsg = nullptr;


	hr = D3DX11CompileEffectFromFile(str, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, dwShaderFlags, 0, pd3dDevice, &g_pEffect, &errorMsg);
	if (FAILED(hr))
	{
		if (errorMsg)
		{
			OutputDebugStringA((char*)errorMsg->GetBufferPointer());
			errorMsg->Release();
		}

		if (g_pEffect)
			g_pEffect->Release();

		return hr;
	}
#else

	ID3DBlob* pEffectBuffer = nullptr;
	V_RETURN(DXUTCompileFromFile(L"Tutorial11.fx", nullptr, "none", "fx_5_0", dwShaderFlags, 0, &pEffectBuffer));
	hr = D3DX11CreateEffectFromMemory(pEffectBuffer->GetBufferPointer(), pEffectBuffer->GetBufferSize(), 0, pd3dDevice, &g_pEffect);
	SAFE_RELEASE(pEffectBuffer);
	if (FAILED(hr))
		return hr;

#endif

	// Obtain the technique
	g_pSkyBoxTechnique = g_pEffect->GetTechniqueByName("SkyboxEffect");
	g_pSceneTechnique = g_pEffect->GetTechniqueByName("SceneEffect");
	// Obtain the variables
	//g_ptxDiffuseVariable = g_pEffect->GetVariableByName("g_txDiffuse")->AsShaderResource();
	g_ptxEnvVariable = g_pEffect->GetVariableByName("g_txEnvCube")->AsShaderResource();
	g_ptxShadowVariable = g_pEffect->GetVariableByName("g_txShadowMap")->AsShaderResource();
	g_pWorldVariable = g_pEffect->GetVariableByName("g_mWorld")->AsMatrix();
	g_pViewProjectionVariable = g_pEffect->GetVariableByName("g_mViewProj")->AsMatrix();
	g_pEyePosWVariable = g_pEffect->GetVariableByName("g_vEyePosW")->AsVector();
	g_pStateIndexVariable = g_pEffect->GetVariableByName("g_rasterizerStateCount")->AsScalar();
	g_pSceneStateIndexVariable = g_pEffect->GetVariableByName("g_SceneStateCount")->AsScalar();
	g_pObjScaleVariable = g_pEffect->GetVariableByName("g_fScale")->AsScalar();
	g_pObjColorVariable = g_pEffect->GetVariableByName("g_vColor")->AsVector();
	g_pLightCbuffer = g_pEffect->GetConstantBufferByName("PerLight")->AsConstantBuffer();
	g_pFrameCbufferVariable = g_pEffect->GetConstantBufferByName("PerFrame")->AsConstantBuffer();
	g_pObjCbufferVariable = g_pEffect->GetConstantBufferByName("PerObj")->AsConstantBuffer();
	g_pDepthStencilStateVariable = g_pEffect->GetVariableByName("EnableDepth")->AsDepthStencil();
	g_pBlendStateVariable = g_pEffect->GetVariableByName("NoBlending")->AsBlend();
	g_pRasterizerVariable = g_pEffect->GetVariableByName("g_rasterizerState")->AsRasterizer();
	g_pVertexShaderVariable = g_pEffect->GetVariableByName("ComSceneVS")->AsShader();
	g_pPixelShaderVariable = g_pEffect->GetVariableByName("ComScenePS")->AsShader();
	g_pDynamicBoxPixelShaderVariable = g_pEffect->GetVariableByName("ComDynamicBoxPS")->AsShader();
	g_pSamplerVariable = g_pEffect->GetVariableByName("gShadowSample")->AsSampler();
	g_pLinearSamplerVariable = g_pEffect->GetVariableByName("g_samLinear")->AsSampler();
	InitState(pd3dDevice);

	float tmp = 1;
	g_pObjScaleVariable->SetFloat(tmp);
	XMFLOAT3 color = XMFLOAT3(1.f, 1.f, 1.f);
	g_pObjColorVariable->SetFloatVector(reinterpret_cast<float*>(&color));

	g_pStateIndexVariable->SetInt(2);

	g_pSceneStateIndexVariable->SetInt(0);
	// Set Waviness


	V_RETURN(DXUTCreateShaderResourceViewFromFile(pd3dDevice, L"Lobby\\LobbyCube.dds", &cubeSRV));
	g_ptxEnvVariable->SetResource(cubeSRV);




	// Define the input layout

	UINT numElements = ARRAYSIZE(layout);

	// Create the input layout
	D3DX11_PASS_DESC PassDesc;
	V_RETURN(g_pSkyBoxTechnique->GetPassByIndex(0)->GetDesc(&PassDesc));
	V_RETURN(pd3dDevice->CreateInputLayout(layout, numElements, PassDesc.pIAInputSignature,
		PassDesc.IAInputSignatureSize, g_pVertexLayout.GetAddressOf()));

	// Set the input layout
	//pd3dImmediateContext->IASetInputLayout(g_pVertexLayout.Get());

	// Load the mesh
	V_RETURN(g_Mesh.Create(pd3dDevice, L"SquidRoom\\SquidRoom.sdkmesh"));
	V_RETURN(g_Plane.Create(pd3dDevice, L"resource\\plane.sdkmesh"));
	V_RETURN(g_Sphere.Create(pd3dDevice, L"resource\\sphere.sdkmesh"));
	g_SkyBox = g_Sphere;
	




	// Initialize the world matrices
	g_World = XMMatrixIdentity();

	// Setup the camera's view parameters

	g_Camera.SetViewParams(s_Eye, s_At);


	initLight();
	initShadowResource(pd3dDevice);
	InitWorkThreads(pd3dDevice);
	initdynamicCube(pd3dDevice);
	return S_OK;
}

HRESULT CALLBACK MultiThreadScene::OnD3D11CreateDevice(ID3D11Device* pd3dDevice, const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc,
	void* pUserContext)
{

	return initScene(pd3dDevice);
}

HRESULT CALLBACK MultiThreadScene::OnD3D11ResizedSwapChain(ID3D11Device* pd3dDevice, IDXGISwapChain* pSwapChain,
	const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext)
{
	float FAspect = static_cast<float>(pBackBufferSurfaceDesc->Width) / static_cast<float>(pBackBufferSurfaceDesc->Height);
	g_Projection = XMMatrixPerspectiveFovLH(XM_PI*0.25f, FAspect, 0.1, 100.f);
	g_Camera.SetProjParams(XM_PI / 4, FAspect, 0.1f, 1000.0f);
	g_Camera.SetWindow(pBackBufferSurfaceDesc->Width, pBackBufferSurfaceDesc->Height);
	g_Camera.SetButtonMasks(MOUSE_LEFT_BUTTON, MOUSE_WHEEL, MOUSE_MIDDLE_BUTTON);
	g_Camera.SetEnablePositionMovement(true);
	return S_OK;
}

void renderSkyBox(ID3D11DeviceContext* pd3dImmediateContext)
{
	
	g_pSkyBoxTechnique->GetPassByIndex(0)->Apply(0, pd3dImmediateContext);
	g_SkyBox.Render(pd3dImmediateContext, INVALID_SAMPLER_SLOT, INVALID_SAMPLER_SLOT, INVALID_SAMPLER_SLOT);
}

void CaluLightViewProj(int Index)
{
	XMVECTOR vLightDir = XMLoadFloat4(&g_cbPerLight.m_LightData[Index].m_vLightDir);
	XMVECTOR vLightPos = XMLoadFloat4(&g_cbPerLight.m_LightData[Index].m_vLightPos);
	XMVECTOR lookAt = vLightDir*g_fLightRadius + vLightPos;
	XMMATRIX viewM = XMMatrixLookAtLH(vLightPos, lookAt, g_XMIdentityR1);

	XMMATRIX projM = XMMatrixPerspectiveFovLH(g_aLightFov[Index], g_aLightAspect[Index], g_aLightNearPlane[Index], g_aLightFarPlane[Index]);
	XMMATRIX vp =viewM * projM;
	g_vViewProj[1] = vp;
	XMStoreFloat4x4(&g_cbPerLight.m_LightData[Index].m_mLightViewProj,XMMatrixTranspose(vp));

}
void MapLightConstBuffer( ID3D11DeviceContext* pd3dContext)
{
	HRESULT hr = S_OK;
	D3D11_MAPPED_SUBRESOURCE MappedResource;
	
	pd3dContext->Map(g_pLightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);
	PerLight* cbperLight = reinterpret_cast<PerLight*> (MappedResource.pData);
	*cbperLight = g_cbPerLight;
	pd3dContext->Unmap(g_pLightBuffer,0);
	
}


void MapFrameBuffer(ID3D11DeviceContext* pd3dContext)
{

}
void Lock()
{
	if (g_bMutiThread == MultiThread)
		WaitForSingleObject(g_hEffectSemaphore, INFINITE);
}
void unLock()
{
	if (g_bMutiThread == MultiThread)
		ReleaseSemaphore(g_hEffectSemaphore, 1, nullptr);
}
void renderScene( ID3D11DeviceContext* pd3dContext, RenderType renderType)
{
	HRESULT hr = S_OK;

	pd3dContext->OMSetDepthStencilState(g_pDepthStencilState, 0);
	static float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	pd3dContext->OMSetBlendState(g_pBlendState, blendFactor, 0xFFFFFFFF);
	pd3dContext->RSSetState(g_pRasterizerState);

	pd3dContext->IASetInputLayout(g_pVertexLayout.Get());
	
		
	
	

	/*Lock();
	g_pWorldVariable->SetMatrix();
	g_pObjScaleVariable->SetFloat(40);
	g_pObjColorVariable->SetFloatVector(reinterpret_cast<float*>(&color));
	g_pViewProjectionVariable->SetMatrix(reinterpret_cast<float*>(&g_vViewProj[bIsShadow]));
	V(g_pSceneTechnique->GetPassByIndex(0)->Apply(0, pd3dContext));
	unLock();*/
	static XMFLOAT3 color = XMFLOAT3(1.f, 1.f, 1.f);
	if (renderType==shadowRender) {
		ID3D11ShaderResourceView* ppNullResources[g_iNumShadows] = { nullptr };
		pd3dContext->PSSetShaderResources(2, g_iNumShadows, ppNullResources);
		pd3dContext->PSSetShader(nullptr, nullptr, 0);
	}
	else
	{
		pd3dContext->PSSetConstantBuffers(1, 1, &g_pObjBuffer);
		pd3dContext->PSSetShader(g_pPixelShader, nullptr, 0);
		pd3dContext->PSSetConstantBuffers(2, 1, &g_pLightBuffer);
		pd3dContext->PSSetSamplers(1,1,&g_pSampler);
		pd3dContext->PSSetShaderResources(1, 1, &g_pShadowResourceView[0]);
		
	}

	pd3dContext->VSSetConstantBuffers(0, 1, &g_pFrameBuffer);
	pd3dContext->VSSetConstantBuffers(1, 1, &g_pObjBuffer);
	pd3dContext->VSSetShader(g_pVertexShader, nullptr, 0);

	D3D11_MAPPED_SUBRESOURCE MappedResource;
	pd3dContext->Map(g_pObjBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);
	PerObj* cbperObj = reinterpret_cast<PerObj*> (MappedResource.pData);
	cbperObj->color = color;
	cbperObj->scale = 40;
	XMStoreFloat4x4(&cbperObj->m_mWorld, g_World);
	pd3dContext->Unmap(g_pObjBuffer, 0);
	g_Plane.Render(pd3dContext, INVALID_SAMPLER_SLOT, INVALID_SAMPLER_SLOT, INVALID_SAMPLER_SLOT);
	
	XMVECTOR lupos = XMVectorSet(-g_isphereRowCount,0.5,g_isphereRowCount,1);
	float deltaDis = g_isphereRowCount * 2 / (g_isphereRowCount - 1);
	
	static const XMVECTOR multiply = XMVectorReplicate(0.5);
	static const XMVECTOR add = XMVectorReplicate(0.5f);
	//Lock();
	for (int i = 0; i < g_isphereRowCount; i++)
	{
		XMVECTOR posR = XMVectorSetZ(lupos, g_isphereRowCount - i*deltaDis);
		for (int j = 0; j < g_isphereRowCount; j++)
		{
			posR = XMVectorSetX(posR,-g_isphereRowCount+j*deltaDis);
			for (int k = 0; k < g_isphereRowCount; k++) {

				posR = XMVectorSetY(posR,  0.5+k * deltaDis);
				if (abs(XMVectorGetX(posR)) <= 2 && XMVectorGetY(posR) >= 4 && XMVectorGetY(posR) <= 8 && abs(XMVectorGetZ(posR)) <= 2)
					continue;
				XMMATRIX world = XMMatrixTranslationFromVector(posR);

				auto vColor = XMVector3Normalize(posR);
				//XMStoreFloat3(&color, XMVectorMultiplyAdd(vColor, multiply, add));

				pd3dContext->Map(g_pObjBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);
				PerObj* cbperObj = reinterpret_cast<PerObj*> (MappedResource.pData);
				XMStoreFloat3(&cbperObj->color, XMVectorMultiplyAdd(vColor, multiply, add));
				cbperObj->scale = 1;
				XMStoreFloat4x4(&cbperObj->m_mWorld, XMMatrixTranspose(world));
				pd3dContext->Unmap(g_pObjBuffer, 0);

				//g_pObjScaleVariable->SetFloat(1);
				//g_pWorldVariable->SetMatrix(reinterpret_cast<float*>(&world));
				//g_pObjColorVariable->SetFloatVector(reinterpret_cast<float*>(&color));
				//g_pViewProjectionVariable->SetMatrix(reinterpret_cast<float*>(&g_vViewProj[bIsShadow]));
				//V(g_pSceneTechnique->GetPassByIndex(0)->Apply(0, pd3dContext));

				g_Sphere.Render(pd3dContext, INVALID_SAMPLER_SLOT, INVALID_SAMPLER_SLOT, INVALID_SAMPLER_SLOT);
			}
		}
	}

	if (renderType != DynamicRender) {
		pd3dContext->PSSetConstantBuffers(1, 1, &g_pObjBuffer);
		pd3dContext->PSSetShader(g_pDynamicBoxPixelShader, nullptr, 0);
		pd3dContext->PSSetConstantBuffers(2, 1, &g_pLightBuffer);
		pd3dContext->PSSetSamplers(1, 1, &g_pSampler);
		pd3dContext->PSSetSamplers(0, 1, &g_pLinearSampler);
		pd3dContext->PSSetShaderResources(1, 1, &g_pShadowResourceView[0]);
		pd3dContext->PSSetShaderResources(2, 1, &g_pCubeSRV);
		pd3dContext->Map(g_pObjBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);
		cbperObj = reinterpret_cast<PerObj*> (MappedResource.pData);
		cbperObj->color = color;
		cbperObj->scale = 4;
		XMMATRIX world = XMMatrixTranslationFromVector(XMVectorSet(0, 4, 0, 0));
		XMStoreFloat4x4(&cbperObj->m_mWorld, XMMatrixTranspose(world));

		pd3dContext->Unmap(g_pObjBuffer, 0);
		g_Sphere.Render(pd3dContext, INVALID_SAMPLER_SLOT, INVALID_SAMPLER_SLOT, INVALID_SAMPLER_SLOT);
		//scale = 1;
	}


	//unLock();
}
void renderShadow(ID3D11DeviceContext* pd3dContext)
{
	CaluLightViewProj(0);
	pd3dContext->ClearDepthStencilView(g_pShadowDepthStencilView[0],
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0, 0);

	
	pd3dContext->RSSetViewports(1, &g_ShadowViewport[0]);
	pd3dContext->OMSetRenderTargets(0, nullptr, g_pShadowDepthStencilView[0]);
	
	D3D11_MAPPED_SUBRESOURCE MappedResource;
	pd3dContext->Map(g_pFrameBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);
	PerFrame* cbperFrame = reinterpret_cast<PerFrame*> (MappedResource.pData);

	XMStoreFloat4x4(&cbperFrame->m_mViewProj, XMMatrixTranspose(g_vViewProj[1]));
	XMStoreFloat4(&cbperFrame->m_vEyePosW, g_Camera.GetEyePt());
	pd3dContext->Unmap(g_pFrameBuffer, 0);

	renderScene(pd3dContext,shadowRender);
	
}

void renderAllScene(ID3D11DeviceContext* pd3dContext)
{
	HRESULT hr;
	V(DXUTSetupD3D11Views(pd3dContext));
	XMMATRIX mViewProjection = g_View * g_Projection;
	
	g_vViewProj[0] = mViewProjection;
	
	D3D11_MAPPED_SUBRESOURCE MappedResource;
	pd3dContext->Map(g_pFrameBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);
	PerFrame* cbperFrame = reinterpret_cast<PerFrame*> (MappedResource.pData);

	XMStoreFloat4x4(&cbperFrame->m_mViewProj, XMMatrixTranspose(g_vViewProj[0]));
	XMStoreFloat4(&cbperFrame->m_vEyePosW, g_Camera.GetEyePt());
	pd3dContext->Unmap(g_pFrameBuffer, 0);

	MapLightConstBuffer(pd3dContext);
	renderScene(pd3dContext,AllRender);
}

void renderDynamicCube(ID3D11DeviceContext* pd3dContext,int instance)
{
	pd3dContext->ClearDepthStencilView(g_pCubeDSV,
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0, 0);

	D3D11_MAPPED_SUBRESOURCE MappedResource;
	pd3dContext->Map(g_pFrameBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource);
	PerFrame* cbperFrame = reinterpret_cast<PerFrame*> (MappedResource.pData);

	XMStoreFloat4x4(&cbperFrame->m_mViewProj, XMMatrixTranspose(g_mCubeViewProj[instance]));
	XMStoreFloat4(&cbperFrame->m_vEyePosW, g_Camera.GetEyePt());
	pd3dContext->Unmap(g_pFrameBuffer, 0);
	
	pd3dContext->RSSetViewports(1, &g_CubeVP);

	pd3dContext->OMSetRenderTargets(1, &g_pCubeRTV[instance], g_pCubeDSV);

	
	//renderScene(pd3dContext);
	renderScene(pd3dContext, AllRender);

}
void CALLBACK MultiThreadScene::OnD3D11FrameRender(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext,
	double fTime, float fElapsedTime, void* pUserContext)
{
	HRESULT hr = S_OK;
	auto pRTV = DXUTGetD3D11RenderTargetView();
	pd3dImmediateContext->ClearRenderTargetView(pRTV, Colors::MidnightBlue);
	auto pDSV = DXUTGetD3D11DepthStencilView();
	pd3dImmediateContext->ClearDepthStencilView(pDSV, D3D11_CLEAR_DEPTH, 1.0, 0);

	g_View = g_Camera.GetViewMatrix();
	g_Projection = g_Camera.GetProjMatrix();
	// Update constant buffer that changes once per frame
	XMMATRIX mViewProjection = g_View * g_Projection;
	g_pViewProjectionVariable->SetMatrix(reinterpret_cast<float*>(&mViewProjection));
	
	g_pWorldVariable->SetMatrix(reinterpret_cast<float*>(&XMMatrixIdentity()));
	//g_pViewVariable->SetMatrix(reinterpret_cast<float*>(&g_View));
	g_pRasterizerVariable->GetRasterizerState(g_iRasterizerCount, &g_pRasterizerState);
	
	XMFLOAT3 tmp;
	XMStoreFloat3(&tmp, g_Camera.GetEyePt());

	g_pEyePosWVariable->SetFloatVector(reinterpret_cast<float*>(&tmp));

	//g_pLightDirVariable->SetFloatVector(g_vLightDir);
	
	//pd3dImmediateContext->ClearState();

	if (g_bMutiThread == ProcessType::MultiThread)
	{
		
		for (int i = 0; i < g_iNumSceneThread; i++)
		{
			SetEvent(g_hBeginSceneThreadEvent[i]);
		}
		WaitForMultipleObjects(g_iNumSceneThread, g_hEndSceneThreadEvent, true, INFINITE);
		for (int i = 0; i < g_iNumSceneThread; i++)
		{
			pd3dImmediateContext->ExecuteCommandList(g_pd3dSceneCommandList[i], false);
			SAFE_RELEASE(g_pd3dSceneCommandList[i]);
		}
	}
	else {
		renderShadow(pd3dImmediateContext);
		
		for(int i =0;i<6;i++)
			renderDynamicCube(pd3dImmediateContext,i);
		
		renderAllScene(pd3dImmediateContext);
		
	}
	renderSkyBox(pd3dImmediateContext);

	V(DXUTSetupD3D11Views(pd3dImmediateContext));
	FrameUI();

}


void FrameUI()
{
	// Any application code here
	ImGui::Begin("Profile");
	ImGui::RadioButton("MultiThread",(int*)&g_bMutiThread, MultiThread);
	ImGui::RadioButton("Immediate", (int*)&g_bMutiThread, Immediate);

	XMFLOAT4 pos, foucs;
	XMStoreFloat4(&pos, g_Camera.GetEyePt());
	XMStoreFloat4(&foucs, g_Camera.GetLookAtPt());
	ImGui::InputFloat4("cameraPos", (float*)&pos);
	ImGui::InputFloat4("cameraFoucs", (float*)&foucs);
	ImGui::InputFloat3("LightDir32", (float*)g_vLightDir.f);
	int index;
	g_pStateIndexVariable->GetInt(&index);
	ImGui::RadioButton("SkyboxWireframe", &index, 3);
	ImGui::RadioButton("SkyboxSolid", &index, 2);
	g_pStateIndexVariable->SetInt(index);

	g_pSceneStateIndexVariable->GetInt(&index);
	ImGui::RadioButton("SceneWireframe", &g_iRasterizerCount, 1);
	ImGui::RadioButton("SceneSolid", &g_iRasterizerCount, 0);
	g_pSceneStateIndexVariable->SetInt(index);
	ImGui::SliderInt("SphereCount", &g_isphereRowCount, 1, 10);
	
	ImGui::Image(g_pShadowResourceView[0], XMFLOAT2(100,100));

	if (ImGui::Button("ViewType"))
	{
		g_bCameraView = !g_bCameraView;

		if (g_bCameraView)
		{
			g_Camera.SetViewParams(s_Eye, s_At);

		}
	}
	ImGui::End();
}

void CALLBACK MultiThreadScene::OnD3D11DestroyDevice(void* pUserContext)
{


	CloseHandle(g_hEffectSemaphore);
	for (int i = 0; i < g_iNumSceneThread; i++)
	{
		CloseHandle(g_hBeginSceneThreadEvent[i]);
		CloseHandle(g_hEndSceneThreadEvent[i]);
		CloseHandle(g_hPerSceneRenderDeferredThread[i]);
		SAFE_RELEASE(g_pd3dSceneDeferredContext[i]);
	}
	
	DXUTGetGlobalResourceCache().OnDestroyDevice();

	g_Sphere.Destroy();
	g_Plane.Destroy();
	g_Mesh.Destroy();
	g_SkyBox.Destroy();
	g_pVertexLayout.Reset();
	SAFE_RELEASE(cubeSRV);
	SAFE_RELEASE(g_pEffect);
	
	for (int i = 0; i < g_iNumShadows; i++)
	{
		SAFE_RELEASE(g_pShadowDepthStencilView[i]);
		SAFE_RELEASE(g_pShadowResourceView[i]);
		SAFE_RELEASE(g_pShadowTex[i]);
	}
	
	g_pSkyBoxTechnique = nullptr;

	g_pWorldVariable = nullptr;
	
	g_pViewProjectionVariable = nullptr;
	
	isInit = false;
}

MultiThreadScene:: ~MultiThreadScene()
{




	DXUTGetGlobalResourceCache().OnDestroyDevice();

	g_Mesh.Destroy();
	g_pVertexLayout.Reset();

	SAFE_RELEASE(g_pEffect);

	g_pSkyBoxTechnique = nullptr;

	
	g_pWorldVariable = nullptr;

	g_pViewProjectionVariable = nullptr;

	isInit = false;

}

/*DXUT Callback*/

LRESULT CALLBACK MultiThreadScene::MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
	bool* pbNoFurtherProcessing, void* pUserContext)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
		return true;

	g_Camera.HandleMessages(hWnd, uMsg, wParam, lParam);
	return 0;//::DefWindowProc(hWnd, uMsg, wParam, lParam);

}

void CALLBACK MultiThreadScene::OnFrameMove(double fTime, float fElapsedTime, void* pUserContext)
{
	g_Camera.FrameMove(fElapsedTime);
		XMVECTOR cycle1 = XMVectorSet(-1.f,
		-1.f,
		0.20f * sinf(2.0f * (fTime + 0.0f * XM_PI)),
		0.f);
	//auto rotationM = XMMatrixRotationY(fElapsedTime);
	
	auto tmp = XMLoadFloat4(&g_cbPerLight.m_LightData[0].m_vLightDir);
	tmp = XMVector4Normalize(cycle1);
	//tmp = XMVector4Transform(tmp, rotationM);
	XMStoreFloat4(&g_cbPerLight.m_LightData[0].m_vLightDir,tmp);
	if (!g_bCameraView)
	{
		auto posTmp = XMLoadFloat4(&g_cbPerLight.m_LightData[0].m_vLightPos);

		g_Camera.SetViewParams(posTmp, posTmp + tmp * g_fLightRadius);

	}
}