#define _USE_MATH_DEFINES

#include "lib.h"
#include "../GameManager.h"
#include "wtypes.h"
#include "fmod/api/inc/fmod.h"
#include <d3d9.h>
#include <d3dx9.h>
#include <strsafe.h>
#include <math.h>
#include <map>
#include <direct.h>
#include <malloc.h>
#include <string>

#pragma comment(lib,"lib/fmod/api/lib/fmodvc.lib")
#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"d3dx9.lib")

// Custom Flexible Vertex Format for CustomVertex
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZRHW|D3DFVF_DIFFUSE|D3DFVF_TEX1)

struct CustomVertex
{
	FLOAT x, y, z, rhw;
	DWORD color;
	float u, v;
};

struct Point
{
	FLOAT x;
	FLOAT y;
};

static LPDIRECT3D9 D3DInterface = NULL;
static LPDIRECT3DDEVICE9 D3DDevice = NULL;
static LPDIRECT3DVERTEXBUFFER9 D3DVertexBuffer = NULL;

static bool fullscreen;

static int frameTime;

static FSOUND_STREAM* music;

static std::map<std::string, IDirect3DTexture9*> spriteMap;
static std::map<std::string, FSOUND_SAMPLE*> soundMap;

static LARGE_INTEGER startTime;
static LARGE_INTEGER freq;

extern HWND hWnd;
static HWND hWnd;

static bool keyDown[256];
static int keyHit[256];

static std::string audioAssetsPath = "../assets/audio/";
static std::string gfxAssetsPath = "../assets/gfx/";

HRESULT Lib::InitD3D( HWND hWnd )
{
    // Create the D3D object.
    if( NULL == ( D3DInterface = Direct3DCreate9( D3D_SDK_VERSION ) ) )
        return E_FAIL;

    // Set up the structure used to create the D3DDevice
    D3DPRESENT_PARAMETERS d3dPresentParameters;
    ZeroMemory( &d3dPresentParameters, sizeof(d3dPresentParameters) );
    d3dPresentParameters.Windowed = !fullscreen;
	d3dPresentParameters.SwapEffect = fullscreen? D3DSWAPEFFECT_FLIP : D3DSWAPEFFECT_DISCARD;
	d3dPresentParameters.PresentationInterval = D3DPRESENT_INTERVAL_ONE ;
	d3dPresentParameters.BackBufferFormat = fullscreen ? D3DFMT_A8R8G8B8 : D3DFMT_UNKNOWN;
	d3dPresentParameters.BackBufferWidth = k_windowWidth;
	d3dPresentParameters.BackBufferHeight = k_windowHeight;
	d3dPresentParameters.FullScreen_RefreshRateInHz = fullscreen? 60 : 0;

    // Create the D3DDevice
    if( FAILED( D3DInterface->CreateDevice(D3DADAPTER_DEFAULT,
										   D3DDEVTYPE_HAL,
										   hWnd,
										   D3DCREATE_SOFTWARE_VERTEXPROCESSING,
										   &d3dPresentParameters,
										   &D3DDevice)))
    {
        return E_FAIL;
    }

    // Device state would normally be set here

    return S_OK;
}

HRESULT Lib::InitVB()
{
  
    if( FAILED( D3DDevice->CreateVertexBuffer( 4*sizeof(CustomVertex),
                                                  D3DUSAGE_DYNAMIC, D3DFVF_CUSTOMVERTEX,
                                                  D3DPOOL_DEFAULT, &D3DVertexBuffer, NULL ) ) )
    {
        return E_FAIL;
    }

    return S_OK;
}

VOID Lib::Cleanup()
{
	if (D3DVertexBuffer != NULL)
	{
		D3DVertexBuffer->Release();
	}

	if (D3DDevice != NULL)
	{
		D3DDevice->Release();
	}

	if (D3DInterface != NULL)
	{
		D3DInterface->Release();
	}

	for each (const auto& sprite in spriteMap)
	{
		sprite.second->Release();
	}
	spriteMap.clear();

	for each (const auto & sound in soundMap)
	{
		FSOUND_Sample_Free(sound.second);
	}
	soundMap.clear();

	GameManager::Instance()->DeleteInstance();
}

LRESULT WINAPI Lib::MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
    switch( msg )
    {		
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		keyDown[wParam&255]=true;
		keyHit[wParam&255]++;
		return 0;
	case WM_KEYUP:
	case WM_SYSKEYUP:
		keyDown[wParam&127]=false;
		break;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
	case WM_ACTIVATEAPP:
		if (!wParam)
		{
			memset(keyDown,0,sizeof(keyDown));
		}
		break;

	default:
		break;
    }


    return DefWindowProc( hWnd, msg, wParam, lParam );
}

INT WINAPI Lib::Start( HINSTANCE hInst, HINSTANCE, LPSTR cmd, INT )
{
	int id = MessageBox(NULL,"Enable fullscreen?","Outer Space",MB_YESNOCANCEL);
	if (id==IDCANCEL) return 0;
	fullscreen=(id==IDYES);

    // Register the window class
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, MsgProc, 0L, 0L,
                      GetModuleHandle(NULL), NULL, NULL, NULL, NULL,
                      "outerSpace", NULL };
    RegisterClassEx( &wc );

	RECT r={0, 0, k_windowWidth, k_windowHeight};
	int style = fullscreen ? WS_POPUP : WS_OVERLAPPEDWINDOW;
	style|=WS_VISIBLE;
	
	AdjustWindowRect(&r,style,false);

    // Create the application's window
    hWnd = CreateWindow("outerSpace", "Outer Space",
                        style, 0,0,r.right-r.left,r.bottom-r.top,
                        GetDesktopWindow(), NULL, wc.hInstance, NULL );
	
	FSOUND_Init(44100, 42, 0);

	QueryPerformanceCounter(&startTime);
	QueryPerformanceFrequency(&freq);

	if (SUCCEEDED(SetupD3D()))
	{
		StartGame();
	}
	else
	{
		int errorBox = MessageBox(NULL, "Error setting up D3D", "Outer Space", MB_OK);
		if (errorBox == IDOK) return 0;
	}

    UnregisterClass( "outerSpace", wc.hInstance );
    return 0;
}

HRESULT Lib::SetupD3D()
{
	if (SUCCEEDED(InitD3D(hWnd)))
	{
		if (SUCCEEDED(InitVB()))
		{
			RECT desktop;
			const HWND hDesktop = GetDesktopWindow();
			GetWindowRect(hDesktop, &desktop);
			int screenWidth = desktop.right;
			int screenHeight = desktop.bottom;

			//center the window on the screen
			int windowPosX = static_cast<int>(screenWidth * 0.5f - k_windowWidth * 0.5f);
			int windowPosY = static_cast<int>(screenHeight * 0.5f - k_windowHeight * 0.5f);

			UINT flags = SWP_NOZORDER | SWP_NOACTIVATE | SWP_ASYNCWINDOWPOS;
			SetWindowPos(hWnd, NULL, windowPosX, windowPosY, k_windowWidth, k_windowHeight, flags);
			SetCursor(LoadCursor(NULL, IDC_ARROW));

			//disable window resizing
			SetWindowLong(hWnd, GWL_STYLE, GetWindowLong(hWnd, GWL_STYLE) & ~WS_SIZEBOX);

			// Show the window
			ShowWindow(hWnd, SW_SHOWDEFAULT);
			UpdateWindow(hWnd);

			return S_OK;
		}
	}

	return E_FAIL;
}

bool Lib::QuitRequested()
{
	bool result = IsKeyDown(VK_ESCAPE);

	if (!result)
	{
		MSG msg;
		ZeroMemory(&msg, sizeof(msg));
		while (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			if (msg.message == WM_QUIT)
			{
				result = true;
				break;
			}
		}
	}

	if (result)
	{
		Cleanup();
	}

	return result;
}

void Lib::BeginFrameDraw(DWORD clearcol)
{
	D3DDevice->Clear(0, NULL, D3DCLEAR_TARGET, clearcol, 1.0f, 0);
	D3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	D3DDevice->SetRenderState(D3DRS_ZENABLE, false);

	D3DDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	D3DDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	D3DDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_DIFFUSE);
	D3DDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TEXTURE);
	D3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);

	D3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
	D3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TEXTURE);
	D3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);

	D3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	D3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	D3DDevice->SetRenderState(D3DRS_LIGHTING, false);
	D3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	D3DVIEWPORT9 vp = { 0,0, k_windowWidth, k_windowHeight, 0, 1 };

	D3DDevice->SetFVF(D3DFVF_CUSTOMVERTEX);

	D3DDevice->BeginScene();
}

void Lib::EndFrameDraw()
{
	D3DDevice->EndScene();

	frameTime = GetTimeInMS();

	D3DDevice->Present(NULL, NULL, NULL, NULL);
	Sleep(0);
	memset(keyHit, 0, sizeof(keyHit));
	SetCursor(LoadCursor(NULL, IDC_ARROW));
}

// Since start of program
unsigned int Lib::GetTimeInMS()
{
	LARGE_INTEGER performanceCount;
	QueryPerformanceCounter(&performanceCount);
	return static_cast<int>(((performanceCount.QuadPart-startTime.QuadPart)*1000) / freq.QuadPart);
}

float Lib::GetTimeSinceLastFrameInSeconds()
{
	return (GetTimeInMS() - frameTime) * 0.001f;
}

float Lib::GetRandomRotationInRads()
{
	return (static_cast<float>(rand()) / RAND_MAX) * 2 * M_PI;
}

float Lib::Lerp(float initValue, float finalValue, float step)
{
	return initValue + step * (finalValue - initValue);
}

void Lib::StartGame()
{
	GameManager::Instance()->Start();
}

bool Lib::IsKeyDown(int key)
{
	return keyDown[key&255];
}

IDirect3DTexture9* Lib::LoadSprite(std::string fname)
{
	std::map<std::string, IDirect3DTexture9*>::iterator it = spriteMap.find(fname);
	if (it != spriteMap.end())
	{
		return it->second;
	}
	else
	{
		IDirect3DTexture9* tex = NULL;
		D3DXCreateTextureFromFile(D3DDevice, (gfxAssetsPath + fname).c_str(), &tex);

		spriteMap.emplace(fname, tex);
		return tex;
	}	
}

void Lib::SetCurrentTexture(void *tex )
{
	IDirect3DTexture9 *texture = (IDirect3DTexture9 *)tex;
	D3DDevice->SetTexture(0,texture);
}

Point Lib::Rotate(float xSize, float ySize, float xCenter, float yCenter, float cosValue, float sinValue)
{
	Point result = { static_cast<FLOAT>(xCenter + (xSize)*cosValue + (ySize)*sinValue),
					 static_cast<FLOAT>(yCenter + (ySize)*cosValue - (xSize)*sinValue) };

	return result;
}

void Lib::DrawSprite(void *sprite,
					 float xCenter,
					 float yCenter,
					 float xSize,
					 float ySize,
					 float angle,
					 DWORD col )
{
	if (sprite == nullptr) return;

	SetCurrentTexture(sprite);
	float cosValue = cosf(angle);
	float sinValue = sinf(angle);

	Point rot1 = Rotate(-xSize, -ySize, xCenter, yCenter, cosValue, sinValue);
	Point rot2 = Rotate( xSize, -ySize, xCenter, yCenter, cosValue, sinValue);
	Point rot3 = Rotate(-xSize,  ySize, xCenter, yCenter, cosValue, sinValue);
	Point rot4 = Rotate( xSize,  ySize, xCenter, yCenter, cosValue, sinValue);

	CustomVertex spriteVertex[] =
	{		
		{ rot1.x, rot1.y, 0.5f, 1.0f, col, 0,0, },
		{ rot2.x, rot2.y, 0.5f, 1.0f, col, 1,0, },
		{ rot3.x, rot3.y, 0.5f, 1.0f, col, 0,1, },
		{ rot4.x, rot4.y, 0.5f, 1.0f, col, 1,1, },
	};

	D3DDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, spriteVertex, sizeof(CustomVertex));
}

int Lib::PlayMusic(std::string fname, float volume)
{
	if (music)
	{
		StopMusic();
	}

	music = FSOUND_Stream_Open((audioAssetsPath + fname).c_str(),FSOUND_LOOP_NORMAL,0,0);
	int chan = FSOUND_Stream_Play(FSOUND_FREE,music);

	if (volume <= 0)
	{
		volume = 0;
	}
	else if (volume > 1)
	{
		volume = 1;
	}

	FSOUND_SetVolume(chan, (int)(volume*255));
	return chan;
}

void Lib::StopMusic()
{
	if (music)
	{
		FSOUND_Stream_Close(music);
	}
	music=NULL;
}

FSOUND_SAMPLE* Lib::LoadAudioClip(std::string fname, bool looped)
{
	std::map<std::string, FSOUND_SAMPLE*>::iterator it = soundMap.find(fname);
	if (it != soundMap.end())
	{
		return it->second;
	}
	else
	{
		int flags = 0;
		if (looped)
		{
			flags |= FSOUND_LOOP_NORMAL;
		}

		FSOUND_SAMPLE* newAudioClip = FSOUND_Sample_Load(FSOUND_FREE, (audioAssetsPath + fname).c_str(), flags, 0, 0);
		soundMap.emplace(fname, newAudioClip);
		return newAudioClip;
	}
}

int Lib::PlayAudioClip(void *sound, float volume)
{
	if (!sound) return -1;

	if (volume <= 0)
	{
		volume = 0;
	}
	else if (volume > 1)
	{
		volume = 1;
	}

	int chan = FSOUND_PlaySound(FSOUND_FREE,(FSOUND_SAMPLE*)sound);
	FSOUND_SetVolume(chan, (int)(volume*255));

	return chan;
}

int Lib::LoadAndPlayAudioClip(std::string fname, bool loop, float volume)
{
	return PlayAudioClip(LoadAudioClip(fname, loop), volume);
}

void Lib::StopAudioClip(int handle)
{
	if (handle <= 0) return;

	FSOUND_StopSound(handle);
}
