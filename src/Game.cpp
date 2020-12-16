#include "Game.h"
#include "ScoreDisplay.h"
#include "EnemyFleet.h"
#include "Player.h"
#include "Powerup.h"
#include "ExtraLifePowerup.h"
#include "IncreaseFireRatePowerup.h"
#include "lib/lib.h"
#include <string>
#include <d3dx9.h>

const float k_initialEnemyFleetSpeed = 25.0f;
const float k_initialEnemyFleetFireRate = 3000.0f;

const float k_maxEnemyFleetSpeed = 200.0f;
const float k_minEnemyFleetFireRate = 200.0f;

const float k_waveMessageTime = 2000;
const float k_waveLabelPositionX = k_windowWidth * 0.5f;
const float k_waveLabelPositionY = k_windowHeight * 0.4f;
const float k_waveLabelSizeX = 222.5f;
const float k_waveLabelSizeY = 65.5f;

const float k_waveNumberPositionY = k_windowHeight * 0.65f;
const float k_waveNumberSizeX = 41;
const float k_waveNumberSizeY = 38;
const float k_waveSpaceBetweenNumbers = k_waveNumberSizeX * 2.2f;

const float k_powerupDropProbability = 0.1f;
const unsigned int k_maxPowerUpsPerLevel = 2;
const unsigned int k_minEnemiesForPowerup = 5;

Game::Game()
{
	currentLevel = 1;
	player = new Player();
	score = new ScoreDisplay();
	enemyFleet = nullptr;
	isBetweenLevels = false;
	newLevelTimeStamp = 0;
	waveSoundHandle = 0;
	powerupsThisLevel = 0;
	enemiesKilledThisLevel = 0;

	waveLabelGraphic = Lib::LoadSprite("Game/WaveLabel.png");

	waveNumberGraphics.reserve(10); //0 to 9
	waveNumberGraphics.emplace_back(Lib::LoadSprite("Numbers/0.png"));
	waveNumberGraphics.emplace_back(Lib::LoadSprite("Numbers/1.png"));
	waveNumberGraphics.emplace_back(Lib::LoadSprite("Numbers/2.png"));
	waveNumberGraphics.emplace_back(Lib::LoadSprite("Numbers/3.png"));
	waveNumberGraphics.emplace_back(Lib::LoadSprite("Numbers/4.png"));
	waveNumberGraphics.emplace_back(Lib::LoadSprite("Numbers/5.png"));
	waveNumberGraphics.emplace_back(Lib::LoadSprite("Numbers/6.png"));
	waveNumberGraphics.emplace_back(Lib::LoadSprite("Numbers/7.png"));
	waveNumberGraphics.emplace_back(Lib::LoadSprite("Numbers/8.png"));
	waveNumberGraphics.emplace_back(Lib::LoadSprite("Numbers/9.png"));
}

Game::~Game()
{
	delete player;
	delete enemyFleet;
	delete score;

	for each (Powerup* powerup in powerUps)
	{
		delete powerup;
	}
}

void Game::Tick()
{
	if (enemyFleet == nullptr || enemyFleet->IsDefeated())
	{
		if (!isBetweenLevels)
		{
			EnterLevelTransition();
		}
		else
		{
			MaybeStartNewLevel();
		}
	}
	else
	{
		enemyFleet->Tick(player->Bullets());
	}

	for each (Powerup* powerup in powerUps)
	{
		if (powerup != nullptr)
		{
			powerup->Tick();
		}
	}
	
	player->Tick(enemyFleet != nullptr ? enemyFleet->Bullets() : std::vector<Bullet*>(),
				 Powerups(),
				 !isBetweenLevels);

	score->Tick();
}

void Game::IncrementScore(int delta)
{
	score->IncrementScore(delta);
}

unsigned int Game::CurrentScore()
{
	return score->score;
}

bool Game::IsHiScore()
{
	return score->newHiScore;
}

void Game::SaveHiScore()
{
	score->SaveHiScore();
}

void Game::NotifyEnemyDeath(float positionX, float positionY)
{
	enemiesKilledThisLevel++;

	MaybeDropPowerup(positionX, positionY);
}

void Game::MaybeDropPowerup(float positionX, float positionY)
{
	if (enemiesKilledThisLevel < k_minEnemiesForPowerup ||
		powerupsThisLevel >= k_maxPowerUpsPerLevel) return;

	bool livesMaxed = player->MaxedOutLives();
	bool fireRateMaxed = player->MaxedOutFireRate();

	if (livesMaxed && fireRateMaxed) return;

	float randomProbability = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
	if (randomProbability <= k_powerupDropProbability * enemiesKilledThisLevel * 0.1f)
	{
		PowerupType type = PowerupType::None;;

		if (livesMaxed)
		{
			type = PowerupType::IncreaseFireRate;
		}
		else if (fireRateMaxed)
		{
			type = PowerupType::ExtraLife;
		}
		else
		{
			int randomType = rand() % 2;
			type = randomType == 0 ? PowerupType::ExtraLife : PowerupType::IncreaseFireRate;
		}

		Powerup* newPowerup = nullptr;
		if (type == PowerupType::ExtraLife)
		{
			newPowerup = new ExtraLifePowerup(positionX, positionY);
		}
		else if (type == PowerupType::IncreaseFireRate)
		{
			newPowerup = new IncreaseFireRatePowerup(positionX, positionY);
		}

		powerUps.emplace_back(newPowerup);
		powerupsThisLevel++;
	}
}

void Game::EnterLevelTransition()
{
	Cleanup();
	waveSoundHandle = Lib::LoadAndPlayAudioClip("wave.wav");
	isBetweenLevels = true;
	newLevelTimeStamp = Lib::GetTimeInMS();
}

void Game::MaybeStartNewLevel()
{
	int currentTimeStamp = Lib::GetTimeInMS();
	if (currentTimeStamp - newLevelTimeStamp >= k_waveMessageTime)
	{
		Lib::StopAudioClip(waveSoundHandle);
		isBetweenLevels = false;
		StartNewLevel();
	}
	else
	{
		DrawWave();
	}
}

void Game::StartNewLevel()
{
	float speed = static_cast<float>(k_initialEnemyFleetSpeed * pow(1.5f, currentLevel - 1));
	float fireRate = static_cast<float>(k_initialEnemyFleetFireRate * pow(0.8f, currentLevel - 1));

	if (speed > k_maxEnemyFleetSpeed)
	{
		speed = k_maxEnemyFleetSpeed;
	}

	if (fireRate < k_minEnemyFleetFireRate)
	{
		fireRate = k_minEnemyFleetFireRate;
	}

	enemyFleet = new EnemyFleet(speed, fireRate);
	powerupsThisLevel = 0;
	enemiesKilledThisLevel = 0;
	currentLevel++;
}

void Game::DrawWave()
{
	Lib::DrawSprite(waveLabelGraphic,
					    k_waveLabelPositionX,
					    k_waveLabelPositionY,
					    k_waveLabelSizeX,
					    k_waveLabelSizeY);

	std::string waveNumberText = std::to_string(currentLevel);
	float initialPos = k_waveLabelPositionX - ((waveNumberText.size() - 1) * k_waveSpaceBetweenNumbers * 0.5f);
	for (unsigned int n = 0; n < waveNumberText.size(); ++n)
	{
		int number = waveNumberText.at(n) - '0';
		Lib::DrawSprite(waveNumberGraphics[number],
						    n * k_waveSpaceBetweenNumbers + initialPos,
						    k_waveNumberPositionY,
						    k_waveNumberSizeX,
						    k_waveNumberSizeY);
	}
}

const std::vector<Powerup*>& Game::Powerups()
{
	return powerUps;
}

void Game::CleanupPowerups()
{
	auto end = std::remove_if(powerUps.begin(),
		powerUps.end(),
		[](Powerup* i) {
			return i->toDestroy;
		});

	powerUps.erase(end, powerUps.end());
}

void Game::Cleanup()
{
	player->Cleanup();

	delete enemyFleet;
	enemyFleet = nullptr;

	CleanupPowerups();
}
