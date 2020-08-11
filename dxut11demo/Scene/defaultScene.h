#pragma once
#include"Scene.h"
#include "SDKmisc.h"
#include "DXUTcamera.h"
#include "SDKmesh.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"
#include <d3d11.h>
#include <wrl/client.h>

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <tchar.h>
#pragma warning( disable : 4100 )

using namespace Microsoft::WRL;

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
struct  SceneImpl;
class defaultScene :public Scene, public Singleton<defaultScene>
{
	friend Singleton<defaultScene>;
public:
	
	virtual HRESULT initScene(ID3D11Device* pd3dDevice);
	virtual std::string getName();
	/*CallbackD3D11*/
	virtual HRESULT CALLBACK OnD3D11CreateDevice(ID3D11Device* pd3dDevice, const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc,
		void* pUserContext);
	virtual HRESULT CALLBACK OnD3D11ResizedSwapChain(ID3D11Device* pd3dDevice, IDXGISwapChain* pSwapChain,
		const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext);
	virtual void CALLBACK OnD3D11FrameRender(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext,
		double fTime, float fElapsedTime, void* pUserContext);
		
	virtual void CALLBACK OnD3D11DestroyDevice(void* pUserContext);
	
	/*DXUT Callback*/

	virtual LRESULT CALLBACK MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
		bool* pbNoFurtherProcessing, void* pUserContext);

	virtual void CALLBACK OnFrameMove(double fTime, float fElapsedTime, void* pUserContext);
	virtual ~defaultScene() ;
private:
	
	std::string sceneName = "defaultScene";
	defaultScene() :Scene(){};
	SceneImpl*pImpl;
};


class baseFx11Scene :public Scene, public Singleton<baseFx11Scene>
{
	friend Singleton<baseFx11Scene>;
public:
	virtual HRESULT initScene(ID3D11Device* pd3dDevice);
	virtual std::string getName();
	/*CallbackD3D11*/
	virtual HRESULT CALLBACK OnD3D11CreateDevice(ID3D11Device* pd3dDevice, const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc,
		void* pUserContext);
	virtual HRESULT CALLBACK OnD3D11ResizedSwapChain(ID3D11Device* pd3dDevice, IDXGISwapChain* pSwapChain,
		const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext);
	virtual void CALLBACK OnD3D11FrameRender(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext,
		double fTime, float fElapsedTime, void* pUserContext);

	virtual void CALLBACK OnD3D11DestroyDevice(void* pUserContext);

	/*DXUT Callback*/

	virtual LRESULT CALLBACK MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
		bool* pbNoFurtherProcessing, void* pUserContext);

	virtual void CALLBACK OnFrameMove(double fTime, float fElapsedTime, void* pUserContext);
	virtual ~baseFx11Scene() ;
private:
	
	std::string sceneName = "basefx11Scene";
	baseFx11Scene() :Scene() {};
	SceneImpl*pImpl;
};


class dynamicScene :public Scene, public Singleton<dynamicScene>
{
	friend Singleton<dynamicScene>;
public:
	virtual HRESULT initScene(ID3D11Device* pd3dDevice);
	virtual std::string getName();
	/*CallbackD3D11*/
	virtual HRESULT CALLBACK OnD3D11CreateDevice(ID3D11Device* pd3dDevice, const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc,
		void* pUserContext);
	virtual HRESULT CALLBACK OnD3D11ResizedSwapChain(ID3D11Device* pd3dDevice, IDXGISwapChain* pSwapChain,
		const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext);
	virtual void CALLBACK OnD3D11FrameRender(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext,
		double fTime, float fElapsedTime, void* pUserContext);

	virtual void CALLBACK OnD3D11DestroyDevice(void* pUserContext);

	/*DXUT Callback*/

	virtual LRESULT CALLBACK MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
		bool* pbNoFurtherProcessing, void* pUserContext);

	virtual void CALLBACK OnFrameMove(double fTime, float fElapsedTime, void* pUserContext);
	virtual ~dynamicScene();
private:

	std::string sceneName = "dynamicLinkScene";
	dynamicScene() :Scene() {};
	SceneImpl*pImpl;
};


class CityScene :public Scene, public Singleton<CityScene>
{
	friend Singleton<CityScene>;
public:
	virtual HRESULT initScene(ID3D11Device* pd3dDevice);
	virtual std::string getName();
	/*CallbackD3D11*/
	virtual HRESULT CALLBACK OnD3D11CreateDevice(ID3D11Device* pd3dDevice, const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc,
		void* pUserContext);
	virtual HRESULT CALLBACK OnD3D11ResizedSwapChain(ID3D11Device* pd3dDevice, IDXGISwapChain* pSwapChain,
		const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext);
	virtual void CALLBACK OnD3D11FrameRender(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext,
		double fTime, float fElapsedTime, void* pUserContext);

	virtual void CALLBACK OnD3D11DestroyDevice(void* pUserContext);

	/*DXUT Callback*/

	virtual LRESULT CALLBACK MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
		bool* pbNoFurtherProcessing, void* pUserContext);

	virtual void CALLBACK OnFrameMove(double fTime, float fElapsedTime, void* pUserContext);
	virtual ~CityScene();
private:

	std::string sceneName = "CityScene";
	CityScene() :Scene() {};
	SceneImpl*pImpl;
};



class BezierScene :public Scene, public Singleton<BezierScene>
{
	friend Singleton<BezierScene>;
public:
	virtual HRESULT initScene(ID3D11Device* pd3dDevice);
	virtual std::string getName();
	/*CallbackD3D11*/
	virtual HRESULT CALLBACK OnD3D11CreateDevice(ID3D11Device* pd3dDevice, const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc,
		void* pUserContext);
	virtual HRESULT CALLBACK OnD3D11ResizedSwapChain(ID3D11Device* pd3dDevice, IDXGISwapChain* pSwapChain,
		const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext);
	virtual void CALLBACK OnD3D11FrameRender(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext,
		double fTime, float fElapsedTime, void* pUserContext);

	virtual void CALLBACK OnD3D11DestroyDevice(void* pUserContext);

	/*DXUT Callback*/

	virtual LRESULT CALLBACK MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
		bool* pbNoFurtherProcessing, void* pUserContext);

	virtual void CALLBACK OnFrameMove(double fTime, float fElapsedTime, void* pUserContext);
	virtual ~BezierScene();
private:

	std::string sceneName = "BezierScene";
	BezierScene() :Scene() {};
	SceneImpl*pImpl;
};


class MultiThreadScene :public Scene, public Singleton<MultiThreadScene>
{
	friend Singleton<MultiThreadScene>;
public:
	virtual HRESULT initScene(ID3D11Device* pd3dDevice);
	virtual std::string getName();
	/*CallbackD3D11*/
	virtual HRESULT CALLBACK OnD3D11CreateDevice(ID3D11Device* pd3dDevice, const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc,
		void* pUserContext);
	virtual HRESULT CALLBACK OnD3D11ResizedSwapChain(ID3D11Device* pd3dDevice, IDXGISwapChain* pSwapChain,
		const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext);
	virtual void CALLBACK OnD3D11FrameRender(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext,
		double fTime, float fElapsedTime, void* pUserContext);

	virtual void CALLBACK OnD3D11DestroyDevice(void* pUserContext);

	/*DXUT Callback*/

	virtual LRESULT CALLBACK MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
		bool* pbNoFurtherProcessing, void* pUserContext);

	virtual void CALLBACK OnFrameMove(double fTime, float fElapsedTime, void* pUserContext);
	virtual ~MultiThreadScene();
private:

	std::string sceneName = "MultiThreadScene";
	MultiThreadScene() :Scene() {};
	SceneImpl*pImpl;
};


class DeferredRendering :public Scene, public Singleton<DeferredRendering>
{
	friend Singleton<DeferredRendering>;
public:
	virtual HRESULT initScene(ID3D11Device* pd3dDevice);
	virtual std::string getName();
	/*CallbackD3D11*/
	virtual HRESULT CALLBACK OnD3D11CreateDevice(ID3D11Device* pd3dDevice, const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc,
		void* pUserContext);
	virtual HRESULT CALLBACK OnD3D11ResizedSwapChain(ID3D11Device* pd3dDevice, IDXGISwapChain* pSwapChain,
		const DXGI_SURFACE_DESC* pBackBufferSurfaceDesc, void* pUserContext);
	virtual void CALLBACK OnD3D11FrameRender(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dImmediateContext,
		double fTime, float fElapsedTime, void* pUserContext);

	virtual void CALLBACK OnD3D11DestroyDevice(void* pUserContext);

	/*DXUT Callback*/

	virtual LRESULT CALLBACK MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
		bool* pbNoFurtherProcessing, void* pUserContext);

	virtual void CALLBACK OnFrameMove(double fTime, float fElapsedTime, void* pUserContext);
	virtual ~DeferredRendering();
private:

	std::string sceneName = "DeferredRendering";
	DeferredRendering() :Scene() {};
	SceneImpl*pImpl;
};