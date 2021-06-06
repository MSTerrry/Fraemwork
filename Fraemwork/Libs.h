#pragma once

#include <vector>
#include <d3d.h>
#include <d3d11.h>
#include <d3d11_1.h>
#include <d3d11_4.h>
#include <DirectXColors.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include <iostream>
#include "GameComponent.h"
#include "winuser.h"
#include "SimpleMath.h"
#include "Camera.h"
#include "Keys.h"

#define ZCHECK(exp) if(FAILED(exp)) { printf("Check failed at file: %s at line %i", __FILE__, __LINE__); return res; }
