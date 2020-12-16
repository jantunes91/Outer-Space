#include "EnemyFleet.h"
#include "GameManager.h"
#include "Game.h"
#include "Enemy.h"
#include "CrabEnemy.h"
#include "OctopusEnemy.h"
#include "SquidEnemy.h"
#include "Bullet.h"
#include "EnemyBullet.h"
#include "lib/lib.h"
#include <math.h>

const int k_numberOfEnemies = 50;
const int k_enemiesPerRow = 10;

const float k_directionChangeCooldown = 200.0f;

EnemyFleet::EnemyFleet(float fleetSpeed, float fleetFireRate)
{
	directionChangeTimeStamp = Lib::GetTimeInMS();
	fireTimeStamp = Lib::GetTimeInMS();
	speed = fleetSpeed;
	fireRate = fleetFireRate;

	enemies.reserve(k_numberOfEnemies);

	//randomize starting direction
	xMoveDirectionFactor = rand() % 2 == 1 ? 1 : -1;

	for (int n = 0; n < k_numberOfEnemies; ++n)
	{
		Enemy* enemy;
		if (n < k_enemiesPerRow)
		{
			enemy = new SquidEnemy();
		}
		else if (n < k_enemiesPerRow * 3)
		{
			enemy = new OctopusEnemy();
		}
		else
		{
			enemy = new CrabEnemy();
		}

		enemy->positionX = static_cast<float>((n % 10) * 60 + 120);
		enemy->positionY = static_cast<float>((n / 10) * 60 + 70);

		enemies.emplace_back(enemy);
	}
}

EnemyFleet::~EnemyFleet()
{
	for each (Bullet* bullet in bullets)
	{
		delete bullet;
	}

	for each (Enemy* enemy in enemies)
	{
		delete enemy;
	}
}

void EnemyFleet::Tick(const std::vector<Bullet*>& playerBullets)
{
	CheckForBulletHits(playerBullets);

	Move();
	MaybeFire();
	UpdateBulletPositions();
}

bool EnemyFleet::IsDefeated()
{
	return AliveEnemies().size() == 0;
}

std::vector<Enemy*> EnemyFleet::AliveEnemies()
{
	std::vector<Enemy*> aliveEnemies;

	for each (Enemy* enemy in enemies)
	{
		if (enemy->IsAlive())
		{
			aliveEnemies.emplace_back(enemy);
		}
	}

	return aliveEnemies;
}

std::vector<Enemy*> EnemyFleet::EnemiesThatCanShoot()
{
	std::vector<Enemy*> enemiesThatCanShoot;
	for (int i = k_numberOfEnemies - k_enemiesPerRow; i < k_numberOfEnemies; i++)
	{
		for (int j = 0; j < k_numberOfEnemies / k_enemiesPerRow; j++)
		{
			int index = i - j * k_enemiesPerRow;
			if (enemies[index]->IsAlive())
			{
				enemiesThatCanShoot.emplace_back(enemies[index]);
				break;
			}
		}
	}
	return enemiesThatCanShoot;
}

const std::vector<Bullet*>& EnemyFleet::Bullets()
{
	return bullets;
}

void EnemyFleet::Move()
{
	std::vector<Enemy*> aliveEnemies = AliveEnemies();
	MaybeChangeDirection(aliveEnemies);
	for (int n = 0; n < k_numberOfEnemies; ++n)
	{
		if (enemies[n]->IsAlive())
		{
			enemies[n]->positionX += Lib::GetTimeSinceLastFrameInSeconds() * xMoveDirectionFactor * speed;
		}
		enemies[n]->Draw();
	}
}

void EnemyFleet::MaybeChangeDirection(std::vector<Enemy*> aliveEnemies)
{
	int currentTime = Lib::GetTimeInMS();
	if (currentTime - directionChangeTimeStamp > k_directionChangeCooldown)
	{
		for each (const auto & enemy in aliveEnemies)
		{
			if (enemy->positionX - k_enemySizeX < 0 ||
				enemy->positionX + k_enemySizeX > k_windowWidth)
			{
				directionChangeTimeStamp = currentTime;
				xMoveDirectionFactor *= -1;
				break;
			}
		}
	}
}

void EnemyFleet::MaybeFire()
{
	int currentTimeStamp = Lib::GetTimeInMS();

	if (currentTimeStamp - fireTimeStamp >= fireRate)
	{
		std::vector<Enemy*> availableEnemies = EnemiesThatCanShoot();
		if (availableEnemies.size() > 0)
		{
			int n = rand() % availableEnemies.size();
			Bullet* bullet = new EnemyBullet();
			bullet->positionX = availableEnemies[n]->positionX;
			bullet->positionY = availableEnemies[n]->positionY + k_enemySizeY;
			bullets.emplace_back(bullet);
			fireTimeStamp = currentTimeStamp;
		}		
	}
}

void EnemyFleet::UpdateBulletPositions()
{
	for (unsigned int n = 0; n < bullets.size();++n)
	{
		if (!bullets[n]->toDestroy)
		{
			bullets[n]->Tick();
		}
	}
}

void EnemyFleet::CheckForBulletHits(const std::vector<Bullet*>& playerBullets)
{
	for each (const auto& bullet in playerBullets)
	{
		if (!bullet->toDestroy)
		{
			for each (Enemy* enemy in enemies)
			{
				if (enemy->IsAlive())
				{
					if (enemy->positionX - k_enemySizeX < bullet->positionX + k_bulletSizeX &&
						enemy->positionX + k_enemySizeX > bullet->positionX - k_bulletSizeX &&
						enemy->positionY - k_enemySizeY < bullet->positionY - k_bulletSizeY &&
						enemy->positionY + k_enemySizeY > bullet->positionY + k_bulletSizeY)
					{
						bullet->toDestroy = true;
						enemy->Die();
						GameManager::Instance()->CurrentGame()->NotifyEnemyDeath(enemy->positionX, enemy->positionY);
						Lib::LoadAndPlayAudioClip("enemyHit.wav");

						GameManager::Instance()->IncrementScore(enemy->ScoreValue());
					}
				}				
			}
		}
	}
}
