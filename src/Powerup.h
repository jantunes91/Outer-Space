#pragma once

class IDirect3DTexture9;

const float k_powerupSizeX = 20;
const float k_powerupSizeY = 20;

enum class PowerupType
{
	None,
	ExtraLife,
	IncreaseFireRate
};

class Powerup
{
public:
	PowerupType type;

	bool toDestroy;

	float positionX;
	float positionY;

	Powerup(float posX, float posY);

	void Tick();

	bool OutsideViewport();

protected:
	IDirect3DTexture9* sprite;
};

