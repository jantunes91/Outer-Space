#include "TitleScreen.h"
#include "GameManager.h"
#include "lib/lib.h"
#include <cmath>
#include <d3dx9.h>

const float k_finalTitleWidth = 336;
const float k_finalTitleHeigth = 120;

const float k_buttonPromptWidth = 228;
const float k_buttonPromptHeigth = 24;

const unsigned int k_titleAnimTimeInMS = 1000;
const unsigned int k_timeToShowButtonPromp = static_cast<int>(k_titleAnimTimeInMS * 1.5f);

TitleScreen::TitleScreen()
{
	isStarting = false;
	buttonPressedTimeStamp = 0;

	initTimeStamp = Lib::GetTimeInMS();

	titleGraphic = Lib::LoadSprite("TitleScreen/title.png");
	buttonPromptGraphic = Lib::LoadSprite("TitleScreen/buttonPrompt.png");
}

void TitleScreen::Tick()
{
	int currentTime = Lib::GetTimeInMS();

	unsigned int elapsedTime = currentTime - initTimeStamp;
	
	DrawTitle(elapsedTime);
	if (elapsedTime > k_timeToShowButtonPromp + 0.5f)
	{
		CheckButton();
		if (!isStarting)
		{
			DrawButtonPrompt(elapsedTime);
		}
	}	
}

void TitleScreen::CheckButton()
{
	if (!isStarting)
	{
		if (Lib::IsKeyDown(VK_SPACE))
		{
			isStarting = true;
			buttonPressedTimeStamp = Lib::GetTimeInMS();
		}
	}
	else
	{
		int currentTime = Lib::GetTimeInMS();

		if (currentTime - buttonPressedTimeStamp > 1000)
		{
			GameManager::Instance()->StartGame();
		}
	}
}

void TitleScreen::DrawTitle(unsigned int elapsedTime)
{
	float scale = 1;
	if (elapsedTime < k_titleAnimTimeInMS)
	{
		float f = static_cast<float>(elapsedTime) / static_cast<float>(k_titleAnimTimeInMS);
		scale = Lib::Lerp(0.0f, 1.0f, f);
	}
	Lib::DrawSprite(titleGraphic,
						k_windowWidth * 0.5f,
						k_windowHeight * 0.5f,
						k_finalTitleWidth * scale,
						k_finalTitleHeigth * scale);
}

void TitleScreen::DrawButtonPrompt(unsigned int elapsedTime)
{
	//make it blink
	bool show = (elapsedTime % 2000) < 1000;
	if (show)
	{
		Lib::DrawSprite(buttonPromptGraphic,
							k_windowWidth * 0.5f,
							k_windowHeight * 0.85f,
							k_buttonPromptWidth,
							k_buttonPromptHeigth);
	}	
}
