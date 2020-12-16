#pragma once

class IDirect3DTexture9;

const float k_bulletSizeX = 5;
const float k_bulletSizeY = 10;

const int k_bulletEnemyColor = 0xFFFF0000;
const int k_bulletPlayerColor = 0xFF00BE00;

class Bullet
{
public:
	bool toDestroy;

	float positionX;
	float positionY;

	Bullet();

	void Tick();

	bool OutsideViewport();

protected:
	unsigned long color;
	int bulletDirectionModifier;

private:
	IDirect3DTexture9* sprite;
};

