#pragma once

#include <vector>

class TitleScreen;
class Game;
class GameOver;

class IDirect3DTexture9;

class GameManager
{
public:
	GameManager();
	~GameManager();

	static GameManager* Instance();
	static void DeleteInstance();

	void Start();

	void StartGame();
	void EndGame();
	void IncrementScore(int delta);

	Game* CurrentGame();

private:
	enum class GameState
	{
		None,
		TitleScreen,
		Game,
		GameOver
	};

	static GameManager* instance;

	GameState state;

	TitleScreen* titleScreen;
	Game* game;
	GameOver* gameOver;

	bool cleanTitleScreen;
	bool cleanGame;
	bool cleanGameOver;

	IDirect3DTexture9* backgroundGraphic;

	void Tick();

	void DrawBackground();

	void ShowTitleScreen();
	void CleanupStates();
};

