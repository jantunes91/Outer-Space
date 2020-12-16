#pragma once

#include <vector>

class Enemy;
class Bullet;
class Game;

class EnemyFleet
{
public:
	EnemyFleet(float fleetSpeed, float fleetFireRate);
	~EnemyFleet();

	bool IsDefeated();
	const std::vector<Bullet*>& Bullets();
	void Tick(const std::vector<Bullet*>& playerBullets);

private:
	int xMoveDirectionFactor;
	float speed;
	unsigned int directionChangeTimeStamp;

	float fireRate;
	unsigned int fireTimeStamp;
	
	std::vector<Enemy*> enemies;
	std::vector<Bullet*> bullets;

	std::vector<Enemy*> AliveEnemies();
	std::vector<Enemy*> EnemiesThatCanShoot();

	void Move();
	void MaybeChangeDirection(std::vector<Enemy*> aliveEnemies);

	void MaybeFire();
	void UpdateBulletPositions();

	void CheckForBulletHits(const std::vector<Bullet*>& playerBullets);
};

