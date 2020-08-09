#pragma once

#include "DXUT.h"
#include"Singleton.h"

#include<string>
class Scene
{
public:
	
	bool IsInit(ID3D11Device* pd3dDevice);
	virtual std::string getName();
	virtual HRESULT initScene(ID3D11Device* pd3dDevice);
	/*CallbackD3D11*/
	virtual HRESULT CALLBACK OnD3D11CreateDevice(ID3D11Device* pd3dDevice, const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc,
		void* pUserContext);
	virtual HRESULT CALLBACK OnD3D11ResizedSwapChain(ID3D11Device* pd3dDevice, IDXGISwapChain* pSwapChain,
		const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext);
	virtual void CALLBACK OnD3D11FrameRender(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext,
		double fTime, float fElapsedTime, void* pUserContext);
	virtual bool CALLBACK IsD3D11DeviceAcceptable(const CD3D11EnumAdapterInfo *AdapterInfo, UINT Output, const CD3D11EnumDeviceInfo *DeviceInfo,
		DXGI_FORMAT BackBufferFormat, bool bWindowed, void* pUserContext);
	virtual void CALLBACK OnD3D11ReleasingSwapChain(void* pUserContext);
	virtual void CALLBACK OnD3D11DestroyDevice(void* pUserContext);

	/*DXUT Callback*/
	virtual void CALLBACK OnKeyboard(UINT nChar, bool bKeyDown, bool bAltDown, void* pUserContext);
	virtual void CALLBACK OnMouse(bool bLeftButtonDown, bool bRightButtonDown, bool bMiddleButtonDown,
		bool bSideButton1Down, bool bSideButton2Down, int nMouseWheelDelta,
		int xPos, int yPos, void* pUserContext);
	virtual LRESULT CALLBACK MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
		bool* pbNoFurtherProcessing, void* pUserContext);
	virtual bool CALLBACK OnDeviceRemoved(void* pUserContext);
	virtual bool CALLBACK ModifyDeviceSettings(DXUTDeviceSettings* pDeviceSettings, void* pUserContext);
	virtual void CALLBACK OnFrameMove(double fTime, float fElapsedTime, void* pUserContext);
public:
	virtual ~Scene() {};
	Scene() = default;
protected:
	bool isInit = false;
private:
	std::string sceneName = "scene";
	
};


