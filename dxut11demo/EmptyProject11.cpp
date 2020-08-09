//--------------------------------------------------------------------------------------
// File: EmptyProject11.cpp
//
// Empty starting point for new Direct3D 11 Win32 desktop applications
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

#include"DXUT.h"
#include"Scene/defaultScene.h"
#include"Misc.h"
#include<algorithm>
#include <numeric>
using namespace DirectX;


Scene* dynamicScene = nullptr;

enum SceneEnum
{
	defaultSceneE = 0,
	baseFx11SceneE = 1,
	dynamicLinkSceneE = 2,
	CitySceneE = 3,
	MultiSceneE = 4,
	SceneCount
};
std::string ScenesName[] = { "defaultScene","basefx11Scene","dynamicLinkScene","CityScene","MultiThreadScene" };
int ScenesSelecter = 4;

LRUScenes scenesList(4);

//--------------------------------------------------------------------------------------
// Reject any D3D11 devices that aren't acceptable by returning false
//--------------------------------------------------------------------------------------
bool CALLBACK IsD3D11DeviceAcceptable( const CD3D11EnumAdapterInfo *AdapterInfo, UINT Output, const CD3D11EnumDeviceInfo *DeviceInfo,
                                       DXGI_FORMAT BackBufferFormat, bool bWindowed, void* pUserContext )
{
	bool ret = true;
	if(dynamicScene)
		ret = dynamicScene->IsD3D11DeviceAcceptable(AdapterInfo, Output, DeviceInfo, BackBufferFormat, bWindowed, pUserContext);
	return ret;
}


//--------------------------------------------------------------------------------------
// Called right before creating a D3D device, allowing the app to modify the device settings as needed
//--------------------------------------------------------------------------------------
bool CALLBACK ModifyDeviceSettings( DXUTDeviceSettings* pDeviceSettings, void* pUserContext )
{
	bool ret = true;
	if (dynamicScene)
		ret = dynamicScene->ModifyDeviceSettings(pDeviceSettings, pUserContext);
	return ret;
}


//--------------------------------------------------------------------------------------
// Create any D3D11 resources that aren't dependant on the back buffer
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnD3D11CreateDevice(ID3D11Device* pd3dDevice, const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc,
	void* pUserContext)
{
	auto pd3dImmediateContext = DXUTGetD3D11DeviceContext();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();

	ImGui::StyleColorsDark();
	// TODO: Set optional io.ConfigFlags values, e.g. 'io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard' to enable keyboard controls.
	// TODO: Fill optional fields of the io structure later.
	// TODO: Load TTF/OTF fonts if you don't want to use the default font.


	// Initialize helper Platform and Renderer bindings (here we are using imgui_impl_win32.cpp and imgui_impl_dx11.cpp)
	ImGui_ImplWin32_Init(DXUTGetHWND());
	ImGui_ImplDX11_Init(pd3dDevice, pd3dImmediateContext);

	dynamicScene = scenesList.Put(MultiThreadScene::Instance());

	auto hr = dynamicScene->OnD3D11CreateDevice(pd3dDevice, pBackBufferSurfaceDesc, pUserContext);
	
	
	return hr;
}

HRESULT LoadMesh(ID3D11Device* pd3dDevice,ID3D11DeviceContext *pd3dImmediateContext, DWORD  dwShaderFlags)
{
	/*
	HRESULT hr = S_OK;
	ID3DBlob* pVSBlob = nullptr;
	V(DXUTCompileFromFile(L"Blinn-Phone.fx", nullptr, "VS", "vs_4_0", dwShaderFlags, 0, &pVSBlob));
	hr = pd3dDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &g_pMeshVertexShader);
	if (FAILED(hr))
	{
		SAFE_RELEASE(pVSBlob);
		return hr;
	}

	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	UINT numElements = ARRAYSIZE(layout);
	hr = pd3dDevice->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), &g_pVertexLayout);
	SAFE_RELEASE(pVSBlob);
	if (FAILED(hr))
		return hr;

	pd3dImmediateContext->IASetInputLayout(g_pVertexLayout);
	
	ID3DBlob* pPSBlob = nullptr;
	V_RETURN(DXUTCompileFromFile(L"Blinn-Phone.fx", nullptr, "PS", "ps_4_0", dwShaderFlags, 0, &pPSBlob));

	// Create the pixel shader
	hr = pd3dDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &g_pMeshPixelShader);
	SAFE_RELEASE(pPSBlob);
	if (FAILED(hr))
		return hr;


	
	// Load the mesh
	V(g_Mesh.Create(pd3dDevice, L"resource\\tiny\\tiny.sdkmesh"));
	*/
	return S_OK;
}
//--------------------------------------------------------------------------------------
// Create any D3D11 resources that depend on the back buffer
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnD3D11ResizedSwapChain( ID3D11Device* pd3dDevice, IDXGISwapChain* pSwapChain,
                                          const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext )
{
	return dynamicScene->OnD3D11ResizedSwapChain(pd3dDevice, pSwapChain, pBackBufferSurfaceDesc, pUserContext);
	

}


//--------------------------------------------------------------------------------------
// Handle updates to the scene.  This is called regardless of which D3D API is used
//--------------------------------------------------------------------------------------
void CALLBACK OnFrameMove( double fTime, float fElapsedTime, void* pUserContext )
{
	
	dynamicScene->OnFrameMove(fTime, fElapsedTime, pUserContext);
}

void SelectSceneUI()
{
	ImGui::Begin("Scene");
	if (ImGui::CollapsingHeader("SceneSelect")){
		for (int i = 0; i < SceneCount; i++)
		{
			ImGui::RadioButton(ScenesName[i].c_str(), &ScenesSelecter,i);
		}
	}
	ImGui::End();
	
}

void UpdateScene(ID3D11Device* pd3dDevice)
{
	dynamicScene = scenesList.Get(ScenesName[ScenesSelecter]);
	if (dynamicScene == nullptr)
	{
		switch (ScenesSelecter)
		{
		case defaultSceneE:
			dynamicScene = scenesList.Put(defaultScene::Instance());

			break;
		case baseFx11SceneE:
			dynamicScene = scenesList.Put(baseFx11Scene::Instance());
			break;
		case dynamicLinkSceneE:
			dynamicScene = scenesList.Put(dynamicScene::Instance());

			break;
		case MultiSceneE:
			dynamicScene = scenesList.Put(MultiThreadScene::Instance());
		default :
			break;
		}
		
		dynamicScene->IsInit(pd3dDevice);
	}

}
//--------------------------------------------------------------------------------------
// Render the scene using the D3D11 device
//--------------------------------------------------------------------------------------
void CALLBACK OnD3D11FrameRender( ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext,
                                  double fTime, float fElapsedTime, void* pUserContext )
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	
	dynamicScene->OnD3D11FrameRender(pd3dDevice, pd3dImmediateContext, fTime, fElapsedTime, pUserContext);
	ImGui::Begin("FPS");
	float fps = ImGui::GetIO().Framerate;
	float spf = 1000.0f / ImGui::GetIO().Framerate;
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", spf, fps);
	{
		static std::vector<float> arr(90);
		static float fpsSum = 0;
		static float ElapsedTime = 0;
		static char overlay[32];
		static float fpsCount = 0;
		static int count = 0;
		if (ElapsedTime > 100) {
			ElapsedTime -= 100;
			arr[count % 90] = fpsSum / fpsCount;
			sprintf(overlay, "avg %.2f", arr[count++ % 90]);
			fpsCount = 0;
			fpsSum = 0;
				
		}
		fpsSum += fps;
		ElapsedTime += spf;
		++fpsCount;
		ImGui::PlotLines("Frame Times", arr.data(), arr.size(), count % 90, overlay, 0.f, 300.f, XMFLOAT2(0, 80));
	}
	ImGui::End();
	// Any application code here
	SelectSceneUI();
	ImGui::EndFrame();
	// Render dear imgui into screen
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	
	UpdateScene(pd3dDevice);

	
}


//--------------------------------------------------------------------------------------
// Release D3D11 resources created in OnD3D11ResizedSwapChain 
//--------------------------------------------------------------------------------------
void CALLBACK OnD3D11ReleasingSwapChain( void* pUserContext )
{
	dynamicScene->OnD3D11ReleasingSwapChain(pUserContext);
}


//--------------------------------------------------------------------------------------
// Release D3D11 resources created in OnD3D11CreateDevice 
//--------------------------------------------------------------------------------------
void CALLBACK OnD3D11DestroyDevice( void* pUserContext )
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
	dynamicScene->OnD3D11DestroyDevice(pUserContext);

	dynamicScene = nullptr;
	scenesList.destory();
}


//--------------------------------------------------------------------------------------
// Handle messages to the application
//--------------------------------------------------------------------------------------

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
                          bool* pbNoFurtherProcessing, void* pUserContext )
{
	auto hr = S_OK;
	if(dynamicScene)
		 hr = dynamicScene->MsgProc(hWnd, uMsg, wParam, lParam, pbNoFurtherProcessing, pUserContext);
	return hr;
}


//--------------------------------------------------------------------------------------
// Handle key presses
//--------------------------------------------------------------------------------------
void CALLBACK OnKeyboard( UINT nChar, bool bKeyDown, bool bAltDown, void* pUserContext )
{

	dynamicScene->OnKeyboard(nChar, bKeyDown, bAltDown, pUserContext);
}


//--------------------------------------------------------------------------------------
// Handle mouse button presses
//--------------------------------------------------------------------------------------
void CALLBACK OnMouse( bool bLeftButtonDown, bool bRightButtonDown, bool bMiddleButtonDown,
                       bool bSideButton1Down, bool bSideButton2Down, int nMouseWheelDelta,
                       int xPos, int yPos, void* pUserContext )
{

	dynamicScene->OnMouse(bLeftButtonDown, bRightButtonDown, bMiddleButtonDown,
		bSideButton1Down, bSideButton2Down, nMouseWheelDelta,
		xPos, yPos, pUserContext);

}


//--------------------------------------------------------------------------------------
// Call if device was removed.  Return true to find a new device, false to quit
//--------------------------------------------------------------------------------------
bool CALLBACK OnDeviceRemoved( void* pUserContext )
{
	return dynamicScene->OnDeviceRemoved(pUserContext);
   
}


//--------------------------------------------------------------------------------------
// Initialize everything and go into a render loop
//--------------------------------------------------------------------------------------
int WINAPI wWinMain( _In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow )
{
    // Enable run-time memory check for debug builds.
#ifdef _DEBUG
    _CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

    // DXUT will create and use the best device
    // that is available on the system depending on which D3D callbacks are set below

    // Set general DXUT callbacks
    DXUTSetCallbackFrameMove( OnFrameMove );
    DXUTSetCallbackKeyboard( OnKeyboard );
    DXUTSetCallbackMouse( OnMouse );
    DXUTSetCallbackMsgProc( MsgProc );
    DXUTSetCallbackDeviceChanging( ModifyDeviceSettings );
    DXUTSetCallbackDeviceRemoved( OnDeviceRemoved );

    // Set the D3D11 DXUT callbacks. Remove these sets if the app doesn't need to support D3D11
    DXUTSetCallbackD3D11DeviceAcceptable( IsD3D11DeviceAcceptable );
    DXUTSetCallbackD3D11DeviceCreated( OnD3D11CreateDevice );
    DXUTSetCallbackD3D11SwapChainResized( OnD3D11ResizedSwapChain );
    DXUTSetCallbackD3D11FrameRender( OnD3D11FrameRender );
    DXUTSetCallbackD3D11SwapChainReleasing( OnD3D11ReleasingSwapChain );
    DXUTSetCallbackD3D11DeviceDestroyed( OnD3D11DestroyDevice );


	
    // Perform any application-level initialization here
	DXUTInit( true, true, nullptr ); // Parse the command line, show msgboxes on error, no extra command line params
    DXUTSetCursorSettings( true, true ); // Show the cursor and clip it when in full screen
    DXUTCreateWindow( L"EmptyProject11" );

    // Only require 10-level hardware or later
    DXUTCreateDevice( D3D_FEATURE_LEVEL_11_0, true, 800, 600 );
    
	
	
	
	DXUTMainLoop(); // Enter into the DXUT ren  der loop

    // Perform any application-level cleanup here

    return DXUTGetExitCode();
}


