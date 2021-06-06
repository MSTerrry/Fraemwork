
#include <Windows.h>
//#include "TransformGame.h"
//#include "LightTextureGame.h"
#include "CubeWithTrianglesGame.h"
HINSTANCE hInstance; HINSTANCE hPrevInst;
LPSTR lpszArgs; int nWinMode;

int main() {
	CubeWithTrianglesGame g(hInstance, hPrevInst, lpszArgs, nWinMode);
	g.Initialize();
	g.Run();
	return 0;
}