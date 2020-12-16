#pragma once

#include<vector>

class IDirect3DTexture9;
class ScoreDisplay;
class EnemyFleet;
class Player;
class Powerup;

class Game
{
public:
	Game();
	~Game();

	void Tick();

	unsigned int CurrentScore();
	void IncrementScore(int delta);
	
	bool IsHiScore();
	void SaveHiScore();

	void NotifyEnemyDeath(float positionX, float positionY);

private:
	unsigned int currentLevel;
	unsigned int newLevelTimeStamp;

	bool isBetweenLevels;

	IDirect3DTexture9* waveLabelGraphic;
	std::vector<IDirect3DTexture9*> waveNumberGraphics;
	unsigned int waveSoundHandle;

	Player* player;
	EnemyFleet* enemyFleet;
	ScoreDisplay* score;

	std::vector<Powerup*> powerUps;
	unsigned int powerupsThisLevel;
	unsigned int enemiesKilledThisLevel;

	void EnterLevelTransition();
	void MaybeStartNewLevel();
	void StartNewLevel();
	void DrawWave();

	const std::vector<Powerup*>& Powerups();

	void MaybeDropPowerup(float positionX, float positionY);
	void CleanupPowerups();

	void Cleanup();
};

