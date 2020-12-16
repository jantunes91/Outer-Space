#pragma once

class IDirect3DTexture9;

const float k_enemySizeX = 26;
const float k_enemySizeY = 20;

class Enemy
{
public:
	float positionX;
	float positionY;

	Enemy();

	void Draw();
	void Die();
	bool IsAlive();

	int ScoreValue();

protected:
	IDirect3DTexture9* shipSprite;
	unsigned int scoreValue;

private:
	bool alive;
	unsigned int deathTimeStamp;
	float explosionRotation;
};

