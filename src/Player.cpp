#include "Player.h"
#include "Bullet.h"
#include "PlayerBullet.h"
#include "Powerup.h"
#include "GameManager.h"
#include "lib/lib.h"
#include <d3dx9.h>

const float k_initialPlayerPositionX = k_windowWidth * 0.5f;
const float k_initialPlayerPositionY = k_windowHeight - 50;

const float k_initialLivesPositionX = 50;
const float k_livesPositionY = k_windowHeight - 20;

const float k_playerSizeX = 25;
const float k_playerSizeY = 25;

const float k_lifeGraphicSizeX = 10;
const float k_lifeGraphicSizeY = 10;

const int k_baseFireRate = 700;
const float k_minFireRateModifier = 0.285f;

const int k_initialLives = 3;
const int k_maxLives = 6;

const float k_playerSpeed = 400.0f;

const int k_powerupLifeColor = 0xFFF12727;
const int k_powerupFireRateColor = 0xFF00BE00;

const float k_powerupGlowSizeX = 31.25f;
const float k_powerupGlowSizeY = 31.25f;

const float k_timeToShowPowerup = 2000;

const float k_lifeIconScaleOffset = 2.2f;

const float k_fireRateIncrementFactor = 0.75f;

const float k_movementTilt = 0.1f;

Player::Player()
{
	fireRateModifier = 1;
	powerupTimeStamp = 0;
	powerupColor = 0;
	fireTimeStamp = Lib::GetTimeInMS();
	positionX = k_initialPlayerPositionX;
	positionY = k_initialPlayerPositionY;
	lives = k_initialLives;
	playerSprite = Lib::LoadSprite("Player/PlayerShip.png");
	powerupGlowSprite = Lib::LoadSprite("Player/PowerupGlow.png");
	lifeSprite = Lib::LoadSprite("Player/Life.png");
}

Player::~Player()
{
	Cleanup();
}

void Player::Tick(const std::vector<Bullet*>& enemyBullets,
				  const std::vector<Powerup*>& powerups,
			      bool canShoot)
{
	if (enemyBullets.size() > 0)
	{
		CheckForBulletHits(enemyBullets);
	}

	if (powerups.size() > 0)
	{
		CheckForPowerupHits(powerups);
	}
	
	MaybeMove();
	if (canShoot)
	{
		MaybeFire();
	}
	UpdateBulletPositions();
	DrawLives();
}

bool Player::MaxedOutFireRate()
{
	return fireRateModifier <= k_minFireRateModifier;
}

bool Player::MaxedOutLives()
{
	return lives >= k_maxLives;
}

void Player::MaybeMove()
{
	if (lives <= 0) return;

	float angle = 0;

	if (Lib::IsKeyDown(VK_LEFT))
	{
		positionX -= Lib::GetTimeSinceLastFrameInSeconds() * k_playerSpeed;
		angle += k_movementTilt;
	}

	if (Lib::IsKeyDown(VK_RIGHT))
	{
		positionX += Lib::GetTimeSinceLastFrameInSeconds() * k_playerSpeed;
		angle -= k_movementTilt;
	}

	// Clamp inside screen
	positionX = positionX < k_playerSizeX ? k_playerSizeX : positionX;
	positionX = positionX > k_windowWidth - k_playerSizeX ? k_windowWidth - k_playerSizeX : positionX;

	unsigned int powerupElapsedTime = Lib::GetTimeInMS() - powerupTimeStamp;
	if (powerupColor != 0 && powerupElapsedTime < k_timeToShowPowerup)
	{
		float f = powerupElapsedTime / k_timeToShowPowerup;
		float scaleFactor = Lib::Lerp(1.0f, 0.0f, f);
		Lib::DrawSprite(powerupGlowSprite, positionX, positionY, k_powerupGlowSizeX * scaleFactor, k_powerupGlowSizeX * scaleFactor, angle, powerupColor);
	}

	Lib::DrawSprite(playerSprite, positionX, positionY, k_playerSizeX, k_playerSizeY, angle);
}

const std::vector<Bullet*>& Player::Bullets()
{
	return bullets;
}

void Player::MaybeFire()
{
	if (lives <= 0) return;

	int currentTimeStamp = Lib::GetTimeInMS();

	if (Lib::IsKeyDown(VK_SPACE) && currentTimeStamp - fireTimeStamp >= k_baseFireRate * fireRateModifier)
	{
		Bullet* bullet = new PlayerBullet();
		bullet->positionX = positionX;
		bullet->positionY = positionY - k_playerSizeY;
		bullets.emplace_back(bullet);
		fireTimeStamp = currentTimeStamp;
	}
}

void Player::UpdateBulletPositions()
{
	for (unsigned int n = 0; n < bullets.size(); ++n)
	{
		if (!bullets[n]->toDestroy)
		{
			bullets[n]->Tick();
		}
	}
}

void Player::CheckForBulletHits(const std::vector<Bullet*>& enemyBullets)
{
	for each (const auto & bullet in enemyBullets)
	{
		if (bullet != nullptr && bullet->toDestroy == false)
		{
			if (positionX - k_playerSizeX < bullet->positionX + k_bulletSizeX &&
				positionX + k_playerSizeX > bullet->positionX - k_bulletSizeX &&
				positionY - k_playerSizeY < bullet->positionY - k_bulletSizeY &&
				positionY + k_playerSizeY > bullet->positionY + k_bulletSizeY)
			{
				bullet->toDestroy = true;
				lives--;
				Lib::LoadAndPlayAudioClip("playerHit.wav");
				if (lives <= 0)
				{
					GameManager::Instance()->EndGame();
					break;
				}
			}
		}
	}
}

void Player::CheckForPowerupHits(const std::vector<Powerup*>& powerups)
{
	for each (const auto & powerup in powerups)
	{
		if (powerup->toDestroy == false)
		{
			if (positionX - k_playerSizeX < powerup->positionX + k_powerupSizeX &&
				positionX + k_playerSizeX > powerup->positionX - k_powerupSizeX &&
				positionY - k_playerSizeY < powerup->positionY - k_powerupSizeY &&
				positionY + k_playerSizeY > powerup->positionY + k_powerupSizeY)
			{
				powerup->toDestroy = true;

				if (powerup->type == PowerupType::ExtraLife)
				{
					if (lives < k_maxLives)
					{
						lives++;
						powerupColor = k_powerupLifeColor;
						powerupTimeStamp = Lib::GetTimeInMS();
					}
				}
				else if (powerup->type == PowerupType::IncreaseFireRate)
				{
					if (fireRateModifier > k_minFireRateModifier)
					{
						fireRateModifier *= k_fireRateIncrementFactor;
						if (fireRateModifier < k_minFireRateModifier)
						{
							fireRateModifier = k_minFireRateModifier;
						}
						powerupColor = k_powerupFireRateColor;
						powerupTimeStamp = Lib::GetTimeInMS();
					}					
				}
			}
		}
	}
}

void Player::DrawLives()
{
	for (unsigned int i = 0; i < lives; i++)
	{
		Lib::DrawSprite(lifeSprite,
						i * k_lifeGraphicSizeX * k_lifeIconScaleOffset + k_initialLivesPositionX,
						k_livesPositionY,
						k_lifeGraphicSizeX,
						k_lifeGraphicSizeY);
	}
}

void Player::Cleanup()
{
	for each (Bullet * bullet in bullets)
	{
		delete bullet;
	}
	bullets.clear();
}
