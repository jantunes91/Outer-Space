#pragma once

#include <windows.h>
#include <string>

class FSOUND_SAMPLE;
class IDirect3DTexture9;
class Point;
class GameManager;

typedef unsigned long DWORD;

const int k_windowWidth = 800;
const int k_windowHeight = 600;

class Lib
{
public:
	static INT WINAPI Start(HINSTANCE hInst, HINSTANCE, LPSTR cmd, INT);

	static bool QuitRequested();

	static void BeginFrameDraw(DWORD clearcolor = 0);
	static void EndFrameDraw();

	static bool IsKeyDown(int key);

	static IDirect3DTexture9* LoadSprite(std::string fname);

	static void DrawSprite(void* sprite,
						   float xcentre,
						   float ycentre,
						   float xsize,
						   float ysize,
						   float rotate_angle_radians = 0,
						   DWORD tint_col_argb = 0xffffffff);

	static int PlayMusic(std::string fname, float volume = 1);
	static void StopMusic();

	static FSOUND_SAMPLE* LoadAudioClip(std::string fname, bool loop = false);
	static int PlayAudioClip(void* sound, float volume = 1);
	static int LoadAndPlayAudioClip(std::string fname, bool loop = false, float volume = 1);
	static void StopAudioClip(int handle);

	static unsigned int GetTimeInMS();
	static float GetTimeSinceLastFrameInSeconds();

	static float GetRandomRotationInRads();

	static float Lerp(float initValue, float finalValue, float step);

private:
	static HRESULT SetupD3D();
	static HRESULT InitD3D(HWND hWnd);
	static HRESULT InitVB();

	static VOID Cleanup();

	static LRESULT WINAPI MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	static void StartGame();

	static void SetCurrentTexture(void* tex);

	static Point Rotate(float xx, float yy, float xcentre, float ycentre, float cosValue, float sinValue);
};
