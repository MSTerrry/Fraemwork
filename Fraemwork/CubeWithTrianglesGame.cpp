#include "CubeComponent.h"
#include "CubeWithTrianglesGame.h"
#include "TrueTriangleComponent.h"
#include "Transform.h"
void CubeWithTrianglesGame::Initialize() {
	cam = new Camera(Display);
	auto t1 = Matrix::CreateScale(1.1);
	InDevice = new InputDevice(Display);
	camController = new CameraController(InDevice, cam);
	auto pc = new PlaneComponent(Device, Context, cam);
	Components.push_back(pc);
	auto cube = new CubeComponent(Device, Context, cam);
	Components.push_back(cube);
	CreateTriangles(cube);
	
}
void CubeWithTrianglesGame::CreateTriangles(CubeComponent* cube) {
	auto t1 = new TrueTriangleComponent(Device, Context, cam,
		Vector3(4, 0, 0),
		new Vector4[]{
			Vector4(1,0,1,1), Vector4(1.0f, 0.0f, 0.0f, 1.0f),
			Vector4(2,0,0,1), Vector4(1.0f, 0.0f, 0.0f, 1.0f),
			Vector4(1,0,-1,1), Vector4(1.0f, 0.0f, 0.0f, 1.0f) },
			false,cube->transform);	
	auto t2 = new TrueTriangleComponent(Device, Context, cam,
		Vector3(0, 0, 4),
		new Vector4[]{
			Vector4(-1,0,1,1), Vector4(1.0f, 0.0f, 0.0f, 1.0f),
			Vector4(0,0,2,1), Vector4(1.0f, 0.0f, 0.0f, 1.0f),
			Vector4(1,0,1,1), Vector4(1.0f, 0.0f, 0.0f, 1.0f)
		},
		true, cube->transform);
	auto t3 = new TrueTriangleComponent(Device, Context, cam,
		Vector3(-4, 0, 0), 
		new Vector4[]{
			Vector4(-1,0,1,1), Vector4(1.0f, 0.0f, 0.0f, 1.0f),
			Vector4(-2,0,0,1), Vector4(1.0f, 0.0f, 0.0f, 1.0f),
			Vector4(-1,0,-1,1), Vector4(1.0f, 0.0f, 0.0f, 1.0f)
		},
		false, cube->transform);
	auto t4 = new TrueTriangleComponent(Device, Context, cam,
		Vector3(0, 0, -4),
		new Vector4[]{
			Vector4(-1,0,-1,1), Vector4(1.0f, 0.0f, 0.0f, 1.0f),
			Vector4(0,0,-2,1), Vector4(1.0f, 0.0f, 0.0f, 1.0f),
			Vector4(1,0,-1,1), Vector4(1.0f, 0.0f, 0.0f, 1.0f)
		},
		true, cube->transform);
	Components.push_back(t1);
	Components.push_back(t2);
	Components.push_back(t3);
	Components.push_back(t4);
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