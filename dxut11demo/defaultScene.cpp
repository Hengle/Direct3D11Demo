
#include"DXUT.h"
#include"defaultScene.h"
#include<d3dx11effect.h>
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
static CModelViewerCamera          g_Camera;
static ComPtr<ID3D11VertexShader>         g_pBoxVertexShader = nullptr;
static ComPtr<ID3D11PixelShader>          g_pBoxPixelShader = nullptr;
static ComPtr<ID3D11InputLayout>          g_pVertexLayout = nullptr;
static ComPtr<ID3D11Buffer>               g_pVertexBuffer = nullptr;
static ComPtr<ID3D11Buffer>               g_pIndexBuffer = nullptr;
static ComPtr<ID3D11Buffer>               g_pCBChangesEveryFrame = nullptr;
static ComPtr<ID3D11ShaderResourceView>   g_pTextureRV = nullptr;
static ComPtr<ID3D11SamplerState>         g_pSamplerLinear = nullptr;
static XMMATRIX                    g_World;
static XMMATRIX                    g_View;
static XMMATRIX                    g_Projection;
static XMFLOAT4                    g_vMeshColor(0.7f, 0.7f, 0.7f, 1.0f);

static CDXUTSDKMesh                        g_Mesh;

static ComPtr<ID3DX11Effect> effect;
std::string defaultScene::getName()
{
	return sceneName;
}
HRESULT defaultScene::initScene(ID3D11Device* pd3dDevice)
{
	
	isInit = true;
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

	HRESULT hr = S_OK;
	ID3DBlob* pVSBlob = nullptr;
	V_RETURN(DXUTCompileFromFile(L"shader\\Cube.fx", nullptr, "VS", "vs_4_0", dwShaderFlags, 0, &pVSBlob));
	hr = pd3dDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &g_pBoxVertexShader);
	if (FAILED(hr))
	{
		SAFE_RELEASE(pVSBlob);
		return hr;
	}
	
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	UINT numElements = ARRAYSIZE(layout);
	hr = pd3dDevice->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), &g_pVertexLayout);
	SAFE_RELEASE(pVSBlob);
	if (FAILED(hr))
		return hr;

	pd3dImmediateContext->IASetInputLayout(g_pVertexLayout.Get());
	ID3DBlob* pPSBlob = nullptr;
	V_RETURN(DXUTCompileFromFile(L"shader\\Cube.fx", nullptr, "PS", "ps_4_0", dwShaderFlags, 0, &pPSBlob));

	// Create the pixel shader
	hr = pd3dDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &g_pBoxPixelShader);
	SAFE_RELEASE(pPSBlob);
	if (FAILED(hr))
		return hr;

	// Create vertex buffer

	SimpleVertex vertices[] =
	{
		{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },

		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },

		{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) },

		{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 0.0f) },

		{ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3(1.0f, -1.0f, -1.0f), XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3(1.0f, 1.0f, -1.0f), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) },

		{ XMFLOAT3(-1.0f, -1.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) },
		{ XMFLOAT3(1.0f, -1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT2(1.0f, 0.0f) },
	};

	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(SimpleVertex) * 24;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData = {};
	InitData.pSysMem = vertices;
	V_RETURN(pd3dDevice->CreateBuffer(&bd, &InitData, &g_pVertexBuffer));

	UINT stride = sizeof(SimpleVertex);
	UINT offset = 0;


	// Create index buffer
	DWORD indices[] =
	{
		3,1,0,
		2,1,3,

		6,4,5,
		7,4,6,

		11,9,8,
		10,9,11,

		14,12,13,
		15,12,14,

		19,17,16,
		18,17,19,

		22,20,21,
		23,20,22
	};

	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(DWORD) * 36;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	InitData.pSysMem = indices;
	V_RETURN(pd3dDevice->CreateBuffer(&bd, &InitData, &g_pIndexBuffer));

	// Set index buffer

	// Set primitive topology


	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	bd.ByteWidth = sizeof(CBChangesEveryFrame);
	V_RETURN(pd3dDevice->CreateBuffer(&bd, nullptr, &g_pCBChangesEveryFrame));



	// Initialize the world matrices
	g_World = XMMatrixIdentity();

	// Initialize the view matrix
	static const XMVECTORF32 s_Eye = { 0.0f, 3.0f, -6.0f, 0.f };
	static const XMVECTORF32 s_At = { 0.0f, 1.0f, 0.0f, 0.f };
	//static const XMVECTORF32 s_Up = { 0.0f, 1.0f, 0.0f, 0.f };
	//g_View = XMMatrixLookAtLH(s_Eye, s_At, s_Up);

	g_Camera.SetViewParams(s_Eye, s_At);
	// Load the Texture
	V_RETURN(DXUTCreateShaderResourceViewFromFile(pd3dDevice, L"resource\\seafloor.dds", &g_pTextureRV));

	// Create the sample state
	D3D11_SAMPLER_DESC sampDesc = {};
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	V_RETURN(pd3dDevice->CreateSamplerState(&sampDesc, &g_pSamplerLinear));
	V_RETURN(g_Mesh.Create(pd3dDevice, L"resource\\tiny.sdkmesh"));

	return S_OK;
}

HRESULT CALLBACK defaultScene::OnD3D11CreateDevice(ID3D11Device* pd3dDevice, const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc,
	void* pUserContext)
{
	
	return initScene(pd3dDevice);
}

HRESULT CALLBACK defaultScene::OnD3D11ResizedSwapChain(ID3D11Device* pd3dDevice, IDXGISwapChain* pSwapChain,
	const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext)
{
	float FAspect = static_cast<float>(pBackBufferSurfaceDesc->Width) / static_cast<float>(pBackBufferSurfaceDesc->Height);
	g_Projection = XMMatrixPerspectiveFovLH(XM_PI*0.25f, FAspect, 0.1, 100.f);
	g_Camera.SetProjParams(XM_PI / 4, FAspect, 0.1f, 1000.0f);
	g_Camera.SetWindow(pBackBufferSurfaceDesc->Width, pBackBufferSurfaceDesc->Height);
	g_Camera.SetButtonMasks(MOUSE_LEFT_BUTTON, MOUSE_WHEEL, MOUSE_MIDDLE_BUTTON);

	return S_OK;
}
void CALLBACK defaultScene::OnD3D11FrameRender(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext,
	double fTime, float fElapsedTime, void* pUserContext)
{
	auto pRTV = DXUTGetD3D11RenderTargetView();
	pd3dImmediateContext->ClearRenderTargetView(pRTV, Colors::MidnightBlue);
	auto pDSV = DXUTGetD3D11DepthStencilView();
	pd3dImmediateContext->ClearDepthStencilView(pDSV, D3D11_CLEAR_DEPTH, 1.0, 0);

	g_View = g_Camera.GetViewMatrix();
	g_Projection = g_Camera.GetProjMatrix();
	// Update constant buffer that changes once per frame
	XMMATRIX mWorldViewProjection = g_World * g_View*g_Projection;

	HRESULT hr;
	D3D11_MAPPED_SUBRESOURCE MappedResource;

	V(pd3dImmediateContext->Map(g_pCBChangesEveryFrame.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &MappedResource));
	auto pCB = reinterpret_cast<CBChangesEveryFrame*>(MappedResource.pData);

	XMStoreFloat4x4(&pCB->mWorldViewProj, XMMatrixTranspose(mWorldViewProjection));
	XMStoreFloat4x4(&pCB->mWorld, XMMatrixTranspose(g_World));
	pCB->vMeshColor = g_vMeshColor;
	pd3dImmediateContext->Unmap(g_pCBChangesEveryFrame.Get(), 0);
	
	UINT Stride[1];
	UINT Offset[1];
	Stride[0] = sizeof(SimpleVertex);
	Offset[0] = 0;
	pd3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	pd3dImmediateContext->IASetVertexBuffers(0, 1, g_pVertexBuffer.GetAddressOf(), Stride, Offset);
	pd3dImmediateContext->IASetIndexBuffer(g_pIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	pd3dImmediateContext->VSSetShader(g_pBoxVertexShader.Get(), nullptr, 0);
	pd3dImmediateContext->VSSetConstantBuffers(0, 1, g_pCBChangesEveryFrame.GetAddressOf());
	pd3dImmediateContext->PSSetShader(g_pBoxPixelShader.Get(), nullptr, 0);
	pd3dImmediateContext->PSSetConstantBuffers(0, 1, g_pCBChangesEveryFrame.GetAddressOf());
	pd3dImmediateContext->PSSetShaderResources(0, 1, g_pTextureRV.GetAddressOf());
	pd3dImmediateContext->PSSetSamplers(0, 1, g_pSamplerLinear.GetAddressOf());
	pd3dImmediateContext->DrawIndexed(36, 0, 0);

	bool show_demo_window = false;

	// Any application code here
	ImGui::Begin("Hello, world!");
	ImGui::Text("This is some useful text.");
	ImGui::Text("box color  %.2f, %.2f, %.2f, %.2f", g_vMeshColor.x, g_vMeshColor.y, g_vMeshColor.z, g_vMeshColor.w);
	ImGui::Checkbox("Demo Window", &show_demo_window);
	XMFLOAT4 pos, foucs;
	XMStoreFloat4(&pos, g_Camera.GetEyePt());
	XMStoreFloat4(&foucs, g_Camera.GetLookAtPt());
	ImGui::InputFloat4("cameraPos", (float*)&pos);
	ImGui::InputFloat4("cameraFoucs", (float*)&pos);
	ImGui::End();
	ImGui::Image(g_pTextureRV.Get(), XMFLOAT2(50, 50));
	
	
}

 void CALLBACK defaultScene::OnD3D11DestroyDevice(void* pUserContext)
{
	 DXUTGetGlobalResourceCache().OnDestroyDevice();

	 g_pBoxVertexShader.Reset();
	 g_pIndexBuffer.Reset();
	 g_pVertexLayout.Reset();
	 g_pVertexBuffer.Reset();
	 g_pBoxPixelShader.Reset(); 
	 g_pCBChangesEveryFrame.Reset();
	 g_pTextureRV.Reset();
	 g_pSamplerLinear.Reset();
	
	 g_Mesh.Destroy();
	 isInit = false;
	 /*
	 SAFE_RELEASE(g_pVertexBuffer);
	 SAFE_RELEASE(g_pIndexBuffer);
	 SAFE_RELEASE(g_pVertexLayout);
	 SAFE_RELEASE(g_pTextureRV);
	 SAFE_RELEASE(g_pBoxVertexShader);
	 SAFE_RELEASE(g_pBoxPixelShader);
	 SAFE_RELEASE(g_pCBChangesEveryFrame);
	 SAFE_RELEASE(g_pSamplerLinear);
	*/ 
}
 defaultScene::~defaultScene()
{
	 
	 DXUTGetGlobalResourceCache().OnDestroyDevice();

	 g_pBoxVertexShader.Reset();
	 g_pIndexBuffer.Reset();
	 g_pVertexLayout.Reset();
	 g_pVertexBuffer.Reset();
	 g_pBoxPixelShader.Reset();
	 g_pCBChangesEveryFrame.Reset();
	 g_pTextureRV.Reset();
	 g_pSamplerLinear.Reset();

	 g_Mesh.Destroy();
	 isInit = false;
}
/*DXUT Callback*/

LRESULT CALLBACK defaultScene::MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
	 bool* pbNoFurtherProcessing, void* pUserContext)
 {
	if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
		return true;

	g_Camera.HandleMessages(hWnd, uMsg, wParam, lParam);
	return 0;//::DefWindowProc(hWnd, uMsg, wParam, lParam);

}

void CALLBACK defaultScene::OnFrameMove(double fTime, float fElapsedTime, void* pUserContext)
{
	g_Camera.FrameMove(fElapsedTime);
	

	g_World = XMMatrixRotationY(60.f*XMConvertToRadians(float(fTime)));
	// Modify the color
	g_vMeshColor.x = (sinf((float)fTime * 1.0f) + 1.0f) * 0.5f;
	g_vMeshColor.y = (cosf((float)fTime * 3.0f) + 1.0f) * 0.5f;
	g_vMeshColor.z = (sinf((float)fTime * 5.0f) + 1.0f) * 0.5f;

}