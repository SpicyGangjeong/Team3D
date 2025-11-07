#pragma once

#include <process.h>
#include "../Default/framework.h"
#include "Client_Enum.h"
#include "Client_Macro.h"

namespace Client
{
	const unsigned int			g_iWinSizeX = 1920;
	const unsigned int			g_iWinSizeY = 1080;

}

using namespace Client;

extern HINSTANCE g_hInstance;
extern HWND g_hWnd;

#ifdef _DEBUG
extern float g_fTimeMult;
#endif // _DEBUG
