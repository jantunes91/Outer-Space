#pragma once

#include <vector>

class IDirect3DTexture9;

class TitleScreen
{
public:
	TitleScreen();

	void Tick();

private:
	bool isStarting;

	unsigned int initTimeStamp;
	unsigned int buttonPressedTimeStamp;

	IDirect3DTexture9* titleGraphic;
	IDirect3DTexture9* buttonPromptGraphic;

	void DrawTitle(unsigned int elapsedTime);

	void CheckButton();	
	void DrawButtonPrompt(unsigned int elapsedTime);
};

