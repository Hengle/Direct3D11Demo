
#include"DXUT.h"
#include"defaultScene.h"
#include <d3dx11effect.h>
using namespace DirectX;

static struct SimpleVertex
{
	XMFLOAT3 Pos;
	XMFLOAT2 Tex;
};
static struct CBChangesEveryFrame
{
	XMFLOAT4X4 mWorldViewProj;
	XMFLOAT4X4 mWorld;
	XMFLOAT4 vMeshColor;
};

static struct MaterialVars
{

	ID3DX11EffectClassInstanceVariable* pClass;
	ID3DX11EffectVectorVariable*        pColor;
	ID3DX11EffectScalarVariable*        pSpecPower;
};

/*Enum*/

static enum AmientEnum
{
	eBaseAmbient,
	eHemiAmbient
};
static int g_eAmbient;

static enum MaterialEnum
{
	ePlastic,
	ePlasticTex,
	eRough,
	eRoughTex
};
static int g_eMaterial = 0;
static std::string g_MaterialNames[4] = {
	"g_plasticMaterial",             // cPlasticMaterial              
   "g_plasticTexturedMaterial",     // cPlasticTexturedMaterial      
   "g_roughMaterial",               // cRoughMaterial        
   "g_roughTexturedMaterial",       // cRoughTexturedMaterial
};
static MaterialVars g_MaterialClasses[4];
static XMFLOAT4 g_materialColor[4] = { XMFLOAT4(1.f,0.f,0.5f,1.f),XMFLOAT4(1.f,0.f,0.5f,1.f),XMFLOAT4(0.f,0.5f,1.f,1.f) ,XMFLOAT4(0.f,0.f,1.f,1.f) };
static int g_specularPower[4] = { 255,128,6,6 };


//Misc Variable
static CModelViewerCamera          g_Camera;
static CDXUTDirectionWidget			g_LightControl;
static ComPtr<ID3D11InputLayout>          g_pVertexLayout = nullptr;
static XMMATRIX                    g_World;
static XMMATRIX                    g_View;
static XMMATRIX                    g_Projection;
static XMFLOAT4                    g_vMeshColor(0.7f, 0.7f, 0.7f, 1.0f);


static ID3D11ShaderResourceView*          cubeSRV = nullptr;
static ID3D11Buffer*						SOVertexBuffer = nullptr;
static ID3D11Buffer* nullBuffer = nullptr;

/*trigge*/
static bool						g_bHemiAmbientClass = false;
static bool                        g_bDirectLighting = false;
static bool                        g_bLightingOnly = false;
static bool                        g_bWireFrame = false;



/*Effect Declare*/
static ID3DX11Effect*					   g_pEffect = nullptr;
static ID3DX11EffectTechnique*             g_pTechnique = nullptr;
static ID3DX11EffectTechnique*             g_pOccluderTechnique = nullptr;
static ID3DX11EffectTechnique*             g_pOcculuderTestTechnique = nullptr;


static ID3DX11EffectShaderResourceVariable*g_ptxDiffuseVariable = nullptr;
static ID3DX11EffectMatrixVariable*        g_pWorldVariable = nullptr;
static ID3DX11EffectMatrixVariable*        g_pViewVariable = nullptr;
static ID3DX11EffectMatrixVariable*        g_pProjectionVariable = nullptr;
static ID3DX11EffectMatrixVariable*        g_pWVPVariable = nullptr;
static ID3DX11EffectVectorVariable*         g_pEyeDirVariable = nullptr;
static ID3DX11EffectScalarVariable*         g_pFillModeVariable = nullptr;
static ID3DX11EffectVectorVariable*		   g_pLightDirVariable = nullptr;
//static ID3DX11EffectMatrixVariable*        g_pViewVariable = nullptr;
//static ID3DX11EffectMatrixVariable*        g_pProjectionVariable = nullptr;
//static ID3DX11EffectScalarVariable*        g_pWavinessVariable = nullptr;
//static ID3DX11EffectScalarVariable*        g_pTimeVariable = nullptr;
//static ID3DX11EffectVectorVariable*		   g_pLightDirVariable = nullptr;
static ID3DX11EffectShaderResourceVariable*g_ptxEnvVariable = nullptr;

static float g_pModeWaviness;
static CDXUTSDKMesh                        g_Mesh;

/*Effect Interface Variable */
static ID3DX11EffectInterfaceVariable*		g_pIAmbientLight = nullptr;
static ID3DX11EffectInterfaceVariable*		g_pIDirectionLight = nullptr;
static ID3DX11EffectInterfaceVariable*		g_pIEnvironmentLight = nullptr;
static ID3DX11EffectInterfaceVariable*		g_pIMaterial = nullptr;

/*Effect Class Variable with member data*/
static ID3DX11EffectClassInstanceVariable*	g_pAmbientLightClass = nullptr;
static ID3DX11EffectVectorVariable*	         g_pAmbientLightColor = nullptr;
static ID3DX11EffectScalarVariable*         g_pAmbientLightEnable = nullptr;
static ID3DX11EffectClassInstanceVariable*  g_pHemiAmbientLightClass = nullptr;
static ID3DX11EffectVectorVariable*         g_pHemiAmbientLightColor = nullptr;
static ID3DX11EffectScalarVariable*         g_pHemiAmbientLightEnable = nullptr;
static ID3DX11EffectVectorVariable*         g_pHemiAmbientLightGroundColor = nullptr;
static ID3DX11EffectVectorVariable*         g_pHemiAmbientLightDirUp = nullptr;
static ID3DX11EffectClassInstanceVariable*  g_pDirectionalLightClass = nullptr;
static ID3DX11EffectVectorVariable*         g_pDirectionalLightColor = nullptr;
static ID3DX11EffectScalarVariable*         g_pDirectionalLightEnable = nullptr;
static ID3DX11EffectVectorVariable*         g_pDirectionalLightDir = nullptr;
static ID3DX11EffectClassInstanceVariable*  g_pEnvironmentLightClass = nullptr;
static ID3DX11EffectVectorVariable*         g_pEnvironmentLightColor = nullptr;
static ID3DX11EffectScalarVariable*		   g_pEnvironmentLightEnable = nullptr;


static ID3D11Predicate*						g_pOccluderPredicate[6];

static CDXUTSDKMesh                        g_CityMesh;
static CDXUTSDKMesh                        g_OccluderMesh;
static CDXUTSDKMesh                        g_HeavyMesh;
static CDXUTSDKMesh                        g_ColumnMesh;

/*Effect Enum*/
static D3D11_INPUT_ELEMENT_DESC layout[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};

static void FrameUI();
static HRESULT UpdateEffect(ID3D11Device* pd3dDevice);
//static void DebugUI(ID3D11DeviceContext* pd3dImmediateContext);
std::string CityScene::getName()
{
	return sceneName;
}
HRESULT CityScene::initScene(ID3D11Device* pd3dDevice)
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
	V_RETURN(DXUTFindDXSDKMediaFileCch(str, MAX_PATH, L"shader//city.fx"));
	ID3DBlob* errorMsg = nullptr;
	hr = D3DX11CompileEffectFromFile(str, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, dwShaderFlags, D3DCOMPILE_EFFECT_ALLOW_SLOW_OPS, pd3dDevice, &g_pEffect, &errorMsg);
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


	g_LightControl.SetLightDirection(XMFLOAT3(-1, 1, -1));
	g_LightControl.SetRadius(300);

	// Obtain the technique
	g_pTechnique = g_pEffect->GetTechniqueByName("CityEffect");
	g_pOccluderTechnique = g_pEffect->GetTechniqueByName("OcculuderTop");
	g_pOcculuderTestTechnique = g_pEffect->GetTechniqueByName("OccluderTest");
	
	// Obtain the variables
	g_ptxDiffuseVariable = g_pEffect->GetVariableByName("g_txDiffuse")->AsShaderResource();

	g_pFillModeVariable = g_pEffect->GetVariableByName("g_fillMode")->AsScalar();
	//g_pEyeDirVariable = g_pEffect->GetVariableByName("g_vEyeDir")->AsVector();
	g_ptxDiffuseVariable = g_pEffect->GetVariableByName("g_txDiffuse")->AsShaderResource();
	g_pWorldVariable = g_pEffect->GetVariableByName("World")->AsMatrix();
	g_pViewVariable = g_pEffect->GetVariableByName("View")->AsMatrix();
	g_pProjectionVariable = g_pEffect->GetVariableByName("Projection")->AsMatrix();
	g_pLightDirVariable = g_pEffect->GetVariableByName("vLightDir")->AsVector();
	


	D3D11_QUERY_DESC qd;
	qd.MiscFlags = D3D11_QUERY_MISC_PREDICATEHINT;
	qd.Query = D3D11_QUERY_OCCLUSION_PREDICATE;
	//为每个仪器都创建一个predicate
	for (int i = 0; i < 6; i++)
		V_RETURN(pd3dDevice->CreatePredicate(&qd, &g_pOccluderPredicate[i]));
	

	// Define the input layout


	UINT numElements = ARRAYSIZE(layout);

	// Create the input layout
	D3DX11_PASS_SHADER_DESC VsPassDesc;
	/*D3DX11_EFFECT_SHADER_DESC VsDesc;
	auto pass = g_pTechnique->GetPassByIndex(0);
	V_RETURN(pass->GetVertexShaderDesc(&VsPassDesc));
	V_RETURN(VsPassDesc.pShaderVariable->GetShaderDesc(VsPassDesc.ShaderIndex, &VsDesc))
		V_RETURN(pd3dDevice->CreateInputLayout(layout, numElements, VsDesc.pBytecode,
			VsDesc.BytecodeLength, g_pVertexLayout.GetAddressOf()));*/

	D3DX11_PASS_DESC PassDesc;
	V_RETURN(g_pTechnique->GetPassByIndex(0)->GetDesc(&PassDesc));
	V_RETURN(pd3dDevice->CreateInputLayout(layout, numElements, PassDesc.pIAInputSignature,
		PassDesc.IAInputSignatureSize, g_pVertexLayout.GetAddressOf()));

	// Set the input layout
	pd3dImmediateContext->IASetInputLayout(g_pVertexLayout.Get());

	// Load the mesh

	V_RETURN(g_CityMesh.Create(pd3dDevice, L"MicroscopeCity\\occcity.sdkmesh",false));
	V_RETURN(g_HeavyMesh.Create(pd3dDevice, L"MicroscopeCity\\scanner.sdkmesh",false));
	V_RETURN(g_ColumnMesh.Create(pd3dDevice, L"MicroscopeCity\\column.sdkmesh",false));
	V_RETURN(g_OccluderMesh.Create(pd3dDevice, L"MicroscopeCity\\occluder.sdkmesh",false));


	

	


	// Initialize the world matrices
	g_World = XMMatrixIdentity();

	// Setup the camera's view parameters
	static const XMVECTORF32 s_Eye = { 0.0f, 3.0f, -20.0f, 0.f };
	static const XMVECTORF32 s_At = { 0.0f, 1.0f, 0.0f, 0.f };
	g_Camera.SetViewParams(s_Eye, s_At);



	return S_OK;
}

HRESULT CALLBACK CityScene::OnD3D11CreateDevice(ID3D11Device* pd3dDevice, const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc,
	void* pUserContext)
{

	return initScene(pd3dDevice);
}




HRESULT CALLBACK CityScene::OnD3D11ResizedSwapChain(ID3D11Device* pd3dDevice, IDXGISwapChain* pSwapChain,
	const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext)
{
	float FAspect = static_cast<float>(pBackBufferSurfaceDesc->Width) / static_cast<float>(pBackBufferSurfaceDesc->Height);
	g_Projection = XMMatrixPerspectiveFovLH(XM_PI*0.25f, FAspect, 0.1, 100.f);
	g_Camera.SetProjParams(XM_PI / 4, FAspect, 0.1f, 1000.0f);
	g_Camera.SetWindow(pBackBufferSurfaceDesc->Width, pBackBufferSurfaceDesc->Height);
	g_Camera.SetButtonMasks(MOUSE_LEFT_BUTTON, MOUSE_WHEEL, MOUSE_MIDDLE_BUTTON);

	return S_OK;
}
void CALLBACK CityScene::OnD3D11FrameRender(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext,
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
	XMMATRIX mWorldViewProjection = g_World * g_View*g_Projection;

	g_pWorldVariable->SetMatrix(reinterpret_cast<float*>(&g_World));
	g_pViewVariable->SetMatrix(reinterpret_cast<float*>(&g_View));
	g_pProjectionVariable->SetMatrix(reinterpret_cast<float*>(&g_Projection));
//	g_pWVPVariable->SetMatrix(reinterpret_cast<float*>(&mWorldViewProjection));

	XMVECTOR lightDir = g_LightControl.GetLightDirection();

	V(g_LightControl.OnRender(Colors::Yellow, g_View, g_Projection, g_Camera.GetEyePt()));

	//g_pViewVariable->SetMatrix(reinterpret_cast<float*>(&g_View));
	//g_pProjectionVariable->SetMatrix(reinterpret_cast<float*>(&g_Projection));
	//g_pTimeVariable->SetFloat((float)fTime);
	//g_pWavinessVariable->SetFloat(g_pModeWaviness);
	XMFLOAT3 LightDirF;
	XMStoreFloat3(&LightDirF, lightDir);
	g_pLightDirVariable->SetFloatVector(reinterpret_cast<float*>(&LightDirF));
	pd3dImmediateContext->IASetInputLayout(g_pVertexLayout.Get());





	if (g_bWireFrame)
		g_pFillModeVariable->SetInt(1);
	else
		g_pFillModeVariable->SetInt(0);

	D3DX11_TECHNIQUE_DESC techDesc;

	V(g_pTechnique->GetDesc(&techDesc));


	//DebugUI(pd3dImmediateContext);
	g_pTechnique->GetPassByIndex(0)->Apply(0, pd3dImmediateContext);
	
	g_CityMesh.Render(pd3dImmediateContext,0);
	g_ColumnMesh.Render(pd3dImmediateContext, 0);
	

	for (int i = 0; i < 6; i++)
	{
		auto tmpM = XMMatrixRotationY(i*(XM_2PI / 6.f));
		g_pWorldVariable->SetMatrix(reinterpret_cast<float*>(&tmpM));
		
		pd3dImmediateContext->Begin(g_pOccluderPredicate[i]);
		g_pOcculuderTestTechnique->GetPassByIndex(0)->Apply(0, pd3dImmediateContext);
		g_OccluderMesh.Render(pd3dImmediateContext, 0);
		pd3dImmediateContext->End(g_pOccluderPredicate[i]);

		pd3dImmediateContext->SetPredication(g_pOccluderPredicate[i], false);
		
		g_pTechnique->GetPassByIndex(0)->Apply(0, pd3dImmediateContext);
		g_HeavyMesh.Render(pd3dImmediateContext, 0);
		pd3dImmediateContext->SetPredication(nullptr, false);
	}



	
	




	FrameUI();

	V(UpdateEffect(pd3dDevice));


}

HRESULT UpdateEffect(ID3D11Device* pd3dDevice)
{
	HRESULT hr = S_OK;

	return hr;
}
void FrameUI()
{
	// Any application code here

	XMFLOAT4 pos, foucs;
	XMStoreFloat4(&pos, g_Camera.GetEyePt());
	XMStoreFloat4(&foucs, g_Camera.GetLookAtPt());
	ImGui::InputFloat4("cameraPos", (float*)&pos);
	ImGui::InputFloat4("cameraFoucs", (float*)&foucs);
	XMVECTORF32 tmp;
	tmp.v = g_LightControl.GetLightDirection();
	ImGui::InputFloat3("LightDir", (float*)tmp.f);

	

	

	ImGui::Checkbox("wireframe", &g_bWireFrame);

}

void CALLBACK CityScene::OnD3D11DestroyDevice(void* pUserContext)
{

	DXUTGetGlobalResourceCache().OnDestroyDevice();

	g_Mesh.Destroy();
	g_CityMesh.Destroy();
	g_OccluderMesh.Destroy();
	g_ColumnMesh.Destroy();
	g_HeavyMesh.Destroy();
	g_pVertexLayout.Reset();
	SAFE_RELEASE(cubeSRV);
	SAFE_RELEASE(g_pEffect);
	SAFE_RELEASE(SOVertexBuffer);
	for(int i = 0;i<6;i++)
		SAFE_RELEASE(g_pOccluderPredicate[i]);

	g_pTechnique = nullptr;

	g_ptxDiffuseVariable = nullptr;

	g_pWorldVariable = nullptr;

	isInit = false;
}

CityScene:: ~CityScene()
{




	DXUTGetGlobalResourceCache().OnDestroyDevice();

	g_Mesh.Destroy();
	g_pVertexLayout.Reset();

	SAFE_RELEASE(g_pEffect);

	g_pTechnique = nullptr;

	g_ptxDiffuseVariable = nullptr;

	g_pWorldVariable = nullptr;

	isInit = false;

}

/*DXUT Callback*/

LRESULT CALLBACK CityScene::MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
	bool* pbNoFurtherProcessing, void* pUserContext)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
		return true;

	g_Camera.HandleMessages(hWnd, uMsg, wParam, lParam);
	g_LightControl.HandleMessages(hWnd, uMsg, wParam, lParam);
	return 0;//::DefWindowProc(hWnd, uMsg, wParam, lParam);

}

void CALLBACK CityScene::OnFrameMove(double fTime, float fElapsedTime, void* pUserContext)
{
	g_Camera.FrameMove(fElapsedTime);



	// Modify the color
	g_vMeshColor.x = (sinf((float)fTime * 1.0f) + 1.0f) * 0.5f;
	g_vMeshColor.y = (cosf((float)fTime * 3.0f) + 1.0f) * 0.5f;
	g_vMeshColor.z = (sinf((float)fTime * 5.0f) + 1.0f) * 0.5f;



}