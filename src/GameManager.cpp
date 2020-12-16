#include "GameManager.h"
#include "TitleScreen.h"
#include "Game.h"
#include "GameOver.h"
#include "lib/lib.h"
#include <string>
#include <d3dx9.h>

const float k_backgroundPositionX = k_windowWidth * 0.5f;
const float k_backgroundPositionY = k_windowHeight * 0.5f;
const float k_backgroundSizeX = k_windowWidth * 0.6f;
const float k_backgroundSizeY = k_windowHeight * 0.6f;

GameManager* GameManager::instance = 0;

GameManager::GameManager()
{
	titleScreen = nullptr;
	game = nullptr;
	gameOver = nullptr;
	state = GameState::None;

	cleanTitleScreen = false;
	cleanGame = false;
	cleanGameOver = false;

	backgroundGraphic = Lib::LoadSprite("background.png");
}

GameManager::~GameManager()
{
	state = GameState::None;

	if (titleScreen != nullptr)
	{
		delete titleScreen;
	}

	if (game != nullptr)
	{
		delete game;
	}

	if (gameOver != nullptr)
	{
		delete gameOver;
	}

	Lib::StopMusic();
}

GameManager* GameManager::Instance()
{
	if (instance == nullptr)
	{
		instance = new GameManager();
	}

	return instance;
}

void GameManager::DeleteInstance()
{
	delete instance;
	instance = nullptr;
}

void GameManager::Start()
{
	ShowTitleScreen();

	Lib::PlayMusic("background.wav", 0.6f);

	while (true)
	{
		if (Lib::QuitRequested()) break;

		Lib::BeginFrameDraw();
		DrawBackground();
		CleanupStates();
		Tick();
		Lib::EndFrameDraw();
	}
}

void GameManager::IncrementScore(int delta)
{
	if (game != nullptr)
	{
		game->IncrementScore(delta);
	}
}

Game* GameManager::CurrentGame()
{
	return game;
}

void GameManager::Tick()
{
	switch (state)
	{
	case GameManager::GameState::TitleScreen:
		titleScreen->Tick();
		break;
	case GameManager::GameState::Game:
		game->Tick();
		break;
	case GameManager::GameState::GameOver:
		gameOver->Tick();
		break;
	case GameManager::GameState::None:
	default:
		break;
	}
}

void GameManager::DrawBackground()
{
	Lib::DrawSprite(backgroundGraphic,
						k_backgroundPositionX,
						k_backgroundPositionY,
						k_backgroundSizeX,
						k_backgroundSizeY);
}

void GameManager::StartGame()
{
	state = GameState::Game;

	cleanTitleScreen = true;
	cleanGameOver = true;

	game = new Game();
}

void GameManager::EndGame()
{
	state = GameState::GameOver;

	game->SaveHiScore();
	gameOver = new GameOver(game->CurrentScore(), game->IsHiScore());

	cleanGame = true;
}

void GameManager::ShowTitleScreen()
{
	state = GameState::TitleScreen;

	titleScreen = new TitleScreen();
}

// In order to avoid clearing structures mid tick,
// we flag them and delete them on the next tick
void GameManager::CleanupStates()
{
	if (cleanTitleScreen)
	{
		if (titleScreen != nullptr)
		{
			delete titleScreen;
			titleScreen = nullptr;
		}
		cleanTitleScreen = false;
	}

	if (cleanGame)
	{
		if (game != nullptr)
		{
			delete game;
			game = nullptr;
		}
		cleanGame = false;
	}

	if (cleanGameOver)
	{
		if (gameOver != nullptr)
		{
			delete gameOver;
			gameOver = nullptr;
		}
		cleanGameOver = false;
	}
}
