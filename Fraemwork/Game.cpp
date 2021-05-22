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
	case WM_KEYDOWN:
	{
		std::cout << "Key: " << static_cast<unsigned int>(wparam) << std::endl;
		if (static_cast<unsigned int>(wparam) == 27) PostQuitMessage(0);
		return 0;
	}	
	case WM_KEYUP:
		return 0;
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
		//Game* pWnd = reinterpret_cast<Game*>(GetWindowLongPtrW(hwnd, 0));
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
	/*TriangleComponent tComp(Device, Context, nullptr);
	tComp.Initialize();	
	Components.push_back(&tComp);*/
	/*if (FAILED(InitShaders())) {
		DestroyResources();
		return;
	}*/

	
}

void Game::Run() {
	MSG msg = {};

	// Loop until there is a quit message from the window or the user.
	bool isExitRequested = false;
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

	D3D11_VIEWPORT viewport = {};
	viewport.Width = static_cast<float>(Display->ScreenWidth);
	viewport.Height = static_cast<float>(Display->ScreenHeight);
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MinDepth = 0;
	viewport.MaxDepth = 1.0f;

	Context->RSSetViewports(1, &viewport);
	Context->OMSetRenderTargets(1, &RenderView, nullptr);

	return res;
}
void Game::CreateBackBuffer(TriangleComponent tComp) {
	

}
void Game::Exit() {

}
HRESULT Game::InitShaders() {
	//HRESULT res;	

	////TriangleComponent tComp(Device,Context,Camera);
	//res = tComp.Initialize();
	//if (FAILED(res)) return res;
	//Components.push_back(&tComp);		
	return S_OK;
}

int Game::Draw(HWND hWnd) {
	auto	curTime = std::chrono::steady_clock::now();
	float	deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(curTime - PrevTime).count() / 1000000.0f;
	PrevTime = curTime;

	TotalTime += deltaTime;
	//frameCount++;


	/*if (TotalTime > 1.0f) {
		float fps = frameCount / TotalTime;

		TotalTime = 0.0f;

		WCHAR text[256];
		swprintf_s(text, TEXT("FPS: %f"), fps);
		SetWindowText(hWnd, text);

		frameCount = 0;
	}*/

	float color[] = { 0.0f, 0.0f, 0.0f, 1.0f };

	Context->OMSetRenderTargets(1, &RenderView, nullptr);
	Context->ClearRenderTargetView(RenderView, color);
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