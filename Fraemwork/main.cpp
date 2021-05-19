
#include <Windows.h>
#include "Game.h"

HINSTANCE hInstance; HINSTANCE hPrevInst;
LPSTR lpszArgs; int nWinMode;

int main() {
	Game g(hInstance, hPrevInst, lpszArgs, nWinMode);
	g.Initialize();
	return 0;
}