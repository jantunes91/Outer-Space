#pragma once

#include <vector>

class IDirect3DTexture9;

class GameOver
{
public:
	GameOver(int score, bool isHiScore);

	void Tick();

private:
	bool isStarting;

	unsigned int initTimeStamp;
	unsigned int buttonPressedTimeStamp;

	unsigned int finalScore;
	
	bool audioPlayed;
	bool showHiScoreMessage;

	IDirect3DTexture9* gameOverGraphic;
	IDirect3DTexture9* finalScoreGraphic;
	IDirect3DTexture9* newHiScoreGraphic;
	IDirect3DTexture9* buttonPromptGraphic;
	std::vector<IDirect3DTexture9*> scoreNumberGraphics;

	void DrawGameOverMessage();

	void DrawFinalScoreMessage();
	void DrawNewHiScoreMessage();

	void DrawScore();

	void DrawButtonPrompt(unsigned int elapsedTime);
	void CheckButton();
};

