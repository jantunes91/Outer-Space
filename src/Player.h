#pragma once

#include <vector>

class Bullet;
class Powerup;

class IDirect3DTexture9;

class Player
{
public:
	Player();
	~Player();

	void Tick(const std::vector<Bullet*>& enemyBullets,
			  const std::vector<Powerup*>& powerups,
			  bool canShoot);

	const std::vector<Bullet*>& Bullets();

	bool MaxedOutFireRate();
	bool MaxedOutLives();

	void Cleanup();
private:
	unsigned int lives;

	float fireRateModifier;

	float positionX;
	float positionY;

	unsigned int fireTimeStamp;

	IDirect3DTexture9* playerSprite;
	IDirect3DTexture9* powerupGlowSprite;
	IDirect3DTexture9* lifeSprite;

	std::vector<Bullet*> bullets;

	int powerupColor;
	unsigned int powerupTimeStamp;

	void MaybeMove();

	void MaybeFire();
	void UpdateBulletPositions();

	void CheckForBulletHits(const std::vector<Bullet*>& enemyBullets);
	void CheckForPowerupHits(const std::vector<Powerup*>& powerups);

	void DrawLives();
};

