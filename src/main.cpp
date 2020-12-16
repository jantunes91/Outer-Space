#include "lib/lib.h"
#include <windows.h>
#include <time.h> 

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifdef _DEBUG
#ifndef DBG_NEW
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
#define new DBG_NEW
#endif
#endif

INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE hInst2, LPSTR cmd, INT integer)
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	srand(static_cast<unsigned int>(time(0)));
	return Lib::Start(hInst, hInst2, cmd, integer);
}
