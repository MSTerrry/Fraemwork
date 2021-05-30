#include "LightTextureGame.h"


void LightTextureGame::Initialize() {
	cam = new Camera(Display);
	InDevice = new InputDevice(Display);
	camController = new CameraController(InDevice, cam);
	pc = new PlaneComponent(Device, Context, cam);
	pc->Initialize();
	Components.push_back(pc);
	
	//auto obj = new ObjModelComponent(Device, Context, L"model.obj", L"model.jpg",cam);
	//obj->ClipPlane = DirectX::SimpleMath::Vector4(0.0f, -1.0f, 0.0f, -10.0f);
	//Components.push_back(obj);
	
	auto tiny = new TinyObjModelComponent(Device, Context, cam, "D:/Graph/Fraemwork/Fraemwork/models/BabyYoda.obj");
	tiny->Initialize();
	Components.push_back(tiny);

	//auto atv = new TinyObjModelComponent(Device, Context, cam, "file.obj");
	//atv->Position = DirectX::SimpleMath::Vector3(400,0,0);
	//Components.push_back(atv);
	
	tri = new TriangleComponent(Device, Context, cam);
	tri->Initialize();
	Components.push_back(tri);
	//InDevice->MouseMove.AddRaw(this, &LightTextureGame::OnMouseMove);
}


void LightTextureGame::Update(float deltaTime) {
	camController->Update(deltaTime);
	if (InDevice->IsKeyDown(Keys::Escape))
		Exit();
	Game::Update(deltaTime);
}

void LightTextureGame::PostDraw(float deltaTime) {

}

void LightTextureGame::OnMouseMove(InputDevice::MouseMoveEventArgs& args) {

}