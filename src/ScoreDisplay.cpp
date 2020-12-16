#include "ScoreDisplay.h"
#include "lib/lib.h"
#include <string>
#include <iostream>
#include <fstream>
#include <d3dx9.h>

const float k_scoreNumberSizeX = 16;
const float k_scoreNumberSizeY = 12;
const float k_scoreSpaceBetweenNumbers = k_scoreNumberSizeX * 2.2f;

const float k_scoreLabelPositionX = 85;
const float k_scoreLabelSizeX = 69.5f;
const float k_scoreLabelSizeY = 16.5f;

const float k_hiScoreLabelPositionX = 476;
const float k_hiScoreLabelSizeX = 91.5f;
const float k_hiScoreLabelSizeY = 16.5f;

const float k_labelPositionY = 27;
const float k_scorePositionY = 27;

const float k_scoreNumbersStartingPosition = k_scoreLabelPositionX + k_scoreLabelSizeX + k_scoreNumberSizeX * 1.5f;
const float k_hiScoreNumbersStartingPosition = k_hiScoreLabelPositionX + k_hiScoreLabelSizeX + k_scoreNumberSizeX * 1.5f;

ScoreDisplay::ScoreDisplay()
{
	score = 0;
	hiScore = 0;
	newHiScore = false;

	scoreLabelGraphics = Lib::LoadSprite("Score/scoreLabel.png");
	hiScoreLabelGraphics = Lib::LoadSprite("Score/hiScoreLabel.png");

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

	LoadHiScore();
}

void ScoreDisplay::IncrementScore(int delta)
{
	score += delta;

	if (score > hiScore)
	{
		newHiScore = true;
		hiScore = score;
	}
}

void ScoreDisplay::Tick()
{
	DrawScore();
	DrawHiScore();
}

void ScoreDisplay::DrawScore()
{	
	Lib::DrawSprite(scoreLabelGraphics,
					k_scoreLabelPositionX,
					k_labelPositionY,
					k_scoreLabelSizeX,
					k_scoreLabelSizeY);

	std::string scoreText = std::to_string(score);
	for (unsigned int n = 0; n < scoreText.size(); ++n)
	{
		int number = scoreText.at(n) - '0';
		Lib::DrawSprite(scoreNumberGraphics[number],
						n * k_scoreSpaceBetweenNumbers + k_scoreNumbersStartingPosition,
						k_scorePositionY,
						k_scoreNumberSizeX,
						k_scoreNumberSizeY);
	}
}

void ScoreDisplay::DrawHiScore()
{
	int count = 0;

	Lib::DrawSprite(hiScoreLabelGraphics,
					    k_hiScoreLabelPositionX,
					    k_labelPositionY,
					    k_hiScoreLabelSizeX,
					    k_hiScoreLabelSizeY);

	std::string scoreText = std::to_string(hiScore);
	for (unsigned int n = 0; n < scoreText.size(); ++n)
	{
		int number = scoreText.at(n) - '0';
		Lib::DrawSprite(scoreNumberGraphics[number],
						    n * k_scoreSpaceBetweenNumbers + k_hiScoreNumbersStartingPosition,
						    k_scorePositionY,
						    k_scoreNumberSizeX,
						    k_scoreNumberSizeY);
	}
}

void ScoreDisplay::LoadHiScore()
{
	std::ifstream saveFile("save.txt");
	if (saveFile.is_open())
	{
		std::string score;
		std::getline(saveFile, score);

		hiScore = std::stoi(score);
		
		saveFile.close();
	}
}

void ScoreDisplay::SaveHiScore()
{
 	std::ofstream saveFile;
	saveFile.open("save.txt");
	saveFile << std::to_string(hiScore);
	saveFile.close();
}
