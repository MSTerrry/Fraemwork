
#include <Windows.h>
#include "TransformGame.h"

HINSTANCE hInstance; HINSTANCE hPrevInst;
LPSTR lpszArgs; int nWinMode;

int main() {
	TransformGame g(hInstance, hPrevInst, lpszArgs, nWinMode);	
	g.Initialize();
	g.Run();
	return 0;
}