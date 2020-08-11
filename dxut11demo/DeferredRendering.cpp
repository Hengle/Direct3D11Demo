
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
static const  int lightCount = 3;
struct Light
{
	XMFLOAT3 	g_vLightDir;
	float		atten;
	XMFLOAT3	g_vLightColor;
	float		radius;
}g_Lights[lightCount];
static CModelViewerCamera          g_Camera;

static ComPtr<ID3D11InputLayout>          g_pVertexLayout = nullptr;
static XMMATRIX                    g_World;
static XMMATRIX                    g_View;
static XMMATRIX                    g_Projection;
static XMFLOAT4                    g_vMeshColor(0.7f, 0.7f, 0.7f, 1.0f);




static ID3D11Texture2D*						g_pCubeTex = nullptr;
static ID3D11Buffer*						SOVertexBuffer = nullptr;
static ID3D11Buffer* nullBuffer = nullptr;
/*Effect Declare*/
static ID3DX11Effect*					   g_pEffect = nullptr;
static ID3DX11EffectTechnique*             g_pTechnique = nullptr;
static ID3DX11EffectShaderResourceVariable*g_ptxBaseColorVariable = nullptr;
static ID3DX11EffectShaderResourceVariable*g_ptxEmissionVariable = nullptr;
static ID3DX11EffectShaderResourceVariable*g_ptxMetalnessVariable = nullptr;
static ID3DX11EffectShaderResourceVariable*g_ptxNormalVariable = nullptr;
static ID3DX11EffectShaderResourceVariable*g_ptxOcclusionVariable = nullptr;
static ID3DX11EffectShaderResourceVariable*g_ptxRoughnessVariable = nullptr;

static ID3DX11EffectMatrixVariable*        g_pWorldVariable = nullptr;
static ID3DX11EffectMatrixVariable*        g_pViewVariable = nullptr;
static ID3DX11EffectMatrixVariable*        g_pViewProjectionVariable = nullptr;
static ID3DX11EffectScalarVariable*        g_pWavinessVariable = nullptr;
static ID3DX11EffectScalarVariable*        g_pTimeVariable = nullptr;
static ID3DX11EffectVectorVariable*		   g_pLightDirVariable = nullptr;
static ID3DX11EffectVectorVariable*		   g_pLightColorVariable = nullptr;
static ID3DX11EffectVectorVariable*		   g_pEyePosWVariable = nullptr;
static ID3DX11EffectConstantBuffer*			g_pLightsVariable = nullptr;
static ID3DX11EffectShaderResourceVariable*g_ptxEnvVariable = nullptr;

ID3D11Buffer* g_pcbLightBuffer = nullptr;

static float g_pModeWaviness;
static CDXUTSDKMesh							g_Mesh;
static ID3D11ShaderResourceView*			g_pBaseColorSRV;
static ID3D11ShaderResourceView*			g_pEmissionSRV;
static ID3D11ShaderResourceView*			g_pMetalnessSRV;
static ID3D11ShaderResourceView*			g_pNormalSRV;
static ID3D11ShaderResourceView*			g_pOcclusionSRV;
static ID3D11ShaderResourceView*			g_pRoughnessSRV;
static ID3D11ShaderResourceView*            g_ptxCubeSRV = nullptr;
static std::string EffectName[] = { "PBREffect" };
static int EffectIndex = 0;
static int preEffectIndex = 0;
static D3D11_INPUT_ELEMENT_DESC layout[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TANGENT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
};


static void FrameUI();
static HRESULT UpdateEffect(ID3D11Device* pd3dDevice);

std::string DeferredRendering::getName()
{
	return sceneName;
}
HRESULT DeferredRendering::initScene(ID3D11Device* pd3dDevice)
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
	V_RETURN(DXUTFindDXSDKMediaFileCch(str, MAX_PATH, L"shader//PBR.fx"));
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
	V_RETURN(DXUTCompileFromFile(L"shader//PBR.fx", nullptr, "none", "fx_5_0", dwShaderFlags, 0, &pEffectBuffer));
	hr = D3DX11CreateEffectFromMemory(pEffectBuffer->GetBufferPointer(), pEffectBuffer->GetBufferSize(), 0, pd3dDevice, &g_pEffect);
	SAFE_RELEASE(pEffectBuffer);
	if (FAILED(hr))
		return hr;

#endif

	// Obtain the technique
	g_pTechnique = g_pEffect->GetTechniqueByName(EffectName[EffectIndex].c_str());

	// Obtain the variables
	g_ptxBaseColorVariable	= g_pEffect->GetVariableByName("g_txBaseColor")->AsShaderResource();
	g_ptxEmissionVariable	= g_pEffect->GetVariableByName("g_txEmission")->AsShaderResource();
	g_ptxMetalnessVariable	= g_pEffect->GetVariableByName("g_txMetalness")->AsShaderResource();
	g_ptxNormalVariable		= g_pEffect->GetVariableByName("g_txNormal")->AsShaderResource();
	g_ptxRoughnessVariable	= g_pEffect->GetVariableByName("g_txRoughness")->AsShaderResource();
	g_ptxOcclusionVariable  = g_pEffect->GetVariableByName("g_txOcclusion")->AsShaderResource();
	g_ptxEnvVariable		= g_pEffect->GetVariableByName("g_txEnv")->AsShaderResource();
	g_pLightsVariable		= g_pEffect->GetConstantBufferByName("PerLight")->AsConstantBuffer();

	
	//g_pWorldVariable = g_pEffect->GetVariableByName("World")->AsMatrix();
	g_pViewProjectionVariable = g_pEffect->GetVariableByName("g_mViewProj")->AsMatrix();

	g_pEyePosWVariable = g_pEffect->GetVariableByName("g_vEyePosW")->AsVector();


	V_RETURN(DXUTCreateShaderResourceViewFromFile(pd3dDevice, L"Lobby\\LobbyCube.dds", &g_ptxCubeSRV));
	V_RETURN(DXUTCreateShaderResourceViewFromFile(pd3dDevice, L"resource\\helmet\\helmet_basecolor.png",&g_pBaseColorSRV));
	V_RETURN(DXUTCreateShaderResourceViewFromFile(pd3dDevice, L"resource\\helmet\\helmet_emission.png", &g_pEmissionSRV));
	V_RETURN(DXUTCreateShaderResourceViewFromFile(pd3dDevice, L"resource\\helmet\\helmet_metalness.png", &g_pMetalnessSRV));
	V_RETURN(DXUTCreateShaderResourceViewFromFile(pd3dDevice, L"resource\\helmet\\helmet_normal.png", &g_pNormalSRV));
	V_RETURN(DXUTCreateShaderResourceViewFromFile(pd3dDevice, L"resource\\helmet\\helmet_occlusion.png", &g_pOcclusionSRV));
	V_RETURN(DXUTCreateShaderResourceViewFromFile(pd3dDevice, L"resource\\helmet\\helmet_roughness.png", &g_pRoughnessSRV));
	

		 
//	g_ptxEnvVariable->SetResource(cubeSRV);

	g_ptxBaseColorVariable->SetResource(g_pBaseColorSRV);
	g_ptxEmissionVariable->SetResource(g_pEmissionSRV);
	g_ptxMetalnessVariable->SetResource(g_pMetalnessSRV);
	g_ptxNormalVariable->SetResource(g_pNormalSRV);
	g_ptxOcclusionVariable->SetResource(g_pOcclusionSRV);
	g_ptxRoughnessVariable->SetResource(g_pRoughnessSRV);
	g_ptxEnvVariable->SetResource(g_ptxCubeSRV);

	
	// Define the input layout

	UINT numElements = ARRAYSIZE(layout);

	// Create the input layout
	D3DX11_PASS_DESC PassDesc;
 	V_RETURN(g_pTechnique->GetPassByIndex(0)->GetDesc(&PassDesc));
	V_RETURN(pd3dDevice->CreateInputLayout(layout, numElements, PassDesc.pIAInputSignature,
		PassDesc.IAInputSignatureSize, g_pVertexLayout.GetAddressOf()));

	// Set the input layout
	pd3dImmediateContext->IASetInputLayout(g_pVertexLayout.Get());

	// Load the mesh

	V_RETURN(g_Mesh.Create(pd3dDevice, L"resource\\helmet\\helmet2.sdkmesh"));


	// create user d3dbuffer
	

	
	// Initialize the world matrices
	g_World = XMMatrixIdentity();

	// Setup the camera's view parameters
	static const XMVECTORF32 s_Eye = { 0.0f, 3.0f, -1.0f, 0.f };
	static const XMVECTORF32 s_At = { 0.0f, 1.0f, 0.0f, 0.f };
	g_Camera.SetViewParams(s_Eye, s_At);

	XMVECTOR vligdir;
	vligdir = XMVectorSet(0.9, 0.9, 0.4,0);
	vligdir = XMVector3Normalize(vligdir);
	XMStoreFloat3(&g_Lights[0].g_vLightDir, vligdir);
	g_Lights[0].g_vLightColor = { 0.5,0.5,0.3 };

	vligdir = XMVectorSet(-1, 0.5, -1, 0);
	vligdir = XMVector3Normalize(vligdir);
	XMStoreFloat3(&g_Lights[1].g_vLightDir, vligdir);
	g_Lights[1].g_vLightColor = { 0.3,0.3,0.3 };

	vligdir = XMVectorSet(1, 0, 1, 0);
	vligdir = XMVector3Normalize(vligdir);
	XMStoreFloat3(&g_Lights[2].g_vLightDir, vligdir);
	g_Lights[2].g_vLightColor = { 0.3f,0.3f,0.3f };


	
	D3D11_BUFFER_DESC desc;
	g_pLightsVariable->GetConstantBuffer(&g_pcbLightBuffer);
	g_pcbLightBuffer->GetDesc(&desc);
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.Usage = D3D11_USAGE_DYNAMIC;
	SAFE_RELEASE(g_pcbLightBuffer);
	pd3dDevice->CreateBuffer(&desc, nullptr, &g_pcbLightBuffer);
	g_pLightsVariable->SetConstantBuffer(g_pcbLightBuffer);
	
	return S_OK;
}

HRESULT CALLBACK DeferredRendering::OnD3D11CreateDevice(ID3D11Device* pd3dDevice, const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc,
	void* pUserContext)
{

	return initScene(pd3dDevice);
}

HRESULT CALLBACK DeferredRendering::OnD3D11ResizedSwapChain(ID3D11Device* pd3dDevice, IDXGISwapChain* pSwapChain,
	const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext)
{
	float FAspect = static_cast<float>(pBackBufferSurfaceDesc->Width) / static_cast<float>(pBackBufferSurfaceDesc->Height);
	g_Projection = XMMatrixPerspectiveFovLH(XM_PI*0.25f, FAspect, 0.1, 100.f);
	g_Camera.SetProjParams(XM_PI / 4, FAspect, 0.1f, 1000.0f);
	g_Camera.SetWindow(pBackBufferSurfaceDesc->Width, pBackBufferSurfaceDesc->Height);
	g_Camera.SetButtonMasks(MOUSE_LEFT_BUTTON, MOUSE_WHEEL, MOUSE_MIDDLE_BUTTON);

	return S_OK;
}
void CALLBACK DeferredRendering::OnD3D11FrameRender(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext,
	double fTime, float fElapsedTime, void* pUserContext)
{
	auto pRTV = DXUTGetD3D11RenderTargetView();
	pd3dImmediateContext->ClearRenderTargetView(pRTV, Colors::MidnightBlue);
	auto pDSV = DXUTGetD3D11DepthStencilView();
	pd3dImmediateContext->ClearDepthStencilView(pDSV, D3D11_CLEAR_DEPTH, 1.0, 0);

	g_View = g_Camera.GetViewMatrix();
	g_Projection = g_Camera.GetProjMatrix();
	// Update constant buffer that changes once per frame
	XMMATRIX mViewProjection =  g_View*g_Projection;

	//g_pWorldVariable->SetMatrix(reinterpret_cast<float*>(&g_World));

	g_pViewProjectionVariable->SetMatrix(reinterpret_cast<float*>(&mViewProjection));


	
	pd3dImmediateContext->IASetInputLayout(g_pVertexLayout.Get());



	UINT Strides[1];
	UINT Offsets[1];
	ID3D11Buffer* pVB[1];
	pVB[0] = g_Mesh.GetVB11(0, 0);
	Strides[0] = (UINT)g_Mesh.GetVertexStride(0, 0);
	Offsets[0] = 0;
	pd3dImmediateContext->IASetVertexBuffers(0, 1, pVB, Strides, Offsets);
	pd3dImmediateContext->IASetIndexBuffer(g_Mesh.GetIB11(0), g_Mesh.GetIBFormat11(0), 0);


	HRESULT hr;


	D3D11_MAPPED_SUBRESOURCE mappedResource;
	pd3dImmediateContext->Map(g_pcbLightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	memcpy_s(mappedResource.pData, sizeof(g_Lights), &g_Lights, sizeof(g_Lights));
	pd3dImmediateContext->Unmap(g_pcbLightBuffer, 0);

		g_pTechnique->GetPassByIndex(0)->Apply(0, pd3dImmediateContext);
		g_Mesh.Render(pd3dImmediateContext);
	
		D3D11_SUBRESOURCE_DATA data;
		data.pSysMem = g_Lights;
	






	FrameUI();

	V(UpdateEffect(pd3dDevice));


}


HRESULT UpdateEffect(ID3D11Device* pd3dDevice)
{
	HRESULT hr;
	if (preEffectIndex == EffectIndex)return S_OK;
	preEffectIndex = EffectIndex;

	g_pTechnique = g_pEffect->GetTechniqueByName(EffectName[EffectIndex].c_str());

	D3DX11_PASS_DESC PassDesc;
	V_RETURN(g_pTechnique->GetPassByIndex(0)->GetDesc(&PassDesc));

	V_RETURN(pd3dDevice->CreateInputLayout(layout, ARRAYSIZE(layout), PassDesc.pIAInputSignature,
		PassDesc.IAInputSignatureSize, &g_pVertexLayout));
	return hr;
}
void FrameUI()
{
	// Any application code here
	ImGui::Begin("Mesh");
	ImGui::Text("MeshName %s", "tiny.sdkmesh");
	XMFLOAT4 pos, foucs;
	XMStoreFloat4(&pos, g_Camera.GetEyePt());
	XMStoreFloat4(&foucs, g_Camera.GetLookAtPt());
	ImGui::InputFloat4("cameraPos", (float*)&pos);
	ImGui::InputFloat4("cameraFoucs", (float*)&foucs);
	//ImGui::InputFloat3("LightDir32", (float*)g_vLightDir.f);

	ImGui::End();
	ImGui::SliderFloat("Wavness", &g_pModeWaviness, 0, 50);

	ImGui::Text("BaseColor");
	ImGui::Image(g_pBaseColorSRV, XMFLOAT2(50, 50));
	ImGui::Text("Emission");
	ImGui::Image(g_pEmissionSRV, XMFLOAT2(50, 50));
	ImGui::Text("Metalness");
	ImGui::Image(g_pMetalnessSRV, XMFLOAT2(50, 50));
	ImGui::Text("Occlusion");
	ImGui::Image(g_pOcclusionSRV, XMFLOAT2(50, 50));
	ImGui::Text("Roughness");
	ImGui::Image(g_pRoughnessSRV, XMFLOAT2(50, 50));

	/*ImGui::Begin("Effect");
	for (int i = 0; i < 1; i++)
		ImGui::RadioButton(EffectName[i].c_str(), &EffectIndex, i);
	ImGui::End();*/
}

void CALLBACK DeferredRendering::OnD3D11DestroyDevice(void* pUserContext)
{



	

	DXUTGetGlobalResourceCache().OnDestroyDevice();

	g_Mesh.Destroy();
	SAFE_RELEASE(g_pBaseColorSRV);
	SAFE_RELEASE(g_pMetalnessSRV);
	SAFE_RELEASE(g_pNormalSRV);
	SAFE_RELEASE(g_pRoughnessSRV);
	SAFE_RELEASE(g_pEmissionSRV);
	SAFE_RELEASE(g_pOcclusionSRV);

	g_pVertexLayout.Reset();
	SAFE_RELEASE(g_ptxCubeSRV);
	SAFE_RELEASE(g_pEffect);
	SAFE_RELEASE(SOVertexBuffer);
	g_pTechnique = nullptr;
	SAFE_RELEASE(g_pcbLightBuffer);

	g_pWorldVariable = nullptr;
	g_pViewVariable = nullptr;

	g_pWavinessVariable = nullptr;
	g_pTimeVariable = nullptr;
	isInit = false;
}

DeferredRendering:: ~DeferredRendering()
{




	DXUTGetGlobalResourceCache().OnDestroyDevice();

	g_Mesh.Destroy();
	g_pVertexLayout.Reset();

	SAFE_RELEASE(g_pEffect);

	g_pTechnique = nullptr;



	g_pWorldVariable = nullptr;
	g_pViewVariable = nullptr;

	g_pWavinessVariable = nullptr;
	g_pTimeVariable = nullptr;
	isInit = false;

}

/*DXUT Callback*/

LRESULT CALLBACK DeferredRendering::MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
	bool* pbNoFurtherProcessing, void* pUserContext)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
		return true;

	g_Camera.HandleMessages(hWnd, uMsg, wParam, lParam);
	return 0;//::DefWindowProc(hWnd, uMsg, wParam, lParam);

}

void CALLBACK DeferredRendering::OnFrameMove(double fTime, float fElapsedTime, void* pUserContext)
{
	g_Camera.FrameMove(fElapsedTime);



	// Modify the color
	g_vMeshColor.x = (sinf((float)fTime * 1.0f) + 1.0f) * 0.5f;
	g_vMeshColor.y = (cosf((float)fTime * 3.0f) + 1.0f) * 0.5f;
	g_vMeshColor.z = (sinf((float)fTime * 5.0f) + 1.0f) * 0.5f;

	float radians = XMConvertToRadians(float(600 * fElapsedTime));
	auto tmpM = XMMatrixRotationY(XMConvertToRadians(radians));
	XMFLOAT3 tmp;
	XMStoreFloat3(&tmp, g_Camera.GetEyePt());
	g_pEyePosWVariable->SetFloatVector(reinterpret_cast<float*>(&tmp));
	//g_vLightDir.v= XMVector3Normalize(XMVector3Transform(g_vLightDir, tmpM));
	//XMVector3Normalize(g_vLightDir);
	//g_pLightDirVariable->SetFloatVector(g_vLightDir);
}