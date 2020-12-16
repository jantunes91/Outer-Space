#include "GameOver.h"
#include "GameManager.h"
#include "lib/lib.h"
#include <string>
#include <d3dx9.h>

const float k_graphicsPositionX = k_windowWidth * 0.5f;

const float k_gameOverGraphicPositionY = k_windowHeight * 0.31f;
const float k_gameOverGraphicSizeX = 165.5;
const float k_gameOverGraphicSizeY = 100;

const float k_finalScoreGraphicPositionY = k_windowHeight * 0.60f;
const float k_finalScoreGraphicSizeX = 172.5;
const float k_finalScoreGraphicSizeY = 22.5;

const float k_newHiScoreGraphicSizeX = 189.5;
const float k_newHiScoreGraphicSizeY = 22.5;

const float k_buttonPromptGraphicsPositionY = k_windowHeight * 0.89f;
const float k_buttonPromptGraphicSizeX = 285;
const float k_buttonPromptGraphicSizeY = 22.5;

const int k_timeToShowButtonPromp = 1500;

const float k_scorePositionY = k_windowHeight * 0.72f;
const float k_scoreNumberSizeX = 32;
const float k_scoreNumberSizeY = 24;
const float k_scoreSpaceBetweenNumbers = k_scoreNumberSizeX * 2.2f;

GameOver::GameOver(int score, bool isHiScore)
{
	audioPlayed = false;
	isStarting = false;
	buttonPressedTimeStamp = 0;

	initTimeStamp = Lib::GetTimeInMS();

	finalScore = score;
	showHiScoreMessage = isHiScore;

	gameOverGraphic = Lib::LoadSprite("GameOver/GameOverLabel.png");
	finalScoreGraphic = Lib::LoadSprite("GameOver/FinalScoreLabel.png");
	newHiScoreGraphic = Lib::LoadSprite("GameOver/NewHiScoreLabel.png");
	buttonPromptGraphic = Lib::LoadSprite("GameOver/ButtonPromptLabel.png");

	scoreNumberGraphics.reserve(10); //0 to 9
	scoreNumberGraphics.emplace_back(Lib::LoadSprite("Numbers/0.png"));
	scoreNumberGraphics.emplace_back(Lib::LoadSprite("Numbers/1.png"));
	scoreNumberGraphics.emplace_back(Lib::LoadSprite("Numbers/2.png"));
	scoreNumberGraphics.emplace_back(Lib::LoadSprite("Numbers/3.png"));
	scoreNumberGraphics.emplace_back(Lib::LoadSprite("Numbers/4.png"));
	scoreNumberGraphics.emplace_back(Lib::LoadSprite("Numbers/5.png"));
	scoreNumberGraphics.emplace_back(Lib::LoadSprite("Numbers/6.png"));
	scoreNumberGraphics.emplace_back(Lib::LoadSprite("Numbers/7.png"));
	scoreNumberGraphics.emplace_back(Lib::LoadSprite("Numbers/8.png"));
	scoreNumberGraphics.emplace_back(Lib::LoadSprite("Numbers/9.png"));
}

void GameOver::Tick()
{
	DrawGameOverMessage();

	if (showHiScoreMessage)
	{
		DrawNewHiScoreMessage();
	}
	else
	{
		DrawFinalScoreMessage();
	}

	DrawScore();

	unsigned int currentTime = Lib::GetTimeInMS();

	unsigned int elapsedTime = currentTime - initTimeStamp;

	if (!audioPlayed && elapsedTime > 1000)
	{
		audioPlayed = true;
		Lib::LoadAndPlayAudioClip("gameOver.wav");
	}

	DrawGameOverMessage();
	if (showHiScoreMessage)
	{
		DrawNewHiScoreMessage();
	}
	else
	{
		DrawFinalScoreMessage();
	}

	DrawScore();

	if (elapsedTime > k_timeToShowButtonPromp + 0.5f)
	{
		CheckButton();
		if (!isStarting)
		{
			DrawButtonPrompt(elapsedTime);
		}
	}
}

void GameOver::DrawGameOverMessage()
{
	Lib::DrawSprite(gameOverGraphic, k_graphicsPositionX, k_gameOverGraphicPositionY, k_gameOverGraphicSizeX, k_gameOverGraphicSizeY);
}

void GameOver::DrawFinalScoreMessage()
{
	Lib::DrawSprite(finalScoreGraphic, k_graphicsPositionX, k_finalScoreGraphicPositionY, k_finalScoreGraphicSizeX, k_finalScoreGraphicSizeY);
}

void GameOver::DrawNewHiScoreMessage()
{
	Lib::DrawSprite(newHiScoreGraphic, k_graphicsPositionX, k_finalScoreGraphicPositionY, k_newHiScoreGraphicSizeX, k_newHiScoreGraphicSizeY);
}

void GameOver::DrawScore()
{
	std::string scoreText = std::to_string(finalScore);
	int initialPos = static_cast<int>(k_graphicsPositionX - ((scoreText.size() - 1) * k_scoreSpaceBetweenNumbers * 0.5f));
	for (unsigned int n = 0; n < scoreText.size(); ++n)
	{
		int number = scoreText.at(n) - '0';
		Lib::DrawSprite(scoreNumberGraphics[number],
							n * k_scoreSpaceBetweenNumbers + initialPos,
							k_scorePositionY,
							k_scoreNumberSizeX,
							k_scoreNumberSizeY);
	}
}

void GameOver::DrawButtonPrompt(unsigned int elapsedTime)
{
	//make it blink
	bool show = (static_cast<int>(elapsedTime) % 2000) < 1000;
	if (show)
	{
		Lib::DrawSprite(buttonPromptGraphic, k_graphicsPositionX, k_buttonPromptGraphicsPositionY, k_buttonPromptGraphicSizeX, k_buttonPromptGraphicSizeY);
	}
}

void GameOver::CheckButton()
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
