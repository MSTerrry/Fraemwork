#include "CubeComponent.h"
#include "CubeWithTrianglesGame.h"
#include "TrueTriangleComponent.h"
void CubeWithTrianglesGame::Initialize() {
	cam = new Camera(Display);
	auto t1 = Matrix::CreateScale(1.1);
	InDevice = new InputDevice(Display);
	camController = new CameraController(InDevice, cam);
	auto pc = new PlaneComponent(Device, Context, cam);
	Components.push_back(pc);
	auto cube = new CubeComponent(Device, Context, cam);
	Components.push_back(cube);
	CreateTriangles();
	
}
void CubeWithTrianglesGame::CreateTriangles() {
	auto t1 = new Vector4[]{
			Vector4(1,0,1,1), Vector4(1.0f, 0.0f, 0.0f, 1.0f),
			Vector4(2,0,0,1), Vector4(1.0f, 0.0f, 0.0f, 1.0f),
			Vector4(1,0,-1,1), Vector4(1.0f, 0.0f, 0.0f, 1.0f)
	};
	auto t2 = new Vector4[]{
			Vector4(-1,0,1,1), Vector4(1.0f, 0.0f, 0.0f, 1.0f),
			Vector4(0,0,2,1), Vector4(1.0f, 0.0f, 0.0f, 1.0f),
			Vector4(1,0,1,1), Vector4(1.0f, 0.0f, 0.0f, 1.0f)
	};
	auto t3 = new Vector4[]{
			Vector4(-1,0,1,1), Vector4(1.0f, 0.0f, 0.0f, 1.0f),
			Vector4(-2,0,0,1), Vector4(1.0f, 0.0f, 0.0f, 1.0f),
			Vector4(-1,0,-1,1), Vector4(1.0f, 0.0f, 0.0f, 1.0f)
	};
	auto t4 = new Vector4[]{
			Vector4(-1,0,-1,1), Vector4(1.0f, 0.0f, 0.0f, 1.0f),
			Vector4(0,0,-2,1), Vector4(1.0f, 0.0f, 0.0f, 1.0f),
			Vector4(1,0,-1,1), Vector4(1.0f, 0.0f, 0.0f, 1.0f)
	};
	Components.push_back(new TrueTriangleComponent(Device, Context, cam,
		Vector3(4, 0, 0), t1,false));
	Components.push_back(new TrueTriangleComponent(Device, Context, cam,
		Vector3(0, 0, 4), t2,true));
	Components.push_back(new TrueTriangleComponent(Device, Context, cam,
		Vector3(-4, 0, 0), t3,false));
	Components.push_back(new TrueTriangleComponent(Device, Context, cam,
		Vector3(0, 0, -4), t4,true));
}

void CubeWithTrianglesGame::Update(float deltaTime) {
	camController->Update(deltaTime);
	if (InDevice->IsKeyDown(Keys::Escape))
		Exit();
	Game::Update(deltaTime);
}

void CubeWithTrianglesGame::PostDraw(float deltaTime) {

}

void CubeWithTrianglesGame::OnMouseMove(InputDevice::MouseMoveEventArgs& args) {

}