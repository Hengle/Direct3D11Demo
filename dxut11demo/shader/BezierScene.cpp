//
#include"DXUT.h"
//#include"defaultScene.h"
//#include <d3dx11effect.h>
//#include "Gemotry.h"
//using namespace DirectX;
//
//
//static CModelViewerCamera          g_Camera;
//
//static ComPtr<ID3D11InputLayout>          g_pVertexLayout = nullptr;
//static XMMATRIX                    g_World;
//static XMMATRIX                    g_View;
//static XMMATRIX                    g_Projection;
//static XMFLOAT4                    g_vMeshColor(0.7f, 0.7f, 0.7f, 1.0f);
//static XMVECTORF32 				   g_vLightDir = { -0.577,0.577,-0.577 };
//
//static ID3D11ShaderResourceView*          cubeSRV = nullptr;
//static ID3D11Buffer*						SOVertexBuffer = nullptr;
//static ID3D11Buffer* nullBuffer = nullptr;
///*Effect Declare*/
//static ID3DX11Effect*					   g_pEffect = nullptr;
//static ID3DX11Effect*					   g_pEvenEffect = nullptr;
//static ID3DX11Effect*					   g_pOddEffect = nullptr;
//
//static ID3DX11EffectTechnique*             g_pTechnique = nullptr;
//
//static ID3DX11EffectMatrixVariable*        g_pWVPariable = nullptr;
//static ID3DX11EffectMatrixVariable*        g_pCameraPosVariable = nullptr;
//static ID3DX11EffectMatrixVariable*        g_pProjectionVariable = nullptr;
//
//
//static float g_pModeWaviness;
//static CDXUTSDKMesh                        g_Mesh;
//
//
//static std::string EffectName[] = { "BaseEffect","ReflactEffect","NormalGSEffect","DynamicNormalGSEffect","Blend" };
//static int EffectIndex = 0;
//static int preEffectIndex = 0;
//static D3D11_INPUT_ELEMENT_DESC layout[] =
//{
//	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
//	{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
//	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
//};
//
//
//static void FrameUI();
//static HRESULT UpdateEffect(ID3D11Device* pd3dDevice);
//
//void compileShader(ID3D11Device* pd3dDevice,const WCHAR*sname, D3D_SHADER_MACRO* maco, DWORD dwShaderFlags,ID3DX11Effect*effect)
//{
//	HRESULT hr = S_OK;
//	WCHAR str[MAX_PATH];
//
//	ID3DBlob* errorMsg = nullptr;
//	V(DXUTFindDXSDKMediaFileCch(str, MAX_PATH, sname));
//	hr = D3DX11CompileEffectFromFile(str, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, dwShaderFlags, 0, pd3dDevice, &effect, &errorMsg);
//	if (FAILED(hr))
//	{
//		if (errorMsg)
//		{
//			OutputDebugStringA((char*)errorMsg->GetBufferPointer());
//			errorMsg->Release();
//		}
//
//		if (g_pEffect)
//			g_pEffect->Release();
//
//	
//	}
//
//}
//
//
//std::string BezierScene::getName()
//{
//	return sceneName;
//}
//HRESULT BezierScene::initScene(ID3D11Device* pd3dDevice)
//{
//	isInit = true;
//	HRESULT hr = S_OK;
//
//	auto pd3dImmediateContext = DXUTGetD3D11DeviceContext();
//
//
//	DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
//#ifdef _DEBUG
//	// Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
//	// Setting this flag improves the shader debugging experience, but still allows 
//	// the shaders to be optimized and to run exactly the way they will run in 
//	// the release configuration of this program.
//	dwShaderFlags |= D3DCOMPILE_DEBUG;
//
//	// Disable optimizations to further improve shader debugging
//	dwShaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
//#endif
//
//#if D3D_COMPILER_VERSION >= 46 
//
//	// Read the D3DX effect file
//	compileShader(pd3dDevice, L"shader//simpleBezier.fx", nullptr, dwShaderFlags, g_pEffect);
//	D3D_SHADER_MACRO macro[] = { "Besizer_Hs_Partition", "fractional_even", NULL, NULL };
//	compileShader(pd3dDevice, L"shader//simpleBezier.fx", nullptr, dwShaderFlags, g_pEvenEffect);
//	
//
//#else
//
//	ID3DBlob* pEffectBuffer = nullptr;
//	V_RETURN(DXUTCompileFromFile(L"Tutorial11.fx", nullptr, "none", "fx_5_0", dwShaderFlags, 0, &pEffectBuffer));
//	hr = D3DX11CreateEffectFromMemory(pEffectBuffer->GetBufferPointer(), pEffectBuffer->GetBufferSize(), 0, pd3dDevice, &g_pEffect);
//	SAFE_RELEASE(pEffectBuffer);
//	if (FAILED(hr))
//		return hr;
//
//#endif
//
//	// Obtain the technique
//	g_pTechnique = g_pEffect->GetTechniqueByName(EffectName[EffectIndex].c_str());
//
//	 g_pEvenEffect->GetVariableByName("ControlHS")->AsShader();
//	// Obtain the variables
//	
//	g_pWVPariable = g_pEffect->GetVariableByName("g_mViewProjection")->AsMatrix();
//	g_pViewVariable = g_pEffect->GetVariableByName("g_vCameraPosWorld")->AsMatrix();
//	g_pProjectionVariable = g_pEffect->GetVariableByName("g_fTessellationFactor")->AsMatrix();
//	
//	// Set Waviness
//	g_pWavinessVariable->SetFloat(g_pModeWaviness);
//
//	V_RETURN(DXUTCreateShaderResourceViewFromFile(pd3dDevice, L"Lobby\\LobbyCube.dds", &cubeSRV));
//	g_ptxEnvVariable->SetResource(cubeSRV);
//
//
//
//
//	// Define the input layout
//
//	UINT numElements = ARRAYSIZE(layout);
//
//	// Create the input layout
//	D3DX11_PASS_DESC PassDesc;
//	V_RETURN(g_pTechnique->GetPassByIndex(0)->GetDesc(&PassDesc));
//	V_RETURN(pd3dDevice->CreateInputLayout(layout, numElements, PassDesc.pIAInputSignature,
//		PassDesc.IAInputSignatureSize, g_pVertexLayout.GetAddressOf()));
//
//	// Set the input layout
//	pd3dImmediateContext->IASetInputLayout(g_pVertexLayout.Get());
//
//	// Load the mesh
//	V_RETURN(g_Mesh.Create(pd3dDevice, L"resource\\tiny.sdkmesh"));
//
//
//	// create user d3dbuffer
//	struct tmp {
//		XMFLOAT3 pos;
//		XMFLOAT3 normal;
//		XMFLOAT2 Tex;
//	};
//	auto vb = g_Mesh.GetVB11(0, 0);
//	D3D11_BUFFER_DESC bd;
//	vb->GetDesc(&bd);
//
//	bd.BindFlags = D3D11_BIND_STREAM_OUTPUT | D3D11_BIND_VERTEX_BUFFER;
//	bd.ByteWidth = g_Mesh.GetNumVertices(0, 0) * sizeof(tmp) * 3;
//	pd3dDevice->CreateBuffer(&bd, nullptr, &SOVertexBuffer);
//
//
//
//
//	// Initialize the world matrices
//	g_World = XMMatrixIdentity();
//
//	// Setup the camera's view parameters
//	static const XMVECTORF32 s_Eye = { 0.0f, 3.0f, -800.0f, 0.f };
//	static const XMVECTORF32 s_At = { 0.0f, 1.0f, 0.0f, 0.f };
//	g_Camera.SetViewParams(s_Eye, s_At);
//
//	return S_OK;
//}
//
//HRESULT CALLBACK BezierScene::OnD3D11CreateDevice(ID3D11Device* pd3dDevice, const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc,
//	void* pUserContext)
//{
//
//	return initScene(pd3dDevice);
//}
//
//HRESULT CALLBACK BezierScene::OnD3D11ResizedSwapChain(ID3D11Device* pd3dDevice, IDXGISwapChain* pSwapChain,
//	const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext)
//{
//	float FAspect = static_cast<float>(pBackBufferSurfaceDesc->Width) / static_cast<float>(pBackBufferSurfaceDesc->Height);
//	g_Projection = XMMatrixPerspectiveFovLH(XM_PI*0.25f, FAspect, 0.1, 100.f);
//	g_Camera.SetProjParams(XM_PI / 4, FAspect, 0.1f, 1000.0f);
//	g_Camera.SetWindow(pBackBufferSurfaceDesc->Width, pBackBufferSurfaceDesc->Height);
//	g_Camera.SetButtonMasks(MOUSE_LEFT_BUTTON, MOUSE_WHEEL, MOUSE_MIDDLE_BUTTON);
//
//	return S_OK;
//}
//void CALLBACK BezierScene::OnD3D11FrameRender(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext,
//	double fTime, float fElapsedTime, void* pUserContext)
//{
//	auto pRTV = DXUTGetD3D11RenderTargetView();
//	pd3dImmediateContext->ClearRenderTargetView(pRTV, Colors::MidnightBlue);
//	auto pDSV = DXUTGetD3D11DepthStencilView();
//	pd3dImmediateContext->ClearDepthStencilView(pDSV, D3D11_CLEAR_DEPTH, 1.0, 0);
//
//	g_View = g_Camera.GetViewMatrix();
//	g_Projection = g_Camera.GetProjMatrix();
//	// Update constant buffer that changes once per frame
//	XMMATRIX mWorldViewProjection = g_World * g_View*g_Projection;
//
//	g_pWorldVariable->SetMatrix(reinterpret_cast<float*>(&g_World));
//	g_pViewVariable->SetMatrix(reinterpret_cast<float*>(&g_View));
//	g_pProjectionVariable->SetMatrix(reinterpret_cast<float*>(&g_Projection));
//	g_pTimeVariable->SetFloat((float)fTime);
//	g_pWavinessVariable->SetFloat(g_pModeWaviness);
//	g_pLightDirVariable->SetFloatVector(g_vLightDir);
//	pd3dImmediateContext->IASetInputLayout(g_pVertexLayout.Get());
//
//
//
//	UINT Strides[1];
//	UINT Offsets[1];
//	ID3D11Buffer* pVB[1];
//	pVB[0] = g_Mesh.GetVB11(0, 0);
//	Strides[0] = (UINT)g_Mesh.GetVertexStride(0, 0);
//	Offsets[0] = 0;
//	pd3dImmediateContext->IASetVertexBuffers(0, 1, pVB, Strides, Offsets);
//	pd3dImmediateContext->IASetIndexBuffer(g_Mesh.GetIB11(0), g_Mesh.GetIBFormat11(0), 0);
//
//	D3DX11_TECHNIQUE_DESC techDesc;
//	HRESULT hr;
//	V(g_pTechnique->GetDesc(&techDesc));
//
//
//
//
//
//	for (UINT p = 0; p < techDesc.Passes; ++p)
//	{
//
//		for (UINT subset = 0; subset < g_Mesh.GetNumSubsets(0); ++subset)
//		{
//			auto pSubset = g_Mesh.GetSubset(0, subset);
//
//			auto PrimType = g_Mesh.GetPrimitiveType11((SDKMESH_PRIMITIVE_TYPE)pSubset->PrimitiveType);
//
//			pd3dImmediateContext->IASetPrimitiveTopology(PrimType);
//			auto pDiffuseRV = g_Mesh.GetMaterial(pSubset->MaterialID)->pDiffuseRV11;
//			g_ptxDiffuseVariable->SetResource(pDiffuseRV);
//			g_pTechnique->GetPassByIndex(p)->Apply(0, pd3dImmediateContext);
//			if (EffectName[EffectIndex] == "DynamicNormalGSEffect" || EffectName[EffectIndex] == "NormalGSEffect")
//			{
//				//为了匹配GS的输入图元类型需要设置为D3D11_PRIMITIVE_TOPOLOGY_POINTLIST
//				if (p == 1)
//					pd3dImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
//			}
//
//			if (EffectName[EffectIndex] == "DynamicNormalGSEffect")
//			{
//				if (p == 1) {
//					pd3dImmediateContext->SOSetTargets(1, &nullBuffer, Offsets);
//					pd3dImmediateContext->IASetVertexBuffers(0, 1, &SOVertexBuffer, Strides, Offsets);
//					pd3dImmediateContext->DrawAuto();
//					continue;
//
//				}
//				pd3dImmediateContext->SOSetTargets(1, &nullBuffer, Offsets);
//				pd3dImmediateContext->SOSetTargets(1, &SOVertexBuffer, Offsets);
//			}
//			pd3dImmediateContext->DrawIndexed((UINT)pSubset->IndexCount, 0, (UINT)pSubset->VertexStart);
//
//
//
//
//		}
//	}
//
//
//
//
//	FrameUI();
//
//	V(UpdateEffect(pd3dDevice));
//
//
//}
//
//
//HRESULT UpdateEffect(ID3D11Device* pd3dDevice)
//{
//	HRESULT hr;
//	if (preEffectIndex == EffectIndex)return S_OK;
//	preEffectIndex = EffectIndex;
//
//	g_pTechnique = g_pEffect->GetTechniqueByName(EffectName[EffectIndex].c_str());
//
//	D3DX11_PASS_DESC PassDesc;
//	V_RETURN(g_pTechnique->GetPassByIndex(0)->GetDesc(&PassDesc));
//
//	V_RETURN(pd3dDevice->CreateInputLayout(layout, ARRAYSIZE(layout), PassDesc.pIAInputSignature,
//		PassDesc.IAInputSignatureSize, &g_pVertexLayout));
//	return hr;
//}
//void FrameUI()
//{
//	// Any application code here
//	ImGui::Begin("Mesh");
//	ImGui::Text("MeshName %s", "tiny.sdkmesh");
//	XMFLOAT4 pos, foucs;
//	XMStoreFloat4(&pos, g_Camera.GetEyePt());
//	XMStoreFloat4(&foucs, g_Camera.GetLookAtPt());
//	ImGui::InputFloat4("cameraPos", (float*)&pos);
//	ImGui::InputFloat4("cameraFoucs", (float*)&foucs);
//	ImGui::InputFloat3("LightDir32", (float*)g_vLightDir.f);
//
//	ImGui::End();
//	ImGui::SliderFloat("Wavness", &g_pModeWaviness, 0, 50);
//
//	ImGui::Begin("Effect");
//	for (int i = 0; i < 4; i++)
//		ImGui::RadioButton(EffectName[i].c_str(), &EffectIndex, i);
//	ImGui::End();
//}
//
//void CALLBACK BezierScene::OnD3D11DestroyDevice(void* pUserContext)
//{
//
//
//
//
//
//	DXUTGetGlobalResourceCache().OnDestroyDevice();
//
//	g_Mesh.Destroy();
//	g_pVertexLayout.Reset();
//	SAFE_RELEASE(cubeSRV);
//	SAFE_RELEASE(g_pEffect);
//	SAFE_RELEASE(SOVertexBuffer);
//	g_pTechnique = nullptr;
//
//	g_ptxDiffuseVariable = nullptr;
//
//	g_pWorldVariable = nullptr;
//	g_pViewVariable = nullptr;
//	g_pProjectionVariable = nullptr;
//	g_pWavinessVariable = nullptr;
//	g_pTimeVariable = nullptr;
//	isInit = false;
//}
//
//BezierScene:: ~baseFx11Scene()
//{
//
//
//
//
//	DXUTGetGlobalResourceCache().OnDestroyDevice();
//
//	g_Mesh.Destroy();
//	g_pVertexLayout.Reset();
//
//	SAFE_RELEASE(g_pEffect);
//
//	g_pTechnique = nullptr;
//
//	g_ptxDiffuseVariable = nullptr;
//
//	g_pWorldVariable = nullptr;
//	g_pViewVariable = nullptr;
//	g_pProjectionVariable = nullptr;
//	g_pWavinessVariable = nullptr;
//	g_pTimeVariable = nullptr;
//	isInit = false;
//
//}
//
///*DXUT Callback*/
//
//LRESULT CALLBACK BezierScene::MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
//	bool* pbNoFurtherProcessing, void* pUserContext)
//{
//	if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
//		return true;
//
//	g_Camera.HandleMessages(hWnd, uMsg, wParam, lParam);
//	return 0;//::DefWindowProc(hWnd, uMsg, wParam, lParam);
//
//}
//
//void CALLBACK BezierScene::OnFrameMove(double fTime, float fElapsedTime, void* pUserContext)
//{
//	g_Camera.FrameMove(fElapsedTime);
//
//
//
//	// Modify the color
//	g_vMeshColor.x = (sinf((float)fTime * 1.0f) + 1.0f) * 0.5f;
//	g_vMeshColor.y = (cosf((float)fTime * 3.0f) + 1.0f) * 0.5f;
//	g_vMeshColor.z = (sinf((float)fTime * 5.0f) + 1.0f) * 0.5f;
//
//	float radians = XMConvertToRadians(float(600 * fElapsedTime));
//	auto tmpM = XMMatrixRotationY(XMConvertToRadians(radians));
//
//	g_vLightDir.v = XMVector3Transform(g_vLightDir, tmpM);
//	XMVector3Normalize(g_vLightDir);
//}