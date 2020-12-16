#pragma once

#include <vector>

class IDirect3DTexture9;

class ScoreDisplay
{
public:
	unsigned int score;
	bool newHiScore;

	ScoreDisplay();

	void Tick();

	void IncrementScore(int delta);	
	void SaveHiScore();

private:
	unsigned int hiScore;

	IDirect3DTexture9* scoreLabelGraphics;
	IDirect3DTexture9* hiScoreLabelGraphics;
	std::vector<IDirect3DTexture9*> scoreNumberGraphics;

	void DrawScore();
	void DrawHiScore();

	void LoadHiScore();
};

