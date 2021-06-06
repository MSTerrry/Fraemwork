#include "Game.h"
Game* GlobalGame;
LRESULT CALLBACK Game::WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{	
	switch (umessage)
	{	
	case WM_DESTROY:
	case WM_CLOSE:
	{
		PostQuitMessage(0);
		return 0;
	}	
	case WM_SIZE:
	{
		std::cout << "Width " << LOWORD(lparam) << " Height " << HIWORD(lparam) << std::endl;
		return 0;
	}
	case WM_INPUT:
	{
		UINT dwSize = 0;
		GetRawInputData(reinterpret_cast<HRAWINPUT>(lparam), RID_INPUT, nullptr, &dwSize, sizeof(RAWINPUTHEADER));
		LPBYTE lpb = new BYTE[dwSize];
		if (lpb == nullptr) {
			return 0;
		}

		if (GetRawInputData((HRAWINPUT)lparam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER)) != dwSize)
			OutputDebugString(TEXT("GetRawInputData does not return correct size !\n"));

		RAWINPUT* raw = reinterpret_cast<RAWINPUT*>(lpb);	

		if (raw->header.dwType == RIM_TYPEKEYBOARD)
		{			
			GlobalGame->InDevice->OnKeyDown({
				raw->data.keyboard.MakeCode,
				raw->data.keyboard.Flags,
				raw->data.keyboard.VKey,
				raw->data.keyboard.Message
				});
		}
		else if (raw->header.dwType == RIM_TYPEMOUSE)
		{
			GlobalGame->InDevice->OnMouseMove({
				raw->data.mouse.usFlags,
				raw->data.mouse.usButtonFlags,
				static_cast<int>(raw->data.mouse.ulExtraInformation),
				static_cast<int>(raw->data.mouse.ulRawButtons),
				static_cast<short>(raw->data.mouse.usButtonData),
				raw->data.mouse.lLastX,
				raw->data.mouse.lLastY
				});
		}

		delete[] lpb;
		return DefWindowProc(hwnd, umessage, wparam, lparam);
	}
	default:
		return DefWindowProc(hwnd, umessage, wparam, lparam);
	}
}

Game::Game(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow):Instance(hInstance)
{	
	GlobalGame = this;
	Initialize();		
}

void Game::Initialize()
{		
	Display = new DisplayWin32(Instance,WndProc);
	if (!Display->hWnd) {
		DestroyResources();
		return;
	}		
	if (FAILED(PrepareResources())) {
		DestroyResources();
		return;
	}		
}

void Game::Run() {
	MSG msg = {};

	// Loop until there is a quit message from the window or the user.
	
	while (!isExitRequested) {
		// Handle the windows messages.
		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		if (msg.message == WM_QUIT) {
			isExitRequested = true;
		}

		Draw(Display->hWnd);
	}

	DestroyResources();

	_debug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
}
void Game::Update(float deltaTime) {
	for (auto var : Components)
	{		
		var->Update(deltaTime);
		var->Draw(deltaTime);
	}
	swapChain1->Present(1, 0);
}
HRESULT Game::PrepareResources() {
	HRESULT res;
	DXGI_SWAP_CHAIN_DESC swapDesc = {};
	swapDesc.BufferCount = 2;
	swapDesc.BufferDesc.Width = Display->ScreenWidth;
	swapDesc.BufferDesc.Height = Display->ScreenHeight;
	swapDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapDesc.OutputWindow = Display->hWnd;
	swapDesc.Windowed = true;
	swapDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	swapDesc.SampleDesc.Count = 1;
	swapDesc.SampleDesc.Quality = 0;

	D3D_FEATURE_LEVEL featureLevel[] = { D3D_FEATURE_LEVEL_11_1 };
	res = D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		D3D11_CREATE_DEVICE_DEBUG,
		featureLevel,
		1,
		D3D11_SDK_VERSION,
		&swapDesc,
		&swapChain,
		&Device,
		nullptr,
		&Context);
	ZCHECK(res);
	if (FAILED(res)) return res;
	ID3D11Texture2D* backTex;
	res = swapChain->GetBuffer(0, IID_ID3D11Texture2D, (void**)&backTex);	ZCHECK(res);
	if (FAILED(res)) return res;
	res = Device->CreateRenderTargetView(backTex, nullptr, &RenderView);			ZCHECK(res);
	if (FAILED(res)) return res;
	swapChain->QueryInterface<IDXGISwapChain1>(&swapChain1);
	Context->QueryInterface(IID_ID3DUserDefinedAnnotation, (void**)&DebugAnnotation);
	Device->QueryInterface(IID_ID3D11Debug, (void**)&_debug);
	CreateBackBuffer();
	D3D11_VIEWPORT viewport = {};
	viewport.Width = static_cast<float>(Display->ScreenWidth);
	viewport.Height = static_cast<float>(Display->ScreenHeight);
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MinDepth = 0;
	viewport.MaxDepth = 1.0f;

	Context->RSSetViewports(1, &viewport);
	Context->OMSetRenderTargets(1, &RenderView, DepthStencilView);

	return res;
}
void Game::CreateBackBuffer() {
	HRESULT res;
	D3D11_TEXTURE2D_DESC depthTexDesc = {};
	depthTexDesc.ArraySize = 1;
	depthTexDesc.MipLevels = 1;
	depthTexDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	depthTexDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;
	depthTexDesc.CPUAccessFlags = 0;
	depthTexDesc.MiscFlags = 0;
	depthTexDesc.Usage = D3D11_USAGE_DEFAULT;
	depthTexDesc.Width = Display->ScreenWidth;
	depthTexDesc.Height = Display->ScreenHeight;
	depthTexDesc.SampleDesc = { 1,0 };
	res = Device->CreateTexture2D(&depthTexDesc, nullptr, &_depthBuffer);
	if (FAILED(res)) return;

	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilDesc = {};
	depthStencilDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthStencilDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilDesc.Flags = 0;
	res = Device->CreateDepthStencilView(_depthBuffer, &depthStencilDesc, &DepthStencilView); if (FAILED(res)) return;


}
void Game::RestoreTargets() {	
	Context->OMSetRenderTargets(1, &RenderView, DepthStencilView);
	D3D11_VIEWPORT viewport = {};
	viewport.Width = static_cast<float>(Display->ScreenWidth);
	viewport.Height = static_cast<float>(Display->ScreenHeight);
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MinDepth = 0;
	viewport.MaxDepth = 1.0f;
	Context->RSSetViewports(1, &viewport);

}
void Game::Exit() {
	isExitRequested = true;
}
HRESULT Game::InitShaders() {

	return S_OK;
}

int Game::Draw(HWND hWnd) {
	auto	curTime = std::chrono::steady_clock::now();
	float	deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(curTime - PrevTime).count() / 1000000.0f;
	PrevTime = curTime;

	TotalTime += deltaTime;
	
	float color[] = { 0.0f, 0.0f, 0.0f, 1.0f };

	Context->OMSetRenderTargets(1, &RenderView, nullptr);
	Context->ClearRenderTargetView(RenderView, color);
	Context->ClearDepthStencilView(DepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0.0f);
	tGame->Update(deltaTime);
	
	return 0;
}

void Game::DestroyResources()
{
	if (Context) Context->ClearState();
	if (_constantBuffer) _constantBuffer->Release();
	if (_vertexBuffer) _vertexBuffer->Release();
	if (_indexBuffer) _indexBuffer->Release();	
	for  (auto c: Components)
	{
		c->DestroyResources();
	}
	if (RenderView) RenderView->Release();
	if (Context) Context->Release();
	if (Device) Device->Release();
}

void Game::PostDraw(float deltaTime) {

}