#include"DXUT.h"
#include"Scene.h"


std::string Scene::getName()
{
	return sceneName;
}
bool Scene::IsInit(ID3D11Device* pd3dDevice)
{
	if (!isInit)
	{
		initScene(pd3dDevice);

	}
	
	return isInit;
}
HRESULT Scene::initScene(ID3D11Device* pd3dDevice)
{
	return S_OK;
}
HRESULT CALLBACK Scene::OnD3D11CreateDevice(ID3D11Device* pd3dDevice, const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc,
	void* pUserContext)
{
	
	return S_OK;
}
HRESULT CALLBACK Scene::OnD3D11ResizedSwapChain(ID3D11Device* pd3dDevice, IDXGISwapChain* pSwapChain,
	const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext)
{
	return S_OK;
}
void CALLBACK Scene::OnD3D11FrameRender(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext,
	double fTime, float fElapsedTime, void* pUserContext)
{

}
bool CALLBACK Scene::IsD3D11DeviceAcceptable(const CD3D11EnumAdapterInfo *AdapterInfo, UINT Output, const CD3D11EnumDeviceInfo *DeviceInfo,
	DXGI_FORMAT BackBufferFormat, bool bWindowed, void* pUserContext)
{
	return true;
}
void CALLBACK Scene::OnD3D11ReleasingSwapChain(void* pUserContext)
{
	
}
void CALLBACK Scene::OnD3D11DestroyDevice(void* pUserContext)
{

}
/*DXUT Callback*/
void CALLBACK Scene::OnKeyboard(UINT nChar, bool bKeyDown, bool bAltDown, void* pUserContext)
{

}
void CALLBACK Scene::OnMouse(bool bLeftButtonDown, bool bRightButtonDown, bool bMiddleButtonDown,
	bool bSideButton1Down, bool bSideButton2Down, int nMouseWheelDelta,
	int xPos, int yPos, void* pUserContext)
{

}
LRESULT CALLBACK Scene::MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
	bool* pbNoFurtherProcessing, void* pUserContext)
{
	return S_OK;
}
bool CALLBACK Scene::OnDeviceRemoved(void* pUserContext)
{
	return true;
}
bool CALLBACK Scene::ModifyDeviceSettings(DXUTDeviceSettings* pDeviceSettings, void* pUserContext)
{
	return true;
}
void CALLBACK Scene::OnFrameMove(double fTime, float fElapsedTime, void* pUserContext)
{

}